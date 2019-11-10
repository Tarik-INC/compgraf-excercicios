#include "line.h"

using namespace std;

Line::Line(const Point& p0, const Point& p1)
    : mP0(p0), mP1(p1)
{}

// Dado um valor t entre 0 e 1, retorna um ponto no segmento.
// Exemplos: Para t = 0, retorna P0;
//           para t = 1, retorna P1;
//           para t = 0.5, retorna o ponto no meio do segmento.
Point Line::Coordinates(double t) const{
	Point newPoint = this->mP0 + (this->mP1 - this->mP0)*t;
    return newPoint;
    // #error Line::Coordinates não foi implementada.
}

double Line::getTForIntersectWithX(double valueX) {
    return (valueX - this->mP0.mX)/ double((this->mP1.mX - this->mP0.mX));
}
double Line::getTForIntersectWithY(double valueY) {
    return (valueY - this->mP0.mY) / double((this->mP1.mY - this->mP0.mY));
}


void Line::Read(const string& prompt)
{
    cout << prompt;
    mP0.Read("Digite as duas coordenadas do ponto inicial: ");
    mP1.Read("Digite as duas coordenadas do ponto final: ");
}

ostream& operator << (ostream& os, const Line& l)
{
    return os << l.mP0 << "=>" << l.mP1;
}


