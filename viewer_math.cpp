#include <viewer_math.h>

Point::Point(double aa, double bb, double cc, double ww) {
    this->a = aa;
    this->b = bb;
    this->c = cc;
    this->w = 1;
}

Point &Point::operator = (const Point &rhc) {
    this->a = rhc.a;
    this->b = rhc.b;
    this->c = rhc.c;
    this->w = rhc.w;
    return *this;
}

double Point::operator *(Point &rhs) {

   return this->a * rhs.a + this->b * rhs.b + this->c* rhs.c;
}

Point Point::operator ^(Point rhs) {

    Point res;
    res.a = this->b * rhs.c - this->c * rhs.b;
    res.b = this->c * rhs.a - this->a * rhs.c;
    res.c = this->a * rhs.b - this->b * rhs.a;
    res.w = 0;
    return res;
}

Point Point::operator -(Point &rhs) {
     return Point(this->a - rhs.a, this->b - rhs.b, this->c - rhs.c, this->w - rhs.w);
}

Point Point::operator +(Point &rhs)
{
    Point res;
    res.a = this->a + rhs.a;
    res.b = this->b + rhs.b;
    res.c = this->c + rhs.c;
    return res;
}

Point Point::operator /(Point &rhs) {
    Point res;
    double aa = mod(rhs);
    res.a = this->a / aa;
    res.b = this->b / aa;
    res.c = this->c / aa;
    return res;
}

double Point::mod(Point v)
{
    return sqrt(fabs(v.a*v.a+v.b*v.b+v.c*v.c));
}

Matrix::Matrix()
{
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            mat[i][j] = 0;
}

Matrix Matrix::operator *(const Matrix &rhm)
{
    Matrix res;

    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            for (int k = 0; k < 4; ++k)
            {
                res.mat[i][j] += mat[i][k] * rhm.mat[k][j];
            }
        }
    }
    return res;
}

Point Matrix::operator *(const Point &rhp)
{
    double t[4] = {0, 0, 0, 0}, tp[4] = {rhp.a, rhp.b, rhp.c, rhp.w};
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            t[i] += mat[i][j] * tp[j];

    return Point (t[0], t[1], t[2], t[3]);
}

scaleMatrix::scaleMatrix (double a, double b, double c)
{
    mat[0][0] = a, mat[1][1] = b, mat[2][2] = c, mat[3][3] = 1;
}

rotateMatrix::rotateMatrix (char axis, double a)
{
    switch (axis)
    {
    case 'X':
        mat[1][1] = mat[2][2] = cos (a);
        mat[1][2] = sin (a);
        mat[2][1] = -1*sin(a);
        mat[0][0] = 1;
        break;

    case 'Y':
        mat[0][0] = mat[2][2] = cos(a);
        mat[0][2] = -1 * sin (a);
        mat[2][0] = sin(a);
        mat[1][1] = 1;
        break;

    case 'Z':
        mat[0][0] = mat[1][1] = cos(a);
        mat[0][1] = sin (a);
        mat[1][0] = -1*sin(a);
        mat[2][2] = 1;
        break;
    }
    mat[3][3] = 1;
}

transporateMatrix::transporateMatrix (double a, double b, double c)
{
    mat[0][3] = a, mat[1][3] = b, mat[2][3] = c;
    for (int i = 0; i < 4; ++i)
        mat[i][i] = 1;
}

Triangle::Triangle(Point a, Point b, Point c)
{
    this->a = a;
    this->b = b;
    this->c = c;
}

Point Triangle::n()
{
    return(this->b - this->a) ^ (this->c - this->a);
}

Triangle operator*(Matrix m, Triangle t)
{
    return Triangle(m * t.a, m * t.b, m * t.c);
}

void Triangle::draw(QPainter *p, QBrush &brush)
{
    if(n().c > 0)
    {
        QPointF Points[3] = {QPointF(this->a.a, this->a.b), QPointF(this->b.a, this->b.b), QPointF(this->c.a, this->c.b)};
        p->setPen(Qt::NoPen);
        p->setBrush(brush);
        p->drawPolygon(Points,3);
    }
}

Point Triangle::getPointA()
{
    return this->a;
}

Point Triangle::getPointB()
{
    return this->b;
}

Point Triangle::getPointC()
{
    return this->c;
}
