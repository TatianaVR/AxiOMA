#include <line.h>

Line::Line() : BlankElement()
{
    k = 0.0;
}

Line::Line(QPointF pointA, QPointF pointB)
{
    startPoint = pointA;
    endPoint = pointB;
}

Line::Line(double K, QPointF point)
{
    endPoint = point;
    k = K;
}

Line::~Line()
{
    delete graph;
}

void Line::setK(double value)
{
    k = value;
}

double Line::getK()
{
    return k;
}
