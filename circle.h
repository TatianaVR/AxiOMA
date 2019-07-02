#ifndef CIRCLE_H
#define CIRCLE_H

#include <blankelement.h>

class Circle : public BlankElement
{
    public:
        Circle();
        Circle(QString, QPointF, QPointF, QPointF);
        Circle(QString, double, QPointF, QPointF);
        ~Circle();

         void setR(double);
         double getR();
         void setArcCenter(QPointF);
         QVector <QPointF> getCircleCenters();
         QPointF getArcCenter();

         QString GCode() override
         {
             return codeElement + " X" + QString::number(endPoint.x()) + " Z" + QString::number(endPoint.y()) + " I" + QString::number(arcCenter.x()) + " K" + QString::number(arcCenter.y());
         }

         void draw(Ui::Project *ui) override
         {
             if(graph != Q_NULLPTR)
             {
                 delete graph;
             }

             if(R != 0.0)
             {
                arcCenter = centers[ui->CircleCenter->sliderPosition()];
             } else
             {
                R = QLineF(arcCenter, endPoint).length();
             }
             graph = new QCPCurve(ui->widget->xAxis, ui->widget->yAxis);

             QVector <double> xPoints, zPoints;//значения по каждой из осей
             double t1 = qAtan2(startPoint.y() - arcCenter.y(), startPoint.x() - arcCenter.x()) / M_PI * 180.0;
             double t2 = qAtan2(endPoint.y() - arcCenter.y(), endPoint.x() - arcCenter.x()) / M_PI * 180.0;

             if(codeElement == "G03")
             {
                 if (qAtan2(endPoint.y() - startPoint.y(), endPoint.x() - startPoint.x()) / M_PI * 180.0 > 0.0)
                 {
                     t1 = (t1 <= 0.0) ? t1 + 360.0 : t1;
                     t2 = (t2 < 0.0) ? t2 + 360.0 : t2;
                 }
                 else if (t2 == 180.0)
                 {
                     t2 = -t2;
                 }
             } else if(codeElement == "G02")
             {
                 if (qAtan2(endPoint.y() - startPoint.y(), endPoint.x() - startPoint.x()) / M_PI * 180.0 < 0.0)
                 {
                     t1 = (t1 < 0.0) ? t1 + 360.0 : t1;
                     t2 = (t2 <= 0.0) ? t2 + 360.0 : t2;
                 }
                 else if (t1 == 180.0)
                 {
                     t1 = -t1;
                 }
             }

             for(double t = qMin(t1, t2); t < qMax(t1, t2); t += 0.025)
             {
                 xPoints.append(arcCenter.x() + R * qCos(t * M_PI / 180.0));
                 zPoints.append(arcCenter.y() + R * qSin(t * M_PI / 180.0));
             }

             graph->setData(zPoints,xPoints);
         }

         QListWidgetItem *createItem() override
         {
             QListWidgetItem *listItem = new QListWidgetItem;
             codeElement == "G03" ? listItem->setIcon(QIcon(":/data/direction/left.png")) : listItem->setIcon(QIcon(":/data/direction/right.png"));
             listItem->setSizeHint(QSize(35, 35));
             return listItem;
         }

         void setParameters(Ui::Project * ui) override
         {
             ui->ElementParametrs->setCurrentIndex(4);
             codeElement == "G03" ? ui->DirectionButton->setStyleSheet("background: url(:/data/direction/left.png)") :
                                    ui->DirectionButton->setStyleSheet("background: url(:/data/direction/right.png)");
             ui->C_argX_value->setValue(endPoint.x());
             ui->C_argZ_value->setValue(endPoint.y());
             R = 0.0;
             ui->C_argR_value->setValue(R);
             ui->C_argI_value->setValue(arcCenter.x());
             ui->C_argJ_value->setValue(arcCenter.y());
         }

         void changeParameters(Ui::Project * ui) override
         {
             codeElement = ui->DirectionButton->styleSheet().contains("left") ? "G03" : "G02";
             endPoint = QPointF(ui->C_argX_value->value(), ui->C_argZ_value->value());
             R = ui->C_argR_value->value();
             arcCenter = QPointF(ui->C_argI_value->value(), ui->C_argJ_value->value());
             draw(ui);
         }

         void swapXZ() override
         {
             startPoint = QPointF(startPoint.y(), startPoint.x());
             endPoint = QPointF(endPoint.y(), endPoint.x());
             arcCenter = QPointF(arcCenter.y(), arcCenter.x());
             codeElement = codeElement == "G02" ? "G03" : "G02";
             R = 0.0;
         }

         void calculateCenters(Ui::Project *);

    private:
        QPointF arcCenter;
        double R;
        QVector <QPointF> centers;
};

#endif // CIRCLE_H
