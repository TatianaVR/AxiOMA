#ifndef VIEWER3D_H
#define VIEWER3D_H

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QVector>
#include <viewer_math.h>

namespace Ui {
class Viewer3D;
}

class Viewer3D : public QWidget
{
    Q_OBJECT

public:
    explicit Viewer3D(QWidget *parent = 0);
    ~Viewer3D();

private slots:

    void recievePointsData(QVector <double>, QVector <double>);

signals:
    void sendViewType(int);
    void sendPointsData(QVector <double>, QVector <double>);

private:
    Ui::Viewer3D *ui;

};

#endif // VIEWER3D_H
