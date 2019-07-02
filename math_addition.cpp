#include <math_addition.h>

void LU(QVector <QVector <double> > &L, QVector <QVector <double> > &U)
{
    int N = L.size();

    for (int i = 0; i < N; i++)
    {
        for (int j = i; j < N; j++)
        {
            L[j][i] = U[j][i] / U[i][i];
        }
    }

    for (int k = 1; k < N; k++)
    {
        for (int i = k - 1; i < N; i++)
        {
            for (int j = i; j < N; j++)
            {
                L[j][i] = U[j][i] / U[i][i];
            }
        }

        for (int i = k; i < N; i++)
        {
            for (int j = k - 1; j < N; j++)
            {
                U[i][j] = U[i][j] - L[i][k - 1] * U[k - 1][j];
            }
        }
    }
}

QVector <double> LU_z(QVector <QVector <double> > L, QVector <double> b)
{
    for (int i = 0; i < b.size(); i++)
    {
        double sum = 0.0;

        for (int j = 0; j < i; j++)
        {
            sum += L[i][j] * b[j];
        }

        b[i] -= sum;
    }

    return b;
}

QVector <double> LU_x(QVector <QVector <double> > U, QVector <double> z)
{
    QVector <double> x(z.size());
    for (int i = x.size() - 1; i >= 0; i--)
    {
        double sum = 0.0;

        for (int j = i; j < x.size(); j++)
        {
            sum += U[i][j] * x[j];
        }

        x[i] = (z[i] - sum) / (U[i][i]);
    }

    return x;
}

QVector < QVector <double> > inverse_matrix(QVector < QVector <double> > L, QVector < QVector <double> > U)
{
    int N = L.size();
    QVector < QVector <double> > IM(N);

    QVector <double> vec_e;

    for (int i = 0; i < N; i++)
    {
        vec_e.fill(0.0, N);
        vec_e[i] = 1.0;
        IM[i] = LU_x(U, LU_z(L, vec_e));
    }

    for(int i = 0; i < N; i++)
    {
        for(int j = i + 1; j < N; j++)
        {
          double s = IM[i][j];
          IM[i][j] = IM[j][i];
          IM[j][i] = s;
        }
    }

    vec_e.clear();

    return IM;
}

QPointF SLAE(QVector < QVector <double> > A, QVector <double> b)
{
    int size = b.size();
    QVector <double> x(size);

    double max;
    int k, index;
    const double eps = 0.00001;  // точность
    k = 0;
    while (k < size)
    {
      // Поиск строки с максимальным a[i][k]
      max = qAbs(A[k][k]);
      index = k;
      for (int i = k + 1; i < size; i++)
      {
        if (qAbs(A[i][k]) > max)
        {
          max = qAbs(A[i][k]);
          index = i;
        }
      }

      // Перестановка строк
      if (max < eps)
      {
        // нет ненулевых диагональных элементов
        return QPointF(0.0, 0.0);
      }

      for (int j = 0; j < size; j++)
      {
        double temp = A[k][j];
        A[k][j] = A[index][j];
        A[index][j] = temp;
      }

      double temp = b[k];
      b[k] = b[index];
      b[index] = temp;

      // Нормализация уравнений
      for (int i = k; i < size; i++)
      {
        double temp = A[i][k];
        if (qAbs(temp) < eps)
        {
            continue; // для нулевого коэффициента пропустить
        }
        for (int j = 0; j < size; j++)
        {
          A[i][j] = A[i][j] / temp;
        }
        b[i] = b[i] / temp;
        if (i == k)
        {
            continue; // уравнение не вычитать само из себя
        }

        for (int j = 0; j < size; j++)
        {
          A[i][j] = A[i][j] - A[k][j];
        }
        b[i] = b[i] - b[k];
      }
      k++;
    }

    // обратная подстановка
    for (k = size - 1; k >= 0; k--)
    {
      x[k] = b[k];
      for (int i = 0; i < k; i++)
      {
        b[i] = b[i] - A[i][k] * x[k];
      }
    }
    return QPointF(x[0], x[1]);
}

//проверка ортогональности двух прямых, каждая из которых задана начальной и конечной точками
bool isOrthogonal(QPointF A, QPointF B, QPointF C, QPointF D)
{
    return (B.x() - A.x()) * (D.x() - C.x()) + (B.y() - A.y()) * (D.y() - C.y()) == 0.0 ? true : false;
}

bool isEqual(double a, double b)
{
    return qAbs(a - b) < EPSILON;
}
