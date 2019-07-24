#ifndef VERTICALLINE_H
#define VERTICALLINE_H

#include <blankelement.h>

using namespace std;

class VerticalLine : public BlankElement
{
    public:
        VerticalLine() : BlankElement() {}
        VerticalLine(QPointF, QPointF);
        ~VerticalLine();

        QString GCode() override
        {
            return codeElement + " X" + QString::number(endPoint.x());
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
            endPoint.x() > startPoint.x() ? listItem->setIcon(QIcon(":/data/line/up.png")) : listItem->setIcon(QIcon(":/data/line/down.png"));
            listItem->setSizeHint(QSize(35, 35));
            return listItem;
        }

        void setParameters(Ui::Project * ui) override
        {
            ui->ElementParametrs->setCurrentIndex(2);
            ui->LX_argX_value->setValue(endPoint.x());
        }

        void changeParameters(Ui::Project * ui) override
        {
            endPoint.rx() = ui->LX_argX_value->value();
            draw(ui);
        }

};

#endif // VERTICALLINE_H
