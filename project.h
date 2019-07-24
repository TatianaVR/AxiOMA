#ifndef PROJECT_H
#define PROJECT_H

#include <math_addition.h>
#include <qcustomplot.h>
#include <startingpoint.h>
#include <horizontalline.h>
#include <verticalline.h>
#include <line.h>
#include <circle.h>

class BlankElement;

namespace Ui {
class Project;
}

class Project : public QWidget
{
    Q_OBJECT

public:
    explicit Project(QWidget *parent = 0);
    ~Project();

     Ui::Project *ui;

private slots:

    void receiveOptions(QString, bool);
    void definitionItemIndex(QMouseEvent *);
    void showToolTip(QMouseEvent *event);
    void repaintSelectedElement(QListWidgetItem*, QListWidgetItem *);
    void on_EventList_clicked(const QModelIndex &);
    void on_VerticalLineButton_clicked();
    void on_HorizontalLineButton_clicked();
    void on_LineButton_clicked();
    void on_DirectionButton_clicked();
    void on_CurveButton_clicked();
    void on_CircleCenter_sliderMoved(int);
    void on_AddCircleCenter_clicked();
    void on_CreateButton_clicked();
    void on_EndButton_clicked();
    void on_RemoveButton_clicked();
    void on_ChangeButton_clicked();
    void on_L_argX_value_valueChanged(double);
    void on_L_argZ_value_valueChanged(double);
    void on_L_argAlpha_value_valueChanged(double);
    void on_C_argR_value_valueChanged(double);
    void on_C_argI_value_valueChanged(double);
    void on_C_argJ_value_valueChanged(double);
    void on_argR_value_valueChanged(double);
    void on_argFSX_value_valueChanged(double);
    void on_argFSZ_value_valueChanged(double);
    void on_argAlpha_value_currentIndexChanged(int);

    void on_Show3D_clicked();
    void on_ChangeAxis_clicked();

signals:
    void sendPointsData(QVector <double>, QVector <double>);

private:

    //структура данных, хранящая все необходимые параметры переходов (фасок / скруглений)
    struct Transition
    {
        double R; //радиус кругления
        double FSX, FSZ; //скосы фасок по каждой из осей
        double alpha; //угол наклона фаски (+ выше оси X, - - ниже)
        bool isDraw; //триггер отрисовки
    };

    QVector <BlankElement *> blank; //вектор элементов заготовки
    QVector <Transition> transitions; //вектор скруглений и фасок
    QVector <double> extremumValues; //минимальные и максимальные значения осей
    QVector <QPointF> centers; //возможные центры дуги, строящейся по заданным двум точкам и радиусу
    QVector <QCPCurve *> graphs;
    bool hasGap;
    int axis;

    void cleanSpinBox(); //очистка всех полей ввода
    void setTransitionEnable(bool);
    void setSelectCenterEnable(bool);
    void setAddEnable(bool);
    void setAxesRange(QList<QCPCurveData>); //автомасштабирование графика
    void redraw(int);

    void drawChamfer(int); //функция отрисовки фасок
    void drawRounding(int); //функция отрисовки скруглений

    void addToEventList(int); //добавление элемента в заготовку и отображение на виджете
    bool combineElements(int); //проверка необходимости объединенить два одинаковых последовательных элемента
    bool isContinuousLoop(); //проверка контура на замкнутость
    bool isValid(int, int); //проверка валидности элементов для создания фаски / скругления

    void logAction(QString, QColor); //вывод информации в лог
    void autoFillGap(int); //автоматическое устранение разрыва
    void saveFile(); //сохранение G-кода в файл
    bool isDifferent(); //проверка сохраняемого файла на наличие изменений
    void parseFile(QString); //парсинг открываемого файла

    void changeXZ(); //смена значений координат X и Z
    void mousePressEvent(QMouseEvent*); //обработка нажатия клавиши мыши
    virtual void keyPressEvent(QKeyEvent*); //горячие клавиши
    //QVector < QVector <double> > computeJacobianMatrix(QVector <double> currentPoint);
    //void computeIntersectionPoint(double epsilon = 10e-4); //вычисление точки пересечния (если прямая задана углом наклона)
    bool eventFilter(QObject*, QEvent*);
};

#endif // PROJECT_H
