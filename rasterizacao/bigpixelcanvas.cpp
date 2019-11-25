#include "bigpixelcanvas.h"
#include <wx/dcclient.h>

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <math.h>

using namespace std;

BEGIN_EVENT_TABLE(BigPixelCanvas, wxPanel)
EVT_PAINT(BigPixelCanvas::OnPaint)
EVT_LEFT_UP(BigPixelCanvas::OnClick)
END_EVENT_TABLE()

inline wxColour operator-(const wxColour &c1, const wxColour &c2)
{
    unsigned char red = c1.Red() - c2.Red();
    unsigned char green = c1.Green() - c2.Green();
    unsigned char blue = c1.Blue() - c2.Blue();
    return wxColour(red, green, blue);
}

inline wxColour operator*(const wxColour &c, float n)
{
    unsigned char red = c.Red() * n;
    unsigned char green = c.Green() * n;
    unsigned char blue = c.Blue() * n;
    return wxColour(red, green, blue);
}

inline wxColour operator+(const wxColour &c1, const wxColour &c2)
{
    unsigned char red = c1.Red() + c2.Red();
    unsigned char green = c1.Green() + c2.Green();
    unsigned char blue = c1.Blue() + c2.Blue();
    return wxColour(red, green, blue);
}

BigPixelCanvas::BigPixelCanvas(wxFrame *parent)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize),
      mPixelSize(5),
      mUsandoComandos(false),
      mBackgroundMode(wxSOLID),
      mColourForeground(*wxGREEN),
      mColourBackground(*wxWHITE),
      mPen(*wxBLACK_PEN)
{
    mOwnerPtr = parent;
    m_clip = false;
}

void BigPixelCanvas::DrawPixel(int x, int y, wxDC &dc)
{
    x *= mPixelSize;
    y *= mPixelSize;

    int halfPixelSize = mPixelSize / 2;
    int xStart = x - halfPixelSize;
    int xEnd = x + halfPixelSize;
    int yStart = y - halfPixelSize;
    int yEnd = y + halfPixelSize;
    for (int x = xStart; x <= xEnd; ++x)
        for (int y = yStart; y <= yEnd; ++y)
            dc.DrawPoint(x, y);
}

void BigPixelCanvas::DrawPixel(int x, int y, double z, wxDC &dc)
{
    if (mZBuffer.IsVisible(y, x, z))
    {
        x *= mPixelSize;
        y *= mPixelSize;
        int halfPixelSize = mPixelSize / 2;
        int xStart = x - halfPixelSize;
        int xEnd = x + halfPixelSize;
        int yStart = y - halfPixelSize;
        int yEnd = y + halfPixelSize;
        for (int x = xStart; x <= xEnd; ++x)
            for (int y = yStart; y <= yEnd; ++y)
                dc.DrawPoint(x, y);
    }
}

void BigPixelCanvas::DrawLine(wxPoint p0, wxPoint p1)
{
    wxClientDC dc(this);
    PrepareDC(dc);
    p0 = ConvertDeviceToLogical(p0);
    p1 = ConvertDeviceToLogical(p1);
    DrawLine(p0, p1, dc);
}

void BigPixelCanvas::DrawLine(const wxPoint &p0, const wxPoint &p1, wxDC &dc)
{
    // Aqui o codigo para desenhar uma linha.
    // Para desenhar um pixel, use "DrawPixel(x, y, dc)".

    int dx = p1.x - p0.x;
    int dy = p1.y - p0.y;

    int signalY = 1;

    int initX = p0.x;
    int initY = p0.y;
    int finalX = p1.x;
    int finalY = p1.y;

    if (dx < 0)
    {
        dx = -dx;
        dy = -dy;
        initX = finalX;
        initY = finalY;
        finalX = p0.x;
        finalY = p0.y;
    }

    if (dy < 0)
    {
        signalY = -1;
        dy = -dy;
    }

    int decision = 2 * dy - dx;
    int eastVariation = 2 * dy;
    int northeasthVariation = (dy - dx) * 2;

    DrawPixel(initX, initY, dc);

    if (dx > dy)
    {
        while (initX < finalX)
        {
            if (decision > 0)
            {
                decision += northeasthVariation;
                initY += signalY;
            }
            else
            {
                decision += eastVariation;
            }
            initX++;
            DrawPixel(initX, initY, dc);
        }
    }
    else
    {
        int decision = 2 * dx - dy;
        int eastVariation = 2 * dx;
        int northeasthVariation = (dx - dy) * 2;

        while (initY * signalY < finalY * signalY)
        {
            if (decision > 0)
            {
                decision += northeasthVariation;
                initX++;
            }
            else
            {
                decision += eastVariation;
            }
            initY += signalY;
            DrawPixel(initX, initY, dc);
        }
    }
}
// #warning BigPixelCanvas::DrawLine não foi implementado (necessário para a rasterização de segmentos de reta).

