#ifndef VIEWER_MATH_H
#define VIEWER_MATH_H

#include <QPainter>

class Point
{
public:
    double a, b, c, w;
    Point(double aa = 0, double bb = 0, double cc = 0, double ww = 1);
    ~Point(){}

    Point &operator = (const Point &rhs);
    double operator *(Point &rhs);
    Point operator ^(Point rhs);
    Point operator -(Point &rhs);
    Point operator +(Point &rhs);
    Point operator /(Point &rhs);

    double mod(Point v);
};

class Matrix
{
public:
    double mat[4][4];
    Matrix();
    Matrix operator *(const Matrix &rhm);
    Point operator *(const Point &rhp);

};

class scaleMatrix: public Matrix
{
public:
    scaleMatrix (double a, double b, double c);
};

class rotateMatrix: public Matrix
{
public:
    rotateMatrix (char axis, double a);
};

class transporateMatrix: public Matrix
{
public:
    transporateMatrix (double a, double b, double c);
};

class Triangle
{
public:
    Point a, b, c;

    Triangle(Point a = Point(), Point b = Point(), Point c = Point());
    Point n();
    void draw(QPainter *p, QBrush &);
    Point getPointA();
    Point getPointB();
    Point getPointC();
};

Triangle operator*(Matrix m, Triangle t);

#endif // VIEWER_MATH_H
