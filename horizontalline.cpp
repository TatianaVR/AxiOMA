#include <horizontalline.h>

HorizontalLine::HorizontalLine(QPointF pointA, QPointF pointB)
{
    startPoint = pointA;
    endPoint = pointB;
}

HorizontalLine::~HorizontalLine()
{
    delete graph;
}