void BigPixelCanvas::DrawCircle(wxPoint center, int radius)
{
    wxClientDC dc(this);
    PrepareDC(dc);
    center = ConvertDeviceToLogical(center);
    DrawCircle(center, radius / mPixelSize, dc);
}

void BigPixelCanvas::DrawCircle(const wxPoint &center, int radius, wxDC &dc)
{
    // Aqui o codigo para desenhar um circulo.
    // Para desenhar um pixel, use "DrawPixel(x, y, dc)".
    for (int r = radius-6; r < radius; r+= 3)
    {

        // int r = radius;
        int decision = 1 - r;
        int y = r;
        int x = 0;

        int limit = round(sin(M_PI / 4) * r);

        DrawPixel(center.x, center.y + r, dc);
        DrawPixel(center.x, center.y, dc);
        DrawPixel(center.x + r, center.y, dc);
        DrawPixel(center.x - r, center.y, dc);
        DrawPixel(center.x, center.y - r, dc);

        int eastVariation = 0;
        int southeastVariation = 0;
        while (y > limit)
        {
            eastVariation = 2 * x + 3;
            southeastVariation = 2 * x - 2 * y + 5;
            if (decision < 0)
            {
                decision += eastVariation;
            }
            else
            {
                decision += southeastVariation;
                y--;
            }
            x++;

            DrawPixel(center.x + x, center.y + y, dc);
            DrawPixel(center.x + y, center.y + x, dc);
            DrawPixel(center.x + y, center.y - x, dc);
            DrawPixel(center.x + x, center.y - y, dc);
            DrawPixel(center.x - x, center.y - y, dc);
            DrawPixel(center.x - y, center.y - x, dc);
            DrawPixel(center.x - y, center.y + x, dc);
            DrawPixel(center.x - x, center.y + y, dc);
        }
    }
}

void BigPixelCanvas::DesenharTriangulo2D(const Triang2D &triangulo)
{
    wxClientDC dc(this);
    PrepareDC(dc);
    DesenharTriangulo2D(triangulo, dc);
}

void BigPixelCanvas::DesenharTriangulo2D(const Triang2D &triangulo, wxDC &dc)
{
   Ponto p0 = triangulo.P1();
   Ponto p1 = triangulo.P2();
   Ponto p2 = triangulo.P3();

   int distanceYP0P1 = abs(p0.mY - p1.mY);
   int distanceYP0P2=  abs(p0.mY - p2.mY);
   int distanceYP1P2 = abs(p2.mY - p1.mY);

   int longEdgeDistance = distanceYP0P1;
   Ponto longEdgeP0 = p0;
   Ponto longEdgeP1 = p1;
   Ponto notInLongEdge = p2;

   if(distanceYP0P2 > longEdgeDistance) {
       longEdgeDistance = distanceYP0P2;
       longEdgeP0 = p1;
       longEdgeP1 = p2;
       notInLongEdge = p1;
   }
   if(distanceYP1P2 > longEdgeDistance) {
       longEdgeDistance = distanceYP1P2;
       longEdgeP0 = p1;
       longEdgeP1 = p2;
       notInLongEdge = p0;
   }

   if(longEdgeP0.mY > longEdgeP1.mY) {
       Ponto aux = longEdgeP0;
       longEdgeP0 = longEdgeP1;
       longEdgeP1 = aux;
   }

   int minY = longEdgeP0.mY;
   int maxY = notInLongEdge.mY;
   float leftLimitX = float(longEdgeP0.mX);
   float rightLimitX = leftLimitX;
   float leftVariationX;
   float rightVariationX;
   bool inLeft = true;

    //Vertice que sobrou esta a esquerda da aresta longa
    if(notInLongEdge.mX - longEdgeP0.mX < 0) {
        // Calculo da variacao relativa (coeficiente)
        // De x( right e left limit ate as extremidades)
        rightVariationX =  float(longEdgeP1.mX - longEdgeP0.mX )/(longEdgeP1.mY - longEdgeP0.mY);
        leftVariationX = float(notInLongEdge.mX - longEdgeP0.mX) / (notInLongEdge.mY - longEdgeP0.mY);
    } else {
        rightVariationX = float(notInLongEdge.mX - longEdgeP0.mX) / (notInLongEdge.mY - longEdgeP1.mY);
        leftVariationX = float(longEdgeP1.mX - longEdgeP0.mX) / (longEdgeP1.mY - longEdgeP0.mY);
        inLeft = false;
    }

    int y = minY;
    while (y < maxY) {
        for(int x  = leftLimitX; x < rightLimitX; x ++) {
            DrawPixel(x, y, dc);
        }
        leftLimitX += leftVariationX;
        rightLimitX += rightVariationX;
        y ++;
    }

    minY = maxY;
    maxY = longEdgeP1.mY;

    if(inLeft) {
        leftVariationX = float(longEdgeP1.mX - notInLongEdge.mX) / (longEdgeP1.mY - notInLongEdge.mY);
    } else {
        rightVariationX = float(longEdgeP1.mX - notInLongEdge.mX) / (longEdgeP1.mY - notInLongEdge.mY);
    }

    while (y < maxY)
    {
        for (int x = leftLimitX; x < rightLimitX; x++)
        {
            DrawPixel(x, y, dc);
        }
        leftLimitX += leftVariationX;
        rightLimitX += rightVariationX;
        y++;
    }
}

