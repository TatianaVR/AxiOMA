#ifndef LINE_H
#define LINE_H

#include <blankelement.h>

using namespace std;

class Line : public BlankElement
{
    public:
        Line();
        Line(QPointF, QPointF);
        Line(double, QPointF);
        ~Line();

        double getK();
        void setK(double);

        QString GCode() override
        {
            return codeElement + " X" + QString::number(endPoint.x()) + " Z" + QString::number(endPoint.y());
        }

        void draw(Ui::Project *ui) override
        {
            if(graph != Q_NULLPTR)
            {
                delete graph;
            }
            graph = new QCPCurve(ui->widget->xAxis, ui->widget->yAxis);
            graph->setData({startPoint.y(), endPoint.y()}, {startPoint.x(), endPoint.x()});
        }

        QListWidgetItem *createItem() override
        {
            QListWidgetItem *listItem = new QListWidgetItem;
            QString icon_name = ":/data/line/";
            endPoint.x() > startPoint.x() ? icon_name.append("up_") : icon_name.append("down_");
            endPoint.y() > startPoint.y() ? icon_name.append("right") : icon_name.append("left");
            listItem->setIcon(QIcon(icon_name));
            listItem->setSizeHint(QSize(35, 35));
            return listItem;
        }

        void setParameters(Ui::Project * ui) override
        {
            ui->ElementParametrs->setCurrentIndex(3);
            ui->L_argX_value->setValue(endPoint.x());
            ui->L_argZ_value->setValue(endPoint.y());
            ui->L_argAlpha_value->setValue(k);
        }

        void changeParameters(Ui::Project * ui) override
        {
            endPoint = QPointF(ui->L_argX_value->value(), ui->L_argZ_value->value());
            k = ui->L_argAlpha_value->value();
            draw(ui);
        }

    private:
        double k;
};

#endif // LINE_H
