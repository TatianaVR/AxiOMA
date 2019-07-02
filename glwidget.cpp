#include "glwidget.h"

#include <QMouseEvent>
#include <QColorDialog>
#include <GL/gl.h>
#include <GL/glu.h>

#include <math_addition.h>

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(parent)
{
    setFormat(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer));

    rotationX = 90.0;
    rotationY = 90.0;
    rotationZ = 0.0;

    scale = 0.005;

    xPoints = {0, 92, 92, 92, 91.8478, 91.4142, 90.7654, 70, 71.5224, 75.8579, 82.3463, 70, 69.2388, 67.0711, 63.8268, 60, 50, 50, 35, 35, 34.6194, 33.5355, 31.9134, 0};
    zPoints = {0, 0, 10, 10, 10.7654, 11.4142, 11.8478, 32, 24.3463, 17.8579, 13.5224, 32, 35.8268, 39.0711, 41.2388, 62, 75, 85, 115, 115, 116.913, 118.536, 119.619, 120};
}

void GLWidget::initializeGL()
{
    glClearColor (240.0 / 255.0, 240.0 / 255.0, 240.0 / 255.0, 0.0);
    glShadeModel(GL_FLAT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glEnable(GL_LIGHTING);
    glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glEnable(GL_NORMALIZE);

    //myShader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/ground.vert");
    //myShader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/ground.frag");
    //myShader.link();
}

void GLWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    GLfloat x = GLfloat(width) / height;
    glFrustum(-x, +x, -1.0, +1.0, 4.0, 15.0);
    glMatrixMode(GL_MODELVIEW);
    glColor3f(1.0, 0.0, 0.0);
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float light0_diffuse[] = {0.4, 0.7, 0.2};
    float light0_direction[] = {0.0, 0.0, 1.0, 0.0};
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, light0_direction);

    //myShader.bind();
    draw();
    //myShader.release();
}

void GLWidget::draw()
{
    Points.clear();
    vectPoints.clear();
    vectorIndex.clear();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -5.0);
    glRotatef(rotationX, 1.0, 0.0, 0.0);
    glRotatef(rotationY, 0.0, 1.0, 0.0);
    glRotatef(rotationZ, 0.0, 0.0, 1.0);
    glScalef(scale, scale, scale);

    int steps_t = xPoints.size();
    for(int i = 0; i < steps_t; i++)
    {
        double x = xPoints[i];
        double y = 0.0;
        double z = zPoints[i];
        Points.push_back(Point(x, y, z));
    }

    double u0 = 0, un = 2 * M_PI , du = (un - u0) / steps_u;
    for(int j = 0; j < steps_u; j++)
    {
        double angle = u0 + j * du;
        rotateMatrix r('Z', angle);
        for(int i = 0; i < steps_t; ++i)
        {
            vectPoints.push_back(r * Points[i]);
        }
    }

    int ld, lu, rd, ru;
    for (int i = 0; i < steps_u; i++)
    {
        for (int j = 0; j < steps_t; j++)
        {
            ld = i * steps_t + j;
            lu = i * steps_t + (j + 1) % steps_t;
            rd = ((i + 1) % steps_u) * steps_t + j;
            ru = ((i + 1) % steps_u) * steps_t + (j + 1) % steps_t;

            vectorIndex.push_back(rd);
            vectorIndex.push_back(ld);
            vectorIndex.push_back(lu);
            vectorIndex.push_back(ru);
        }
    }

    glPointSize(1.0);
    int size1 = vectorIndex.size() / 4;
    switch (viewType)
    {
        case 0:
            glBegin(GL_QUADS);
            for (int i = 0; i < size1; ++i)
            {
                Point Point1(vectPoints[vectorIndex[4 * i    ]]);
                Point Point2(vectPoints[vectorIndex[4 * i + 1]]);
                Point Point3(vectPoints[vectorIndex[4 * i + 2]]);
                Point Point4(vectPoints[vectorIndex[4 * i + 3]]);

                glVertex3f(Point4.a, Point4.b, Point4.c);
                glVertex3f(Point3.a, Point3.b, Point3.c);
                glVertex3f(Point2.a, Point2.b, Point2.c);
                glVertex3f(Point1.a, Point1.b, Point1.c);
            }
            break;
        case 1:
            glBegin(GL_LINES);
            for (int i = 0; i < size1; ++i)
            {

                Point Point1(vectPoints[vectorIndex[4 * i    ]]);
                Point Point2(vectPoints[vectorIndex[4 * i + 1]]);
                Point Point4(vectPoints[vectorIndex[4 * i + 3]]);

                glVertex3f(Point1.a, Point1.b, Point1.c);
                glVertex3f(Point2.a, Point2.b, Point2.c);

                glVertex3f(Point1.a, Point1.b, Point1.c);
                glVertex3f(Point4.a, Point4.b, Point4.c);
            }
            break;
        case 2:
            glBegin(GL_POINTS);
            size1 = vectPoints.size();
            for (int i = 0; i < size1; ++i)
            {
                Point Point1(vectPoints[i]);
                glVertex3f(Point1.a, Point1.b, Point1.c);
            }
            break;
        default:
            break;
    }
    glEnd();
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    GLfloat dx = GLfloat(event->x() - lastPos.x()) / width();
    GLfloat dy = GLfloat(event->y() - lastPos.y()) / height();
    if (event->buttons() & Qt::LeftButton)
    {
        rotationX += 180 * dy;
        rotationY += 180 * dx;
        updateGL();
    }
    lastPos = event->pos();
}

void GLWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    int face = faceAtPosition(event->pos());
    if (face != -1) {
        QColor color = QColorDialog::getColor(faceColors[face], this);
        if (color.isValid()) {
            faceColors[face] = color;
            updateGL();
        }
    }
}

void GLWidget::wheelEvent(QWheelEvent *event)
{
    scale += sgn(event->delta()) * 0.001;
    glScalef(scale, scale, scale);
    updateGL();
}

int GLWidget::faceAtPosition(const QPoint &pos)
{
    const int MaxSize = 512;
    GLuint buffer[MaxSize];
    GLint viewport[4];

    makeCurrent();

    glGetIntegerv(GL_VIEWPORT, viewport);
    glSelectBuffer(MaxSize, buffer);
    glRenderMode(GL_SELECT);

    glInitNames();
    glPushName(0);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    GLfloat x = GLfloat(width()) / height();
    glFrustum(-x, x, -1.0, 1.0, 4.0, 15.0);
    draw();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    if (!glRenderMode(GL_RENDER))
        return -1;
    return buffer[3];
}

void GLWidget::recieveViewType(int index)
{
    viewType = index;
    updateGL();
}

void GLWidget::recievePointsData(QVector <double> X, QVector <double> Z)
{
    xPoints = X;
    zPoints = Z;
}

void GLWidget::recieveDensity(int value)
{
    steps_u = value;
    updateGL();
}

void GLWidget::changeRotationAxis(int index)
{
    QVector <double> temp = zPoints;
    zPoints = xPoints;
    xPoints = temp;
    temp.clear();
    updateGL();
}
