#include "glwidget.h"

#include <math_addition.h>

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(parent)
{
    setFormat(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer));

    rotationX = 45.0;
    rotationY = 45.0;
    rotationZ = 0.0;

    scale = 0.0215;

    xPoints = {0, 92, 92, 92, 91.8478, 91.4142, 90.7654, 89.9913, 82.3383, 75.8517, 71.5191, 70, 69.2388, 67.0711, 63.8268, 60, 50, 50, 35, 35, 34.6194, 33.5355, 31.9134, 0, 0};
    zPoints = {0, 0, 10, 10, 10.7654, 11.4142, 11.8478, 12, 13.5258, 17.864, 24.3544, 32, 35.8268, 39.0711, 41.2388, 62, 75, 85, 115, 115, 116.913, 118.536, 119.619, 120, 0};
}

void GLWidget::initializeGL()
{
    glClearColor (240.0 / 255.0, 240.0 / 255.0, 240.0 / 255.0, 0.0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);

    calculatePoints();
}

void GLWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, GLfloat(width) / height, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    myShader.removeAllShaders();
    myShader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/" + lightModelName + "/shader.vert");
    myShader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/" + lightModelName + "/shader.frag");
    myShader.link();

    myShader.bind();
    myShader.setUniformValue("eyePos", eyePosition);
    myShader.setUniformValue("lightPos", lightPosition);
    myShader.setUniformValue("diffColor", diffuseColor);
    myShader.setUniformValue("specColor", specularColor);
    for(int i = 0; i < attributeName.size(); i++)
    {
        myShader.setUniformValue(attributeName[i].toLocal8Bit().data(), (GLfloat) attributeValue[i]);
    }
    drawSurface();
    myShader.release();

    glPopMatrix();
    glFlush();
}

void GLWidget::calculatePoints()
{
    GeometricArray.clear();
    ResaultGeometric.clear();
    steps_t = xPoints.size();
    double Angle = 2.0 * M_PI / steps_u;

    for(int i = 0; i < steps_t; i++)
    {
        GeometricArray.append(QVector3D(xPoints[i], 0.0, zPoints[i]));
    }

    ResaultGeometric.resize(steps_t);
    for(int ax = 0; ax < steps_t; ax++)
    {
        ResaultGeometric[ax].resize(steps_u);
        for(int bx = 0; bx < steps_u; bx++)
        {
            double new_x, new_y;
            if (bx > 0.0)
            {
                new_x = ResaultGeometric[ax][bx - 1][0] * cos(Angle) - ResaultGeometric[ax][bx - 1][1] * sin(Angle);
                new_y = ResaultGeometric[ax][bx - 1][0] * sin(Angle) + ResaultGeometric[ax][bx - 1][1] * cos(Angle);

            }
            else // для построения первой медианы мы используем начальную кривую, описывая ее нулевым значением угла поворота
            {
                    new_x = GeometricArray[ax].x() * cos(0.0) - GeometricArray[ax].y() * sin(0.0);
                    new_y = GeometricArray[ax].y() * sin(0.0) + GeometricArray[ax].y() * cos(0.0);
            }

            ResaultGeometric[ax][bx].setX(new_x);
            ResaultGeometric[ax][bx].setY(new_y);
            ResaultGeometric[ax][bx].setZ(GeometricArray[ax].z());
        }
    }
}

