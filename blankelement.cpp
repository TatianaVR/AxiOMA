#include <blankelement.h>

BlankElement::BlankElement()
{
    setCodeElement("G01");
    startPoint = endPoint = QPointF();
    graph = Q_NULLPTR;
}

BlankElement::~BlankElement()
{
    delete graph;
}

void BlankElement::setCodeElement(QString code)
{
    codeElement = code;
}

void BlankElement::setStartPoint(QPointF point)
{
    startPoint = point;
}

void BlankElement::setStartPoint(double x, double y)
{
    setStartPoint(QPointF(x,y));
}

void BlankElement::setStartPointX(double x)
{
    startPoint.rx() = x;
}

void BlankElement::setStartPointZ(double z)
{
    startPoint.ry() = z;
}

void BlankElement::setEndPoint(QPointF point)
{
    endPoint = point;
}

void BlankElement::setEndPoint(double x, double y)
{
    setEndPoint(QPointF(x,y));
}

void BlankElement::setEndPointX(double x)
{
    endPoint.rx() = x;
}

void BlankElement::setEndPointZ(double z)
{
    endPoint.ry() = z;
}

void BlankElement::setGraph(QCPCurve * newGraph)
{
    graph = newGraph;
}

QString BlankElement::getCodeElement()
{
    return codeElement;
}

QPointF BlankElement::getStartPoint()
{
    return startPoint;
}

QPointF BlankElement::getEndPoint()
{
    return endPoint;
}

QCPCurve *BlankElement::getGraph()
{
    return graph;
}

QVector < QVector <double> > BlankElement::matA(double angel)
{
    return
    {
        {startPoint.y() - endPoint.y(), endPoint.x() - startPoint.x()},
        {1.0, -qTan(angel)}
    };
}

double BlankElement::b1()
{
    return endPoint.x() * startPoint.y() - startPoint.x() * endPoint.y();
}

void BlankElement::swapXZ()
{
    startPoint = QPointF(startPoint.y(), startPoint.x());
    endPoint = QPointF(endPoint.y(), endPoint.x());
}

/*void BlankElement::test(Ui::Project *window)
{
    window->test->setValue(1.0);
}*/

