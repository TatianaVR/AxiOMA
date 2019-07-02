#ifndef BLANKELEMENT_H
#define BLANKELEMENT_H

#include <QString>
#include <QPointF>
#include <qcustomplot.h>
#include <QSpinBox>
#include <ui_project.h>
#include <math_addition.h>

class BlankElement
{
    public:
        BlankElement();
        virtual ~BlankElement();

        void setCodeElement(QString);
        void setStartPoint(QPointF);
        void setStartPoint(double, double);
        void setStartPointX(double);
        void setStartPointZ(double);
        void setEndPoint(QPointF);
        void setEndPoint(double, double);
        void setEndPointX(double);
        void setEndPointZ(double);
        void setGraph(QCPCurve *);
        QString getCodeElement();
        QPointF getStartPoint();
        QPointF getEndPoint();
        QCPCurve *getGraph();

        QVector < QVector <double> > matA(double);
        double b1();

        virtual void draw(Ui::Project *) = 0;
        virtual QString GCode() = 0;
        virtual QListWidgetItem *createItem() = 0;
        virtual void setParameters(Ui::Project *) = 0;
        virtual void changeParameters(Ui::Project *) = 0;

        virtual void swapXZ();

    protected:
        QString codeElement; //код элемента
        QPointF startPoint, endPoint; //начальная и конечная точки
        QCPCurve *graph; //график элемента
};

#endif // BLANKELEMENT_H
