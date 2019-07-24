#ifndef MATH_ADDITION_H
#define MATH_ADDITION_H

#define EPSILON 1e-4

void LU(QVector <QVector <double> > &L, QVector <QVector <double> > &U);
QVector <double> LU_z(QVector <QVector <double> > L, QVector <double> b);
QVector <double> LU_x(QVector <QVector <double> > U, QVector <double> z);
QVector < QVector <double> > inverse_matrix(QVector < QVector <double> > L, QVector < QVector <double> > U);
bool isOrthogonal(QPointF A, QPointF B, QPointF C, QPointF D); //проверка ортогональности двух прямых, каждая из которых задана начальной и конечной точками
bool isEqual(double a, double b);
QPointF SLAE(QVector < QVector <double> >, QVector <double>);

template <typename T> int sgn(T val)
{
    return (T(0) < val) - (val < T(0));
}
#endif // MATH_ADDITION_H