void BigPixelCanvas::DesenharTriangulo3D(const Triang3D &triangulo, wxDC &dc)
{
    Interv3D intervalo;
    while (triangulo.AtualizarIntervaloHorizontal(&intervalo))
        if (intervalo.Valido())
            DesenharIntervaloHorizontal(intervalo, dc);
}

void BigPixelCanvas::DesenharIntervaloHorizontal(const Interv2D &intervalo, wxDC &dc)
{
    int x = intervalo.mXMin;
    while (x < intervalo.mXMax)
    {
        DrawPixel(x, intervalo.mY, dc);
        ++x;
    }
}

void BigPixelCanvas::DesenharIntervaloHorizontal(const Interv3D &intervalo, wxDC &dc)
{
// Colocar aqui o código para desenhar um intervalo horizontal 3D. Necessário
// para a implementação do z-buffer.
// Desenhar um intervalo 3D é como desenhar um intervalo 2D, usando z-buffer.
#warning BigPixelCanvas::DesenharIntervaloHorizontal não foi implementado (necessário para a rasterização do z-buffer).
}

void BigPixelCanvas::OnPaint(wxPaintEvent &event)
{
    wxPaintDC pdc(this);
    wxDC &dc = pdc;

    PrepareDC(dc);

    mOwnerPtr->PrepareDC(dc);
    dc.SetBackgroundMode(mBackgroundMode);
    if (mBackgroundBrush.Ok())
        dc.SetBackground(mBackgroundBrush);
    if (mColourForeground.Ok())
        dc.SetTextForeground(mColourForeground);
    if (mColourBackground.Ok())
        dc.SetTextBackground(mColourBackground);

    dc.Clear();
    if (mUsandoComandos)
        InterpretarComandos();
}

void BigPixelCanvas::InterpretarComandos()
{
    ifstream arquivo("comandos.txt");
    wxClientDC dc(this);
    PrepareDC(dc);
    string comando;
    while (arquivo >> comando)
    {
        if (comando == "linha")
        {
            int p0x, p0y, p1x, p1y;
            arquivo >> p0x >> p0y >> p1x >> p1y;
            DrawLine(wxPoint(p0x, p0y), wxPoint(p1x, p1y), dc);
        }
        else if (comando == "cor")
        {
            int r, g, b;
            arquivo >> r >> g >> b;
            mPen.SetColour(r, g, b);
            dc.SetPen(mPen);
        }
        else if (comando == "triangulo3d")
        {
            int x, y, z;
            arquivo >> x >> y >> z;
            P3D p1(x, y, z);
            arquivo >> x >> y >> z;
            P3D p2(x, y, z);
            arquivo >> x >> y >> z;
            P3D p3(x, y, z);
            Triang3D tri(p1, p2, p3);
            DesenharTriangulo3D(tri, dc);
        }
    }
}

void BigPixelCanvas::OnClick(wxMouseEvent &event)
{
    wxPostEvent(mOwnerPtr, event);
}

void BigPixelCanvas::PrepareDC(wxDC &dc)
{
    int height, width;
    GetClientSize(&width, &height);
    dc.SetLogicalOrigin(-width / 2, height / 2);
    dc.SetAxisOrientation(true, true);
    dc.SetMapMode(wxMM_TEXT);
    dc.SetPen(mPen);
    mZBuffer.AlterarCapacidade(static_cast<unsigned int>(height / mPixelSize),
                               static_cast<unsigned int>(width / mPixelSize));
}

wxPoint BigPixelCanvas::ConvertDeviceToLogical(const wxPoint &p)
{
    wxClientDC dc(this);
    PrepareDC(dc);
    wxPoint result;
    result.x = dc.DeviceToLogicalX(p.x) / mPixelSize;
    result.y = dc.DeviceToLogicalY(p.y) / mPixelSize;
    return result;
}
