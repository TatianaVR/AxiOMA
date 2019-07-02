#include <circle.h>

Circle::Circle() : BlankElement()
{
    arcCenter = QPointF();
    R = 0.0;
}

Circle::~Circle()
{
    delete graph;
}

Circle::Circle(QString code, QPointF center, QPointF pointA, QPointF pointB)
{
    codeElement = code;
    arcCenter = center;
    startPoint = pointA;
    endPoint = pointB;
    R = 0.0;
}

Circle::Circle(QString code, double arcR, QPointF pointA, QPointF pointB)
{
    codeElement = code;
    R = arcR;
    startPoint = pointA;
    endPoint = pointB;
}

void Circle::setR(double value)
{
    R = value;
}

double Circle::getR()
{
    return QLineF(endPoint, arcCenter).length();
}

void Circle::setArcCenter(QPointF center)
{
    arcCenter = center;
}

QPointF Circle::getArcCenter()
{
    return arcCenter;
}

QVector <QPointF> Circle::getCircleCenters()
{
    return centers;
}

void Circle::calculateCenters(Ui::Project *ui)
{
    double d = sqrt(pow(startPoint.x() - endPoint.x(), 2) + pow(startPoint.y() - endPoint.y(), 2)); //расстояние прямой, проходящей через точки A и B
    double h = sqrt(pow(R, 2) - pow(d / 2., 2)); //длина высоты, проведенной от середины гепотенузы до центра окружности
    //расчёт центров дуги
    centers.append(QPointF(startPoint.x() + (endPoint.x() - startPoint.x()) / 2.0 + h * (endPoint.y() - startPoint.y()) / d,
                           startPoint.y() + (endPoint.y() - startPoint.y()) / 2.0 - h * (endPoint.x() - startPoint.x()) / d));
    centers.append(QPointF(startPoint.x() + (endPoint.x() - startPoint.x()) / 2.0 - h * (endPoint.y() - startPoint.y()) / d,
                           startPoint.y() + (endPoint.y() - startPoint.y()) / 2.0 + h * (endPoint.x() - startPoint.x()) / d));

    ui->center1->setText("C(" + QString::number(centers[0].x()) + ";" + QString::number(centers[0].y()) + ")");
    ui->center2->setText("C(" + QString::number(centers[1].x()) + ";" + QString::number(centers[1].y()) + ")");
}
