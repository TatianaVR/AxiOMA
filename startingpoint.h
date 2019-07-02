#ifndef STARTINGPOINT_H
#define STARTINGPOINT_H

#include <blankelement.h>

class StartingPoint : public BlankElement
{
    public:
        StartingPoint() : BlankElement() {}
        StartingPoint(QPointF);

    void draw(Ui::Project *ui) override
    {
        if(graph != Q_NULLPTR)
        {
            delete graph;
        }
        graph = new QCPCurve(ui->widget->xAxis, ui->widget->yAxis);
        graph->setData({startPoint.y(), endPoint.y()}, {startPoint.x(), endPoint.x()});
    }

    QString GCode() override
    {
        return codeElement + " X" + QString::number(endPoint.x()) + " Z" + QString::number(endPoint.y());
    }

    QListWidgetItem *createItem() override
    {
        QListWidgetItem *listItem = new QListWidgetItem;
        listItem->setIcon(QIcon(":/data/start.png"));
        listItem->setSizeHint(QSize(35, 35));
        return listItem;
    }

    void setParameters(Ui::Project * ui) override
    {
        ui->ElementParametrs->setCurrentIndex(0);
        ui->SP_argX_value->setValue(endPoint.x());
        ui->SP_argZ_value->setValue(endPoint.y());
    }

    void changeParameters(Ui::Project * ui) override
    {
        startPoint = endPoint = QPointF(ui->SP_argX_value->value(), ui->SP_argZ_value->value());
    }
};

#endif // STARTINGPOINT_H
