#include "viewer3d.h"
#include "ui_viewer3d.h"
#include <QDebug>
#include <QColorDialog>

#define _USE_MATH_DEFINES
#include <cmath>
#include "math_addition.h"

using namespace std;

Viewer3D::Viewer3D(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Viewer3D)
{
    ui->setupUi(this);

    connect(ui->viewType, SIGNAL(currentIndexChanged(int)), ui->drawGL, SLOT(recieveViewType(int)));
    connect(ui->SetAxis, SIGNAL(currentIndexChanged(int)), ui->drawGL, SLOT(changeRotationAxis(int)));
    connect(ui->mesh_density, SIGNAL(valueChanged(int)), ui->drawGL, SLOT(recieveDensity(int)));

    emit ui->viewType->currentIndexChanged(0);
    emit ui->mesh_density->valueChanged(ui->mesh_density->value());
}

Viewer3D::~Viewer3D()
{
    delete ui;
}

void Viewer3D::recievePointsData(QVector <double> X, QVector <double> Z)
{
    emit sendPointsData(X, Z);
}
