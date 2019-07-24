#include <verticalline.h>

VerticalLine::VerticalLine(QPointF pointA, QPointF pointB)
{
    startPoint = pointA;
    endPoint = pointB;
}

VerticalLine::~VerticalLine()
{
    delete graph;
}