void GLWidget::drawSurface()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -7.0);
    glRotatef(rotationX, 1.0, 0.0, 0.0);
    glRotatef(rotationY, 0.0, 1.0, 0.0);
    glRotatef(rotationZ, 0.0, 0.0, 1.0);
    glScalef(scale, scale, scale);

    glPointSize(1.0);

    switch (viewType)
    {
        case 0:
            glBegin(GL_QUADS);
            for ( int ax = 0; ax < steps_t - 1; ax++)
            {
                for ( int bx = 0; bx < steps_u; bx++)
                { // вспомогательные переменные для более наглядного использования кода при расчете нормалей
                    double x1 = 0., x2 = 0., x3 = 0., x4 = 0., y1 = 0., y2 = 0., y3 = 0., y4 = 0., z1 = 0., z2 = 0., z3 = 0., z4 = 0.; // первая вершина
                    x1 = ResaultGeometric[ax][bx][0];
                    y1 = ResaultGeometric[ax][bx][1];
                    z1 = ResaultGeometric[ax][bx][2];
                    if (ax + 1 < steps_t)
                    {
                        x2 = ResaultGeometric[ax + 1][bx][0];
                        y2 = ResaultGeometric[ax + 1][bx][1];
                        z2 = ResaultGeometric[ax + 1][bx][2];

                        if (bx + 1 < steps_u)
                        { // берем следующую точку последовательности и следующий медиан
                            x3 = ResaultGeometric[ax + 1][bx + 1][0];
                            y3 = ResaultGeometric[ax + 1][bx + 1][1];
                            z3 = ResaultGeometric[ax + 1][bx + 1][2]; // точка, соответствующая по номеру, только на соседнем медиане

                            x4 = ResaultGeometric[ax][bx + 1][0];
                            y4 = ResaultGeometric[ax][bx + 1][1];
                            z4 = ResaultGeometric[ax][bx + 1][2];
                        }
                        else
                        { // если это последний медиан, то в качестве след. мы берем начальный (замыкаем геометрию фигуры)
                            x3 = ResaultGeometric[ax + 1][0][0];
                            y3 = ResaultGeometric[ax + 1][0][1];
                            z3 = ResaultGeometric[ax + 1][0][2];

                            x4 = ResaultGeometric[ax][0][0];
                            y4 = ResaultGeometric[ax][0][1];
                            z4 = ResaultGeometric[ax][0][2];
                        }
                    }
                    else // данный элемент ax последний, следовательно, мы будем использовать начальный (нулевой) вместо данного ax
                    { // следующей точкой будет нулевая ax
                        x2 = ResaultGeometric[0][bx][0];
                        y2 = ResaultGeometric[0][bx][1];
                        z2 = ResaultGeometric[0][bx][2];

                        if (bx + 1 < steps_u)
                        {
                            x3 = ResaultGeometric[0][bx + 1][0];
                            y3 = ResaultGeometric[0][bx + 1][1];
                            z3 = ResaultGeometric[0][bx + 1][2];

                            x4 = ResaultGeometric[ax][bx + 1][0];
                            y4 = ResaultGeometric[ax][bx + 1][1];
                            z4 = ResaultGeometric[ax][bx + 1][2];
                        }
                        else
                        {
                            x3 = ResaultGeometric[0][0][0];
                            y3 = ResaultGeometric[0][0][1];
                            z3 = ResaultGeometric[0][0][2];

                            x4 = ResaultGeometric[ax][0][0];
                            y4 = ResaultGeometric[ax][0][1];
                            z4 = ResaultGeometric[ax][0][2];
                        }
                    } // переменные для расчета нормали

                    double n1 = 0, n2 = 0, n3 = 0; // нормаль будем рассчитывать как векторное произведение граней полигона
                     // для нулевого элемента нормаль мы будем считать немного по-другому.
                                 // на самом деле разница в расчете нормали актуальна только для первого и последнего полигона на медиане
                    if (ax == 0)
                    {
                        n1 = (y2 - y1) * (z3 - z1) - (y3 - y1) * (z2 - z1);
                        n2 = (z2 - z1) * (x3 - x1) - (z3 - z1) * (x2 - x1);
                        n3 = (x2 - x1) * (y3 - y1) - (x3 - x1) * (y2 - y1);
                    }
                    else // для остальных - 1,3,4
                    {
                        n1 = (y4 - y3) * (z1 - z3) - (y1 - y3) * (z4 - z3);
                        n2 = (z4 - z3) * (x1 - x3) - (z1 - z3) * (x4 - x3);
                        n3 = (x4 - x3) * (y1 - y3) - (x1 - x3) * (y4 - y3);
                     }

                     glNormal3d(-n1, -n2, -n3); // передаем 4 вершины для отрисовки полигона Gl.
                     glVertex3d(x4, y4, z4);
                     glVertex3d(x3, y3, z3);
                     glVertex3d(x2, y2, z2);
                     glVertex3d(x1, y1, z1);
                }
           }
           break;
        case 1:
            glBegin(GL_LINE_STRIP);
            for( int ax = 0; ax < steps_t - 1; ax++)
            {
                for (int bx = 0; bx < steps_u; bx++)
                {
                    glVertex3d(ResaultGeometric[ax][bx][0], ResaultGeometric[ax][bx][1], ResaultGeometric[ax][bx][2]);
                    glVertex3d(ResaultGeometric[ax + 1][bx][0], ResaultGeometric[ax + 1][bx][1], ResaultGeometric[ax + 1][bx][2]);
                    if (bx + 1 < steps_u)
                    {
                        glVertex3d(ResaultGeometric[ax + 1][bx + 1][0], ResaultGeometric[ax + 1][bx + 1][1], ResaultGeometric[ax + 1][bx + 1][2]);
                    }
                    else
                    {
                        glVertex3d(ResaultGeometric[ax + 1][0][0], ResaultGeometric[ax + 1][0][1], ResaultGeometric[ax + 1][0][2]);
                    }
                }
            }
            break;
        case 2:
            glBegin(GL_POINTS);
            for( int ax = 0; ax < steps_t; ax++)
            {
                for ( int bx = 0; bx < steps_u; bx++)
                {
                    glVertex3d(ResaultGeometric[ax][bx][0], ResaultGeometric[ax][bx][1], ResaultGeometric[ax][bx][2]);
                }
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

void GLWidget::wheelEvent(QWheelEvent *event)
{
    scale += sgn(event->delta()) * scale / 5;
    glScalef(scale, scale, scale);
    updateGL();
}

void GLWidget::receiveViewType(int index)
{
    viewType = index;
    updateGL();
}

void GLWidget::receivePointsData(QVector <double> X, QVector <double> Z)
{
    xPoints = X;
    zPoints = Z;
    calculatePoints();
    updateGL();
}

void GLWidget::receiveDensity(int value)
{
    steps_u = value;
    calculatePoints();
    updateGL();
}

void GLWidget::changeRotationAxis()
{
    QVector <double> temp = zPoints;
    zPoints = xPoints;
    xPoints = temp;
    temp.clear();
    calculatePoints();
    updateGL();
}

void GLWidget::receiveModelLight(const QString str)
{
    lightModelName = str;
    updateGL();
}

void GLWidget::receiveDiffuseColor(QColor color)
{
    diffuseColor = color;
    updateGL();
}

void GLWidget::receiveSpecularColor(QColor color)
{
    specularColor = color;
    updateGL();
}

void GLWidget::receiveLightModelAttributes(QVector <QString> names, QVector <double> values)
{
   attributeName = names;
   attributeValue = values;
   updateGL();
}

void GLWidget::receiveLightSettings(QVector3D light, QVector3D eye)
{
    lightPosition = light;
    eyePosition = eye;
    updateGL();
}
