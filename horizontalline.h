#ifndef HORIZONTALLINE_H
#define HORIZONTALLINE_H

#include <blankelement.h>

class HorizontalLine : public BlankElement
{
    public:
        HorizontalLine() : BlankElement() {}
        HorizontalLine(QPointF, QPointF);
        ~HorizontalLine();

        QString GCode() override
        {
            return codeElement + " Z" + QString::number(endPoint.y());
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
            endPoint.y() > startPoint.y() ? listItem->setIcon(QIcon(":/data/line/right.png")) : listItem->setIcon(QIcon(":/data/line/left.png"));
            listItem->setSizeHint(QSize(35, 35));
            return listItem;
        }

        void setParameters(Ui::Project * ui) override
        {
            ui->ElementParametrs->setCurrentIndex(1);
            ui->LZ_argZ_value->setValue(endPoint.y());
        }

        void changeParameters(Ui::Project * ui) override
        {
            endPoint.ry() = ui->LZ_argZ_value->value();
            draw(ui);
        }

};

#endif // HORIZONTALLINE_H
