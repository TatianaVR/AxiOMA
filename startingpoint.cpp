#include "startingpoint.h"

StartingPoint::StartingPoint(QPointF point)
{
    setCodeElement("G01");
    startPoint = endPoint = point;
    graph = Q_NULLPTR;
}
