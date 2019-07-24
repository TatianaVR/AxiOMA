#include "viewer3d.h"
#include "ui_viewer3d.h"

#include "math_addition.h"

using namespace std;

Viewer3D::Viewer3D(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Viewer3D)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::FramelessWindowHint); //нет рамки

    connect(ui->viewType, SIGNAL(currentIndexChanged(int)), ui->drawGL, SLOT(receiveViewType(int)));
    connect(ui->SetAxis, SIGNAL(currentIndexChanged(int)), ui->drawGL, SLOT(changeRotationAxis()));
    connect(ui->mesh_density, SIGNAL(valueChanged(int)), ui->drawGL, SLOT(receiveDensity(int)));
    connect(ui->ModelLight, SIGNAL(currentTextChanged(const QString)), ui->drawGL, SLOT(receiveModelLight(const QString)));
    connect(this, SIGNAL(sendPointsData(QVector <double>, QVector <double>)), ui->drawGL, SLOT(receivePointsData(QVector <double>, QVector <double>)));
    connect(this, SIGNAL(sendDiffuseColor(QColor)), ui->drawGL, SLOT(receiveDiffuseColor(QColor)));
    connect(this, SIGNAL(sendSpecularColor(QColor)), ui->drawGL, SLOT(receiveSpecularColor(QColor)));
    connect(this, SIGNAL(sendLightModelAttributes(QVector <QString>, QVector <double>)), ui->drawGL, SLOT(receiveLightModelAttributes(QVector <QString>, QVector <double>)));
    connect(this, SIGNAL(sendLightSettings(QVector3D, QVector3D)), ui->drawGL, SLOT(receiveLightSettings(QVector3D, QVector3D)));
    foreach (QDoubleSpinBox * spinBox, ui->LightAttributes->findChildren<QDoubleSpinBox *>())
    {
        connect(spinBox, SIGNAL(valueChanged(double)), this, SLOT(updateLightModelAttributes()));
    }

    foreach (QSpinBox * spinBox, ui->groupBox_4->findChildren<QSpinBox *>())
    {
        connect(spinBox, SIGNAL(valueChanged(int)), this, SLOT(updateLightSettings()));
    }

    emit ui->viewType->currentIndexChanged(ui->viewType->currentIndex());
    emit ui->ModelLight->currentTextChanged(ui->ModelLight->currentText());
    emit ui->mesh_density->valueChanged(ui->mesh_density->value());
    emit sendDiffuseColor(ui->figureColor->palette().button().color());
    emit sendSpecularColor(ui->highlightColor->palette().button().color());

    on_ModelLight_currentIndexChanged(0);
}

Viewer3D::~Viewer3D()
{
    delete ui;
}

void Viewer3D::receivePointsData(QVector <double> X, QVector <double> Z)
{
    emit sendPointsData(X, Z);
}

void Viewer3D::updateLightModelAttributes()
{
    getAttributes();
    emit sendLightModelAttributes(attributeName, attributeValue);
}

void Viewer3D::updateLightSettings()
{
    QVector3D lightPosition(ui->lightX->value(), ui->lightY->value(), ui->lightZ->value());
    QVector3D eyePosition(ui->eyeX->value(), ui->eyeY->value(), ui->eyeZ->value());
    emit sendLightSettings(lightPosition, eyePosition);
}

void Viewer3D::on_EndButton_clicked()
{
    close();
}

void Viewer3D::on_figureColor_clicked()
{
    QPalette myPalette = ui->figureColor->palette();
    QColor color = QColorDialog::getColor(myPalette.button().color());
    if (!color.isValid())
    {
        return;
    }

    myPalette.setColor(ui->figureColor->backgroundRole(), color);
    ui->figureColor->setPalette(myPalette);
    emit sendDiffuseColor(color);
}

void Viewer3D::on_highlightColor_clicked()
{
    QPalette myPalette = ui->highlightColor->palette();
    QColor color = QColorDialog::getColor(myPalette.button().color());
    if (!color.isValid())
    {
        return;
    }

    myPalette.setColor(ui->highlightColor->backgroundRole(), color);
    ui->highlightColor->setPalette(myPalette);
    emit sendSpecularColor(color);
}

void Viewer3D::on_ModelLight_currentIndexChanged(int index)
{
    ui->LightAttributes->setCurrentIndex(index);
    getAttributes();
    emit sendLightModelAttributes(attributeName, attributeValue);
}

void Viewer3D::getAttributes()
{
    attributeName.clear();
    attributeValue.clear();

    foreach (QLabel * label, ui->LightAttributes->currentWidget()->findChildren<QLabel *>())
    {
        attributeName.append(label->text());
    }

    foreach (QDoubleSpinBox * spinBox, ui->LightAttributes->currentWidget()->findChildren<QDoubleSpinBox *>())
    {
        attributeValue.append(spinBox->value());
    }
}
