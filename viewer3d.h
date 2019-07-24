#ifndef VIEWER3D_H
#define VIEWER3D_H

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

    void receivePointsData(QVector <double>, QVector <double>);
    void updateLightModelAttributes();
    void updateLightSettings();

    void on_EndButton_clicked();
    void on_figureColor_clicked();    
    void on_highlightColor_clicked();
    void on_ModelLight_currentIndexChanged(int index);

signals:
    void sendViewType(int);
    void sendDiffuseColor(QColor);
    void sendSpecularColor(QColor);
    void sendEyePosition(double, double, double);
    void sendLightPosition(double, double, double);
    void sendLightModelAttributes(QVector <QString>, QVector <double>);
    void sendLightSettings(QVector3D, QVector3D);
    void sendPointsData(QVector <double>, QVector <double>);

private:
    Ui::Viewer3D *ui;

    QVector <QString> attributeName;
    QVector <double> attributeValue;

    void getAttributes();

};

#endif // VIEWER3D_H
