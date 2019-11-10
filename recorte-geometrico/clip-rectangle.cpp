#include "clip-rectangle.h"
#include <iostream>
#include <cmath>
#include <math.h>

using namespace std;

ClipRectangle::ClipRectangle(double xmin, double xmax, double ymin, double ymax)
	: mXmin(xmin), mXmax(xmax), mYmin(ymin), mYmax(ymax), mInitialized(true)
{
}

void ClipRectangle::Set(double xmin, double xmax, double ymin, double ymax)
{
	mXmin = xmin;
	mXmax = xmax;
	mYmin = ymin;
	mYmax = ymax;
	mInitialized = true;
}

// Retorna um ponteiro para uma linha recem alocada inteiramente dentro da area de recorte
// ou NULL se l for completamente descartada.
// A linha alocada aqui será desalocada na função CleanUp().
Line *ClipRectangle::Clip(const Line &l)
{

	Line *newLine = new Line(l.Coordinates(0), l.Coordinates(1));

	unsigned char bitP0 = newLine->mP0.convertToBits(mXmin, mXmax, mYmin, mYmax);
	unsigned char bitP1 = newLine->mP1.convertToBits(mXmin, mXmax, mYmin, mYmax);
	bool lineOutRectangle = (bitP0 & bitP1) != 0;
	bool lineInRectangle = (bitP0 == 0 && bitP1 == 0);

	double limitsArray[4] = {mXmin, mXmax, mYmin, mYmax};

	double intersectT;
	int i = 0;
	while (!lineInRectangle && !lineOutRectangle && i < 4)
	{
		if (i < 2)
		{
			intersectT = newLine->getTForIntersectWithX(limitsArray[i]);
		}
		else
		{
			intersectT = newLine->getTForIntersectWithY(limitsArray[i]);
		}

		if (intersectT >= 0 && intersectT <= 1)
		{
			if ((bitP0 & int(pow(2, i))) != 0)
			{
				newLine = new Line(newLine->Coordinates(intersectT), newLine->Coordinates(1));
				bitP0 = newLine->mP0.convertToBits(mXmin, mXmax, mYmin, mYmax);
			}
			else
			{
				newLine = new Line(newLine->Coordinates(0), newLine->Coordinates(intersectT));
				bitP1 = newLine->mP1.convertToBits(mXmin, mXmax, mYmin, mYmax);
			}
		}
		i++;

		lineOutRectangle = (bitP0 & bitP1) != 0;
		lineInRectangle = (bitP0 == 0 && bitP1 == 0);

	}

	if(lineOutRectangle) {
		return NULL;
	} else if (lineInRectangle) {
		return newLine;
	}

	return newLine;
	//    #error ClipRectangle::Clip não foi implementada.
}

void ClipRectangle::Read(const string &prompt)
{
	cout << prompt;
	cin >> mXmin >> mXmax >> mYmin >> mYmax;
	if (not mInitialized)
		mInitialized = true;
}

ostream &operator<<(ostream &os, const ClipRectangle &r)
{
	return os << "(Clipping Rectangle: " << r.mXmin << ", " << r.mXmax << ", "
			  << r.mYmin << ", " << r.mYmax << ")";
}
