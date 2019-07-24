#ifndef GLWIDGET_H
#define GLWIDGET_H

class GLWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit GLWidget(QWidget *parent = 0);

protected:
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

    void calculatePoints();

private slots:
    void receiveViewType(int);
    void receivePointsData(QVector <double>, QVector <double>);
    void receiveDensity(int);
    void receiveModelLight(const QString);
    void receiveDiffuseColor(QColor);
    void receiveSpecularColor(QColor);
    void receiveLightModelAttributes(QVector <QString>, QVector <double>);
    void receiveLightSettings(QVector3D, QVector3D);
    void changeRotationAxis();

private:
    void drawSurface();

    GLfloat rotationX, rotationY, rotationZ;
    QColor diffuseColor, specularColor;
    QPoint lastPos;

    QVector <double> xPoints, zPoints;

    int viewType, steps_t, steps_u;
    double scale;

    QOpenGLShaderProgram myShader;

    QVector < QVector3D > GeometricArray;
    QVector < QVector < QVector3D > > ResaultGeometric;

    QVector <QString> attributeName;
    QVector <double> attributeValue;

    QVector4D lightPosition, eyePosition;

    QString lightModelName;
};

#endif // GLWIDGET_H
