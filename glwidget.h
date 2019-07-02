#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <QOpenGLShaderProgram>
#include <QVector>
#include <viewer_math.h>

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
    void mouseDoubleClickEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

private slots:
    void recieveViewType(int);
    void recievePointsData(QVector <double>, QVector <double>);
    void recieveDensity(int);
    void changeRotationAxis(int);

private:
    void draw();
    int faceAtPosition(const QPoint &pos);

    GLfloat rotationX;
    GLfloat rotationY;
    GLfloat rotationZ;
    QColor faceColors[4];
    QPoint lastPos;

    QVector <double> xPoints, zPoints;
    QVector <Point> Points, vectPoints;
    QVector <int> vectorIndex;

    int viewType, steps_u;
    double scale;

    QOpenGLShaderProgram myShader;

};

#endif // GLWIDGET_H
