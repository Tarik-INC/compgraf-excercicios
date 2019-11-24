#include "clip-rectangle.h"
#include <iostream>
#include <cmath>

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

    double posInicio = 0;
    double posFim = 1;
    Point p0 = l.mP0;
    Point p1 = l.mP1;

    // double numerador[4] = {mXmin - l.mP0.mX, l.mP0.mX - mXmax, l.mP0.mY - mYmax, mYmin - l.mP0.mY};
    // double denominador[4] = {l.mP1.mX - l.mP0.mX, l.mP0.mX - l.mP1.mX, l.mP0.mY - l.mP1.mY, l.mP1.mY - l.mP0.mY};
    double numerador;
    double denominador;
    double paramT = 0;

    for (int i = 0; i < 4; i++)
    {
        switch (i)
        {
        case 0:
            numerador = mXmin - p0.mX;
            denominador = p1.mX - p0.mX;
            break;
        case 1:
            numerador = p0.mX - mXmax;
            denominador = p0.mX - p1.mX;
            break;
        case 2:
            numerador = p0.mY - mYmax;
            denominador = p0.mY - p1.mY;
            break;
        case 3:
            numerador = mYmin - p0.mY;
            denominador = p1.mY - p0.mY;
            break;
        default:
            break;
        }
        if (denominador == 0)
        {
            // paralelo ao lado de corte;
            if (numerador > 0)
            {
                // fora da área de corte
                return NULL;
            }
        }

        paramT = numerador / denominador;

        if (denominador > 0)
        {
            // potencialmente entrando
            if (paramT > posFim)
            {
                return NULL;
            }
            {
            if (paramT > posInicio)
                posInicio = paramT;
            }
        }
        else
        {
            // potencialmente saindo

            if (paramT < posInicio)
            {
                return NULL;
            }
            if (paramT < posFim)
            {
                posFim = paramT;
            }
        }
    }

    return new Line(l.Coordinates(posInicio), l.Coordinates(posFim));
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
