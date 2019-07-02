#include <verticalline.h>

VerticalLine::VerticalLine(QPointF pointA, QPointF pointB)
{
    if(pointA.y() != pointB.y())
    {
        throw invalid_argument("Wrong Argument");
    }
    else
    {
        startPoint = pointA;
        endPoint = pointB;
    }
}

VerticalLine::~VerticalLine()
{
    delete graph;
}
