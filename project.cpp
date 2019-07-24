#include "project.h"
#include "ui_project.h"

#include "viewer3d.h"
#include "widget.h"

using namespace std;

#define DELTA 5.0

Project::Project(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Project)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::FramelessWindowHint); //нет рамки

    //Задание вектора, хранящего минимум и максимум осей коориднат
    extremumValues.fill(0.0, 2); // extremumValuesх[0] <- min,  extremumValues[1] <- max

    //Подписываем оси Ox и Oz
    ui->widget->xAxis->setLabel("Z");
    ui->widget->yAxis->setLabel("X");

    setSelectCenterEnable(false);
    setTransitionEnable(false);

    //при выборе в соответствующем окне элемента вызывается функция выделения его на графике
    connect(ui->EventList, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)), this, SLOT(repaintSelectedElement(QListWidgetItem*, QListWidgetItem*)));
    connect(ui->widget, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(definitionItemIndex(QMouseEvent*)));
    connect(ui->widget, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(showToolTip(QMouseEvent*)));

    //очистка полей ввода при наведении курсора
    foreach (QDoubleSpinBox * spinBox, this->findChildren<QDoubleSpinBox *>()) //поиск всех элементов, предназначенных для ввода данных
    {
        foreach (QObject *editLine, spinBox->children()) //вычленяем из элементов поле ввода
        {
            editLine->installEventFilter(this); //переопределяем событие нажатия мышки по полю ввода (см. bool Project::eventFilter(QObject *obj, QEvent *event))
        }
    }

    hasGap = false;
    axis = 0;
}

//Удаление формы
Project::~Project()
{
    delete ui;
}

//обработчик событий для очистки полей ввода при наведении курсора
bool Project::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type() == QEvent::MouseButtonPress) //проверка события нажатия кнопки мыши
    {
        qobject_cast<QLineEdit*>(obj)->clear(); //если объект - поле ввода, то очищаем его
    }
    return false;
}

//Снятие выделения с выбранного элемента
void Project::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    if (!ui->EventList->selectedItems().isEmpty())
    {
        cleanSpinBox();
        if(ui->EventList->currentRow() > 0)
        {
            blank[ui->EventList->currentRow()]->getGraph()->setSelected(false);
            ui->widget->replot();
        }
        if(hasGap)
        {
            ui->EventList->setCurrentRow(blank.size() - 1);
        }
        ui->EventList->clearSelection();
    }
}

//Hot Keys
void Project::keyPressEvent(QKeyEvent *event)
{
    switch (event->nativeVirtualKey())
    {
        case Qt::Key_Z:
            on_HorizontalLineButton_clicked();
            break;
        case Qt::Key_X:
            on_VerticalLineButton_clicked();
            break;
        case Qt::Key_C:
            on_CurveButton_clicked();
            break;
        case Qt::Key_V:
            on_LineButton_clicked();
            break;
        case Qt::Key_R:
            changeXZ();
            break;
        default:
            break;
    }
}

//Функция, меняющая местами значения координат X и Z
void Project::changeXZ()
{
    ui->EventList->clear();

    for(int i = 0; i < blank.size(); i++)
    {
        blank[i]->swapXZ();
        if(dynamic_cast<HorizontalLine*>(blank[i]))
        {
            QPointF p1 = blank[i]->getStartPoint(), p2 = blank[i]->getEndPoint();
            delete blank[i]->getGraph();
            blank.remove(i);
            blank.insert(i, new VerticalLine(p1, p2));
        }
        else if(dynamic_cast<VerticalLine*>(blank[i]))
        {
            QPointF p1 = blank[i]->getStartPoint(), p2 = blank[i]->getEndPoint();
            delete blank[i]->getGraph();
            blank.remove(i);
            blank.insert(i, new HorizontalLine(p1, p2));
        }
        ui->EventList->addItem(blank[i]->createItem());
        redraw(i);
    }
}

//Функция, обрабатывающая входящие параметры при открытии текущего окна
//project_name - имя проекта / isExist - триггер, определяющий открываем новый файл или уже существующий
void Project::receiveOptions(QString project_name, bool isExist)
{
    ui->string_name->setText(project_name); //вывод имени заготовки
    ui->ElementParametrs->setCurrentIndex(0);
    if (isExist) //если открываем существующий файл
    {
        ui->CreateButton->setEnabled(false);
        logAction("Открыт файл " + project_name + ".nc", Qt::black);
        parseFile(project_name); //производим парсинг этого файла
        ui->EventList->item(0)->setSelected(true);
        on_EventList_clicked(ui->EventList->model()->index(0,0));
    }
     //даем пользователю возможность производить с заготовкой дальнейшие манипуляции : добавление подсказки в Лог
    blank.size() != 0 ? setAddEnable(true) : logAction("Введите начальную точку", Qt::black);
}

//Функция очистки старых значений в полях (значение по умолчанию в 0)
void Project::cleanSpinBox()
{
    QList<QDoubleSpinBox *> elements = this->findChildren<QDoubleSpinBox *>();
    for(int i = 0; i < elements.size(); i++)
    {
        elements[i]->setValue(0.0);
    }

    ui->argAlpha_value->setCurrentIndex(0);

    if(axis == 0)
    {
        ui->CreateButton->setEnabled(true);
    }
    ui->RemoveButton->setEnabled(false);
    ui->ChangeButton->setEnabled(false);

}

//Скрыть / раскрыть окно параметров для ввода параметров фасок и скруглений (при вводе начальной точки и дуги данный функционал не доступен)
void Project::setTransitionEnable(bool isEnable)
{
    for(int i = 0; i < ui->TransitionItem->count(); i++)
    {
        ui->TransitionItem->itemAt(i)->widget()->setVisible(isEnable);
    }
}

//Скрыть / раскрыть окно параметров для выбора центра окружности
void Project::setSelectCenterEnable(bool isEnable)
{
    for(int i = 0; i < ui->SelectCenter->count(); i++)
    {
        ui->SelectCenter->itemAt(i)->widget()->setVisible(isEnable);
    }
}

//Доступны / недоступны кнопки элементов
void Project::setAddEnable(bool isEnabled)
{
    ui->VerticalLineButton->setEnabled(isEnabled);
    ui->HorizontalLineButton->setEnabled(isEnabled);
    ui->LineButton->setEnabled(isEnabled);
    ui->CurveButton->setEnabled(isEnabled);
}

//======================================
//==== структура = тела = функции ======
// очистка всех полей ввода параметров
// задание ID добавляемого элемента
// вывод соответствующего окна параметров
//======================================
//Нажатие кнопки для добавления прямой Z
void Project::on_HorizontalLineButton_clicked()
{
    cleanSpinBox();
    ui->ElementParametrs->setCurrentIndex(1);
    setTransitionEnable(true);
}

//Нажатие кнопки для добавления прямой X
void Project::on_VerticalLineButton_clicked()
{
    cleanSpinBox();
    ui->ElementParametrs->setCurrentIndex(2);
    setTransitionEnable(true);
}

//Нажатие кнопки для добавления прямой
void Project::on_LineButton_clicked()
{
    cleanSpinBox();
    ui->ElementParametrs->setCurrentIndex(3);
    setTransitionEnable(true);
}

//Нажатие кнопки для добавления дуги
void Project::on_CurveButton_clicked()
{
    cleanSpinBox();
    ui->DirectionButton->setStyleSheet("background: url(:/data/direction/left.png)");
    ui->ElementParametrs->setCurrentIndex(4);
    setTransitionEnable(false);
}
//======================================

//Функция изменения направления по или против часовой стрелки (смена иконки для соответствующей кнопки)
void Project::on_DirectionButton_clicked()
{
    if(ui->DirectionButton->styleSheet().contains("left"))
    {
        ui->DirectionButton->setStyleSheet("background: url(:/data/direction/right.png)");
    }
    else
    {
        ui->DirectionButton->setStyleSheet("background: url(:/data/direction/left.png)");
    }
    if(ui->label_3->isVisible())
    {
        blank.last()->getCodeElement() == "G02" ?  blank.last()->setCodeElement("G03") : blank.last()->setCodeElement("G02");
        redraw(blank.size() - 1);
    }
}

//Автоматическое вычисление масштаба осей
//xValues - все значения X добавляемого элемента / zValues - все значения Z
void Project::setAxesRange(QList<QCPCurveData> data)
{
    QVector <double> xValues, zValues;
    foreach (QCPCurveData item, data)
    {
        xValues.append(item.key);
        zValues.append(item.value);
    }
    //Вычисляем минимальные и максимальные значения для каждой из векторов xValues, zValues
    double minX, maxX, minZ, maxZ;
    minX = *min_element(xValues.constBegin(), xValues.constEnd());
    maxX = *max_element(xValues.constBegin(), xValues.constEnd());
    minZ = *min_element(zValues.constBegin(), zValues.constEnd());
    maxZ = *max_element(zValues.constBegin(), zValues.constEnd());
    //Выбор минимума из минимумов и максимума из максимумов каждого из векторов
    double max, min;
    minX < minZ ? min = minX : min = minZ;
    maxX < maxZ ? max = maxZ : max = maxX;
    //Находим минимум и максимум среди текущих и вычисленных для векторов xValues, zValues значений
    if(min < extremumValues[0])
    {
        extremumValues[0] = min;
    }
    if(max > extremumValues[1])
    {
        extremumValues[1] = max;
    }
    //Задаем найденные максимум и минимум в качестве диапазона каждой из осей с отступом DELTA по всем направлениям
    ui->widget->xAxis->setRange(extremumValues[0] - DELTA, extremumValues[1] + DELTA);
    ui->widget->yAxis->setRange(extremumValues[0] - DELTA, extremumValues[1] + DELTA);
    ui->widget->replot();
}

//Функция перересовки элемента
//index - номер элемента
void Project::redraw(int index)
{
    blank[index]->draw(ui);
    setAxesRange(blank[index]->getGraph()->data()->values());
    ui->widget->replot();
}

//Функция, реализующая смену центра окружности при её добавлении
void Project::on_CircleCenter_sliderMoved(int position)
{
    Q_UNUSED(position);
    redraw(blank.size() - 1);
}

//Подтверждение выбранной окружности, её дальнейшее добавление в список всех элементов
void Project::on_AddCircleCenter_clicked()
{
    //Скрываем окно параметров для выбора центра окружности
    ui->CircleCenter->setValue(0);
    setSelectCenterEnable(false);

    //разблокируем кнопки для добавления других элементов
    setAddEnable(true);

    centers.clear();

    addToEventList(blank.size() - 1);
}

//Функция отрисовки фасок с пересчетом элементов
//index - индекс заданного элемента
void Project::drawChamfer(int index)
{
    if(transitions[index].FSX != 0.0)
    {
        transitions[index].FSZ = transitions[index].FSX / qTan(transitions[index].alpha);
    }
    else
    {
        transitions[index].FSX = transitions[index].FSZ / qTan(transitions[index].alpha);
    }

    QPointF point1, point2;

    if((isOrthogonal(blank[blank.size() - 2]->getStartPoint(), blank[blank.size() - 2]->getEndPoint(), blank[blank.size() - 1]->getStartPoint(), blank[blank.size() - 1]->getEndPoint())))
    {
        int id = -1;
        double val_y = 0.0, val_x = 0.0;
        if(dynamic_cast<HorizontalLine*>(blank[blank.size() - 2]))
        {
            id = blank.size() - 2;
            val_y = blank[id]->getEndPoint().y();
            val_x = blank[id + 1]->getStartPoint().x() > blank[id + 1]->getEndPoint().x() ? val_y + transitions[index].FSX : val_y - transitions[index].FSX;
        }
        else if(dynamic_cast<VerticalLine*>(blank[blank.size() - 2]))
        {
            id = blank.size() - 1;
            val_y = blank[id]->getStartPoint().y();
            val_x = blank[id - 1]->getStartPoint().x() > blank[id - 1]->getEndPoint().x() ? val_y + transitions[index].FSX : val_y - transitions[index].FSX;
        }
        else
        {
            logAction("Ошибка создания фаски : элементы " + QString::number(blank.size() - 1) + " и " + QString::number(blank.size()) + " не ортогональны и не сонаправлены", Qt::red);
            return;
        }

        double angel = transitions[index].alpha * sgn((blank[blank.size() - 1]->getEndPoint().y() - blank[blank.size() - 2]->getStartPoint().y()) / (blank[blank.size() - 1]->getEndPoint().x() - blank[blank.size() - 2]->getStartPoint().x()));

        QVector <double> b(2);
        b[0] = blank[blank.size() - 2]->b1();
        b[1] = blank[id]->getStartPoint().x() - qTan(angel) * val_x;

        point1 = SLAE(blank[blank.size() - 2]->getMatrixA(angel), b);
        blank[blank.size() - 2]->setEndPoint(point1);

        b[0] = blank[blank.size() - 1]->b1();
        point2 = SLAE(blank[blank.size() - 1]->getMatrixA(angel), b);
        blank[blank.size() - 1]->setStartPoint(point2);
    }
    else if(typeid(*blank[blank.size() - 1]) == typeid(*blank[blank.size() - 2]))
    {
        if(dynamic_cast<HorizontalLine*>(blank[blank.size() - 2]))
        {
            blank[blank.size() - 2]->setEndPointZ(blank[blank.size() - 2]->getEndPoint().y() -
                    sgn(blank[blank.size() - 2]->getEndPoint().y() - blank[blank.size() - 2]->getStartPoint().y()) * transitions[index].FSZ);
            blank[blank.size() - 1]->setEndPointX(blank[blank.size() - 1]->getEndPoint().x() + transitions[index].FSX);
            blank[blank.size() - 1]->setStartPointX(blank[blank.size() - 1]->getEndPoint().x());
        }
        else if(dynamic_cast<VerticalLine*>(blank[blank.size() - 2]))
        {
            blank[blank.size() - 2]->setEndPointX(blank[blank.size() - 2]->getEndPoint().x() -
                    sgn(blank[blank.size() - 2]->getEndPoint().x() - blank[blank.size() - 2]->getStartPoint().x()) * transitions[index].FSX);
            blank[blank.size() - 1]->setEndPointZ(blank[blank.size() - 1]->getEndPoint().y() + transitions[index].FSZ);
            blank[blank.size() - 1]->setStartPointZ(blank[blank.size() - 1]->getEndPoint().y());
        }
        else
        {
            logAction("Ошибка создания фаски : элементы " + QString::number(blank.size() - 1) + " и " + QString::number(blank.size()) + " не ортогональны и не сонаправлены", Qt::red);
            return;
        }
        point1 = blank[blank.size() - 2]->getEndPoint();
        point2 = blank[blank.size() - 1]->getStartPoint();
    }
    else
    {
        logAction("Ошибка создания фаски : элементы " + QString::number(blank.size() - 1) + " и " + QString::number(blank.size()) + " не ортогональны и не сонаправлены", Qt::red);
        return;
    }

    blank.append(new Line(point1, point2));
    addToEventList(blank.size() - 1);

    BlankElement *swapItem = blank[blank.size() - 2];
    blank[blank.size() - 2] = blank[blank.size() - 1];
    blank[blank.size() - 1] = swapItem;

    for(int iGraph = blank.size() - 3; iGraph < blank.size(); iGraph++)
    {
        redraw(iGraph);
    }
}

//Функция отрисовки скруглений с пересчетом элементов
//index - индекс заданного элемента
void Project::drawRounding(int index)
{
    if(isOrthogonal(blank[blank.size() - 2]->getStartPoint(), blank[blank.size() - 2]->getEndPoint(), blank[blank.size() - 1]->getStartPoint(), blank[blank.size() - 1]->getEndPoint()))
    {
        QLineF line1(blank[blank.size() - 2]->getEndPoint(), blank[blank.size() - 2]->getStartPoint());
        line1.setLength(transitions[index].R);

        QLineF line2 = QLineF (blank[blank.size() - 1]->getStartPoint(), blank[blank.size() - 1]->getEndPoint());
        line2.setLength(transitions[index].R);

        QLineF hypotenuse(blank[blank.size() - 2]->getEndPoint(),  QLineF(line1.p2(), line2.p2()).center());
        hypotenuse.setLength(transitions[index].R * qSqrt(2.0));

        blank[blank.size() - 2]->setEndPoint(line1.p2());
        blank[blank.size() - 1]->setStartPoint(line2.p2());

        double direction = (blank[blank.size() - 2]->getEndPoint().x() - hypotenuse.p2().x())*(blank[blank.size() - 1]->getStartPoint().y() - hypotenuse.p2().y())
                - (blank[blank.size() - 2]->getEndPoint().y() - hypotenuse.p2().y())*(blank[blank.size() - 1]->getStartPoint().x() - hypotenuse.p2().x());
        QString code = direction > 0 ? "G02" : "G03";

        blank.append(new Circle(code, hypotenuse.p2(), blank[blank.size() - 2]->getEndPoint(),  blank[blank.size() - 1]->getStartPoint()));
        addToEventList(blank.size() - 1);

        BlankElement *swapItem = blank[blank.size() - 2];
        blank[blank.size() - 2] = blank[blank.size() - 1];
        blank[blank.size() - 1] = swapItem;

        for(int iGraph = blank.size() - 3; iGraph < blank.size(); iGraph++)
        {
            redraw(iGraph);
        }
    }
    else
    {
        logAction("Ошибка создания скругления : элементы " + QString::number(blank.size() - 2) + " и " + QString::number(blank.size() - 1) + " не ортогональны", Qt::red);
        return;
    }
}

//Функция, позволяющая мышкой выбирать элемент графика
void Project::definitionItemIndex(QMouseEvent *event)
{
     //считываем координаты точки на графике соответствующие месту клика мышкой
     double x = ui->widget->yAxis->pixelToCoord(event->pos().y());
     double y = ui->widget->xAxis->pixelToCoord(event->pos().x());
     double currentValue = 0.0; //вспомогательная переменная, хранящая текущее отклонение
     for(int i = 1; i < blank.size(); i++)
     {
         //условие проверки, позволяющее избежать ложных срабатываний
         if((blank[i]->getStartPoint().y() <= y && y <= blank[i]->getEndPoint().y()) || (blank[i]->getEndPoint().y() <= y && y <= blank[i]->getStartPoint().y()))
         {
             if(blank[i]->getCodeElement() == "G01") //проверка на принадлежность точки прямой
             {
                // если прямая задана точками (x1,y1) и (x2,y2), то уравнение прямой будет иметь вид:
                // (y1 - y2) * x + (x2 - x1) * y + (y2 * x1 - y1 * x2) = 0
                currentValue = (blank[i]->getStartPoint().y() - blank[i]->getEndPoint().y()) * x +
                     (blank[i]->getEndPoint().x() - blank[i]->getStartPoint().x()) * y +
                     (blank[i]->getStartPoint().x() * blank[i]->getEndPoint().y() - blank[i]->getStartPoint().y() * blank[i]->getEndPoint().x());
             }
             else //проверка на принадлежность точки дуге
             {
                //зная центр окружности (x1,y1) и радиус R, то уравнение окружности имеет вид:
                // (x - x1)^2 + (y - y1)^2 - R^2 = 0
                 if(dynamic_cast<Circle*>(blank[i]))
                 {
                     currentValue = pow(x - static_cast<Circle*>(blank[i])->getArcCenter().x(), 2) + pow(y - static_cast<Circle*>(blank[i])->getArcCenter().y(), 2) - pow(static_cast<Circle*>(blank[i])->getR(), 2);
                 }
             }
             if (abs(currentValue) < 50.0) //если найденное абсолютное отклонение меньше указанного значения
             {
                 ui->EventList->setCurrentRow(i);
                 on_EventList_clicked(ui->EventList->model()->index(i,0));
                 break;
             }
         }
     }
}

//Функция вывода координат точки, в которой находится мышка
void Project::showToolTip(QMouseEvent *event)
{
    for(QCPLayoutElement *element : ui->widget->plotLayout()->elements(true))
    {
        QCPAxisRect *axisRect = static_cast<QCPAxisRect *>(element);
        if(axisRect)
        {
            if(axisRect->rect().contains(event->pos()))
            {
                double x = ui->widget->yAxis->pixelToCoord(event->pos().y());
                double z = ui->widget->xAxis->pixelToCoord(event->pos().x());
                ui->widget->setToolTip(tr("%1,%2").arg(z).arg(x));
                break;
            }
        }
    }
}

//Функция, позволяющая вернуть элемент контура в исходное состояние
//next - текущий выбранный элемент, previous - элемент, который был выбран до текущего
void Project::repaintSelectedElement(QListWidgetItem* next, QListWidgetItem* previous)
{
    Q_UNUSED(next);
    //возвращаем предыдущий выбранный элемент в исходное состояние
    int currentIndex = ui->EventList->row(previous); //получаем индекс
    if(currentIndex > 0)
    {
        blank[currentIndex]->getGraph()->setSelected(false);
    }
}

void Project::on_EventList_clicked(const QModelIndex &index)
{
    setTransitionEnable(false);
    int currentIndex = index.row(); //получаем индекс
    ui->RemoveButton->setEnabled(true);
    ui->ChangeButton->setEnabled(true);
    if(currentIndex != 0) //возвращаем предыдущий выбранный элемент в исходное состояние
    {
        blank[currentIndex]->getGraph()->setSelected(true);
        ui->widget->replot();
    }
    else
    {
        ui->RemoveButton->setEnabled(false);
        ui->ChangeButton->setEnabled(false);
    }

    if(hasGap)
    {
        ui->EventList->setCurrentRow(currentIndex);
    }

    blank[currentIndex]->setParameters(ui);
}

//Функция, объединящая два последовательных "одинаковых" элемента
bool Project::combineElements(int index)
{
    bool toUnion = false;
    if(dynamic_cast<HorizontalLine*>(blank[index]) || dynamic_cast<VerticalLine*>(blank[index]))
    {
        if(transitions.isEmpty())
        {
            toUnion = true;
        }
        else if(transitions[0].FSX == 0.0 && transitions[0].FSZ == 0.0)
        {
            toUnion = true;
        }
    }
    else if(dynamic_cast<Line*>(blank[index]))
    {
        double k1 = (blank[index - 1]->getEndPoint().y() - blank[index - 1]->getStartPoint().y()) / (blank[index - 1]->getEndPoint().x() - blank[index - 1]->getStartPoint().x());
        double k2 = (blank[index]->getEndPoint().y() - blank[index]->getStartPoint().y()) / (blank[index]->getEndPoint().x() - blank[index]->getStartPoint().x());
        if(isEqual(k1, k2))
        {
            toUnion = true;
        }
    }
    else if(dynamic_cast<Circle*>(blank[index]))
    {
        if((static_cast<Circle*>(blank[index])->getArcCenter() == static_cast<Circle*>(blank[index - 1])->getArcCenter()) && (blank[index]->getCodeElement() == blank[index - 1]->getCodeElement()))
        {
            static_cast<Circle*>(blank[index - 1])->setR(0.0);
            toUnion  = true;
        }
    }

    if(toUnion)
    {
        logAction("Объединены элементы " + QString::number(index) + " и " + QString::number(index + 1), Qt::black);
        blank[index - 1]->setEndPoint(blank[index]->getEndPoint());
        delete blank[index]->getGraph();
        blank.remove(index);
        logAction("Элемент " + QString::number(index) + " изменен на " + blank[index - 1]->GCode(), Qt::black);
    }

    return toUnion;
}

bool Project::isContinuousLoop()
{
    bool hasNoGap = true;
    for(int i = 0; i < blank.size() - 1; i++)
    {
        if(blank[i]->getEndPoint() != blank[i + 1]->getStartPoint())
        {
            hasNoGap = false;
            break;
        }
    }
    return hasNoGap;
}

bool Project::isValid(int type, int previous)
{
    QColor color = Qt::red;
    bool validData = true;
    switch (type)
    {
        case 0:
            break;
        case 1:
            if(isEqual(blank[previous]->getEndPoint().y(), ui->LZ_argZ_value->value()))
            {
                logAction("Ошибка ввода данных : начальная и конечная точки не могут совпадать", color);
                validData = false;
            }
            break;
        case 2:
            if(isEqual(blank[previous]->getEndPoint().x(), ui->LX_argX_value->value()))
            {
                logAction("Ошибка ввода данных : начальная и конечная точки не могут совпадать", color);
                validData = false;
            }
            break;
        case 3:
        {
            if(isEqual(ui->L_argX_value->value(), blank[previous]->getEndPoint().x()) || isEqual(ui->L_argZ_value->value(), blank[previous]->getEndPoint().y()))
            {
                logAction("Ошибка ввода данных : попытка создать не наклонную линию", color);
                validData = false;
            }
            break;
        }
        case 4:
            break;
    }
    return validData;
}

//Добавление нового элемента (инициализация параметров)
void Project::on_CreateButton_clicked()
{
    int previous = blank.isEmpty() ? 0 : (hasGap ? ui->EventList->currentRow() : blank.size() - 1);

    if(!isValid(ui->ElementParametrs->currentIndex(), previous))
    {
        ui->CreateButton->setEnabled(false);
        return;
    }

    switch (ui->ElementParametrs->currentIndex()) //ID текущего добавляемого элемента (0 - начальная точка / 1 - прямая X / 2 - прямая Z / 3 - прямая / 4 - окружность)
    {
        case 0:
        {
            blank.append(new StartingPoint (QPointF(ui->SP_argX_value->value(),ui->SP_argZ_value->value())));
            setAddEnable(true); //делаем доступными для нажатия кнопки добавления различных элементов
            break;
        }
        case 1:
        {
            blank.append(new HorizontalLine (blank[previous]->getEndPoint(), QPointF(blank[previous]->getEndPoint().x(), ui->LZ_argZ_value->value())));
            break;
        }
        case 2:
        {
            blank.append(new VerticalLine (blank[previous]->getEndPoint(), QPointF(ui->LX_argX_value->value(), blank[previous]->getEndPoint().y())));
            break;
        }
        case 3:
        {
            if(ui->L_argAlpha_value->value() == 0.0)
            {
                blank.append(new Line(blank[previous]->getEndPoint(), QPointF(ui->L_argX_value->value(), ui->L_argZ_value->value())));
            }
            else
            {

            }
            break;
        }
        case 4:
        {
            QString code = ui->DirectionButton->styleSheet().contains("left") ? "G03" : "G02";
            if(!isEqual(ui->C_argR_value->value(), 0.0))
            {
                Circle *element = new Circle(code, ui->C_argR_value->value(), blank[previous]->getEndPoint(), QPointF(ui->C_argX_value->value(), ui->C_argZ_value->value()));
                setSelectCenterEnable(true);
                setAddEnable(false);
                blank.append(element);
                static_cast<Circle*>(blank.last())->calculateCenters(ui);
                redraw(blank.size() - 1);
                ui->CreateButton->setEnabled(false);
                return;
            }
            else
            {
                blank.append(new Circle(code, QPointF(ui->C_argI_value->value(), ui->C_argJ_value->value()), blank[previous]->getEndPoint(), QPointF(ui->C_argX_value->value(), ui->C_argZ_value->value())));
            }
            break;
        }
    }

    //механизм отрисовки фасок и скруглений
    for(int i = 0; i <= transitions.size() - 1; i++)
    {
        if(!transitions[i].isDraw)
        {
            transitions[i].R != 0.0 ? drawRounding(i) : drawChamfer(i);
            transitions.removeLast();
            previous++;
        }
    }

    if((ui->argFSX_value->value() != 0.0) || (ui->argFSZ_value->value() != 0.0) || (ui->argR_value->value() != 0.0)||(ui->argAlpha_value->currentIndex() != 0))
    {
        Transition item;
        item.FSX = ui->argFSX_value->value();
        item.FSZ = ui->argFSZ_value->value();
        item.R = ui->argR_value->value();
        item.alpha = ui->argAlpha_value->currentText().toDouble() * M_PI / 180.0;
        item.isDraw = false;
        transitions.append(item);
    }

    if(blank.size() > 1)
    {
        if(hasGap && (previous != blank.size() - 2))
        {
            blank[previous]->getGraph()->setSelected(false);
            blank.insert(previous + 1, blank.last());
            blank.removeLast();

            if(!isContinuousLoop())
            {
                ui->EventList->setCurrentRow(previous + 1);
            }
            else
            {
                hasGap = false;
                ui->EventList->clearSelection();
                logAction("Разрыв контура устранен", Qt::darkGreen);
            }

            if(blank[previous + 1]->getEndPoint() == blank[previous + 2]->getStartPoint())
            {
                if((typeid(*blank[previous + 1]) == typeid(*blank[previous + 2])) && (combineElements(previous + 2)))
                {
                    redraw(previous + 1);
                    hasGap ?  ui->EventList->setCurrentRow(previous) : ui->EventList->clearSelection();
                    return;
                }
            }
        }

        if((typeid(*blank[previous]) == typeid(*blank[previous + 1])) && (combineElements(previous + 1)))
        {
            redraw(previous);
            hasGap ?  ui->EventList->setCurrentRow(previous) : ui->EventList->clearSelection();
            return;
        }

        previous++;
    }

    redraw(previous);
    addToEventList(previous);

    if(hasGap)
    {
        ui->EventList->setCurrentRow(previous);
    }

    ui->CreateButton->setDisabled(true);
}

//Добавление элемента в общий список и вывод его на окно приложения
void Project::addToEventList(int index)
{
    if(index > ui->EventList->count() - 1)
    {
        ui->EventList->addItem(blank[index]->createItem()); //добавление созданного элемента на боковую панель
        logAction("Элемент " + blank[index]->GCode() + " успешно создан", Qt::black);
    }
    else
    {
        ui->EventList->insertItem(index, blank[index]->createItem());
        if(!hasGap)
        {
            logAction("Элемент " + QString::number(index + 1) + " изменен на " + blank[index]->GCode(), Qt::black);
        }
        else
        {
            logAction("Элемент " + blank[index]->GCode() + " успешно создан", Qt::black);
        }
    }
    ui->EventList->scrollToBottom();
}

//функция логирования
void Project::logAction(QString message, QColor textColor)
{
    ui->DialogBox->setTextColor(textColor);
    ui->DialogBox->insertPlainText(message);
    ui->DialogBox->insertPlainText("\n");
    ui->DialogBox->ensureCursorVisible();
}

//Функция автоматического устранения разрыва
void Project::autoFillGap(int index)
{
    if(isEqual(blank[index - 1]->getEndPoint().x(), blank[index]->getStartPoint().x()))
    {
        blank.insert(index, new HorizontalLine(blank[index - 1]->getEndPoint(), blank[index]->getStartPoint()));
    }
    else if(isEqual(blank[index - 1]->getEndPoint().y(), blank[index]->getStartPoint().y()))
    {
        blank.insert(index, new VerticalLine(blank[index - 1]->getEndPoint(), blank[index]->getStartPoint()));
    }
    else
    {
        blank.insert(index, new Line(blank[index - 1]->getEndPoint(), blank[index]->getStartPoint()));
    }
    ui->EventList->insertItem(index, blank[index]->createItem());
    redraw(index);
}

//сохранение G-кода в файл
void Project::saveFile()
{
    while(ui->EventList->count() != blank.size())
    {
        blank.removeLast();
    }

    for(int i = 1; i < blank.size(); i++)
    {
        if(blank[i]->getStartPoint() != blank[i - 1]->getEndPoint())
        {
            autoFillGap(i);
            i--;
        }
    }

    QFile saveFile(QApplication::applicationDirPath() + "/models/" + ui->string_name->text() + ".nc"); //имя файла + место сохранения
    if (!saveFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) //открытие файла на запись и его очистка
    {
        QMessageBox::critical(this, "Ошибка","Не удается открыть файл для записи");
    }
    else
    {
        for(int i = 0; i < blank.size(); i++)//запись текущего G-элемента в файл
        {
            saveFile.write(blank[i]->GCode().toUtf8() + "\n");
        }
        saveFile.close();
    }
}

//Функция поиска наличия изменений
bool Project::isDifferent()
{
    QFile cur_file(QApplication::applicationDirPath() + "/models/" + ui->string_name->text() + ".nc");
    int rows = 0;
    bool hasDifferences = false;
    if ((cur_file.open(QIODevice::ReadOnly | QIODevice::Text)))
    {
       while(!cur_file.atEnd() && rows < blank.size())
       {
           if(cur_file.readLine() != blank[rows]->GCode().toUtf8() + "\n")
           {
                hasDifferences = true;
                break;
           }
           rows++;
       }
    }

    if(rows != blank.size() || !cur_file.atEnd())
    {
        hasDifferences = true;
    }

    return hasDifferences;
}

//Нажатие кнопки Завершить
void Project::on_EndButton_clicked()
{
    if (QFile(QApplication::applicationDirPath() + "/models/" + ui->string_name->text() + ".nc").exists()) //проверка, что перезаписываем существующий файл
    {
        if(isDifferent())
        {
            QMessageBox messageBox;
            messageBox.setWindowTitle("Перезапись файл");
            messageBox.setIcon(QMessageBox::Question);
            messageBox.setText("Файл \"" + ui->string_name->text() + ".nc\" уже существует. Вы уверены, что хотите перезаписать его?");
            messageBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
            messageBox.setButtonText(QMessageBox::Ok, tr("Да"));
            messageBox.setButtonText(QMessageBox::Cancel, tr("Нет"));
            messageBox.setDefaultButton(QMessageBox::Ok);
            switch (messageBox.exec()) //подтверждение или отклонение перезаписи файла
            {
                case QMessageBox::Ok: //подтверждение и последующая перезапись файла
                    saveFile();
                    break;
                case QMessageBox::Cancel: //отмена перезаписи
                    bool ok;
                    QString newBlankName = "";
                    while(newBlankName.isEmpty())
                    {
                        newBlankName = QInputDialog::getText(this, "Переименовать", "Введите новое имя заготовки:", QLineEdit::Normal, "", &ok); //ввод нового имени для файла
                        if(!ok) //отмена ввода -> полностью отменяем запись в файл;
                        {
                            return;
                        }
                        else if(!newBlankName.isEmpty()) //если введеное имя не пустое занова вызываем функцию записи в файл
                        {
                            ui->string_name->setText(newBlankName);
                            on_EndButton_clicked();
                        }
                    }
                    break;
            }
        }
    }
    else //если файла с указанным именем нет, то создаем его и записываем в него наш G-код
    {
        saveFile();
    }

    Widget *window = new Widget();//открытие стартового окна
    window->show();
    close();
}

//Функция парсинга открываемого файла
//project_name - имя открываемого файла
void Project::parseFile(QString project_name)
{
    QFile blankFile(QApplication::applicationDirPath() + "/models/" + project_name + ".nc");
    blankFile.open(QIODevice::ReadOnly);
    //BlankElement elem;
    QStringList commandArguments = QString(blankFile.readLine().trimmed()).split(" ");
    blank.append(new StartingPoint(QPointF(commandArguments[1].mid(1,commandArguments[1].size() - 1).toDouble(), commandArguments[2].mid(1,commandArguments[2].size() - 1).toDouble())));
    addToEventList(blank.size() - 1);
    while(!blankFile.atEnd())
    {
        commandArguments = QString(blankFile.readLine().trimmed()).split(" ");
        if(commandArguments[0] == "G01")
        {
            if(commandArguments.size() > 2)
            {
                blank.append(new Line(blank.last()->getEndPoint(),
                                      QPointF(commandArguments[1].mid(1,commandArguments[1].size() - 1).toDouble(), commandArguments[2].mid(1,commandArguments[2].size() - 1).toDouble())));
            }
            else
            {
                if(commandArguments[1].startsWith("Z"))
                {
                   blank.append(new HorizontalLine(blank.last()->getEndPoint(),
                                                    QPointF(blank.last()->getEndPoint().x(), commandArguments[1].mid(1,commandArguments[1].size() - 1).toDouble())));
                }
                if(commandArguments[1].startsWith("X"))
                {
                    blank.append(new VerticalLine(blank.last()->getEndPoint(),
                                                     QPointF(commandArguments[1].mid(1,commandArguments[1].size() - 1).toDouble(), blank.last()->getEndPoint().y())));
                }
            }
        }
        else if((commandArguments[0] == "G02") || (commandArguments[0] == "G03"))
        {
            QPointF end, center;
            end = QPointF(commandArguments[1].mid(1,commandArguments[1].size() - 1).toDouble(), commandArguments[2].mid(1,commandArguments[2].size() - 1).toDouble());
            if(commandArguments[3].startsWith("R"))
            {
                Circle *element = new Circle(commandArguments[0], commandArguments[3].mid(1,commandArguments[3].size() - 1).toDouble(), blank.last()->getEndPoint(), end);
                setSelectCenterEnable(true);
                logAction("Выберите необходимый центр окружности", Qt::black);
                blank.append(element);
                static_cast<Circle*>(blank.last())->calculateCenters(ui);
                redraw(blank.size() - 1);
                QEventLoop loop;
                connect(ui->AddCircleCenter, SIGNAL(clicked()), &loop, SLOT(quit()));
                loop.exec();
                center = element->getArcCenter();
                continue;
            }
            else
            {
                center = QPointF(commandArguments[3].mid(1,commandArguments[3].size() - 1).toDouble(), commandArguments[4].mid(1,commandArguments[4].size() - 1).toDouble());
            }
            blank.append(new Circle(commandArguments[0], center, blank.last()->getEndPoint(), end));
        }
        else
        {
            logAction("Ошибка чтения файла : неизвестная команда " + commandArguments.join(" ").trimmed() + " в строке " + QString::number(blank.size() + 1), Qt::red);
            return;
        }
        redraw(blank.size() - 1);
        addToEventList(blank.size() - 1);
    }
}

void Project::on_RemoveButton_clicked()
{
    int removeIndex = ui->EventList->currentRow();
    ui->EventList->clearSelection();
    delete ui->EventList->item(removeIndex);
    logAction("Элемент " + blank[removeIndex]->GCode() + " успешно удален", Qt::black);
    delete blank[removeIndex]->getGraph();
    blank.remove(removeIndex);
    ui->widget->replot();

    if(removeIndex < blank.size())
    {
        QMessageBox messageBox;
        messageBox.setWindowTitle("Разрыв контура");
        messageBox.setIcon(QMessageBox::Question);
        messageBox.setText("Хотите автоматически устранить его?");
        messageBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        messageBox.setButtonText(QMessageBox::Yes, tr("Да"));
        messageBox.setButtonText(QMessageBox::No, tr("Нет"));
        messageBox.setDefaultButton(QMessageBox::Yes);

        ui->RemoveButton->setEnabled(false);
        ui->ChangeButton->setEnabled(false);

        switch (messageBox.exec())
        {
            case QMessageBox::Yes:
            {
                autoFillGap(removeIndex);
                logAction("Элемент " + blank[removeIndex]->GCode() + " успешно создан", Qt::black);
                logAction("Разрыв контура устранен", Qt::darkGreen);
                break;
            }
            case QMessageBox::No:
            {
                hasGap = true;
                ui->EventList->setCurrentRow(removeIndex - 1);
                logAction("Контур не замкнут. Устраните разрыв", Qt::darkYellow);
                break;
            }
        }
    }
}

void Project::on_ChangeButton_clicked()
{
    int currentIndex = ui->EventList->currentRow();
    blank[currentIndex]->changeParameters(ui);
    addToEventList(currentIndex);
    delete ui->EventList->item(currentIndex);
    ui->EventList->clearSelection();
    setAxesRange(blank[currentIndex]->getGraph()->data()->values());
    ui->widget->replot();
    ui->ChangeButton->setDisabled(true);
    ui->RemoveButton->setDisabled(true);
}

//============================================================
//======== блокировка = взаимноисключающих = значений ========
//============================================================
void Project::on_L_argX_value_valueChanged(double arg1)
{
    if(arg1 != 0.0)
    {
        ui->L_argAlpha_value->setEnabled(false);
    }
    else if(ui->L_argZ_value->value() == 0.0)
    {
        ui->L_argAlpha_value->setEnabled(true);
    }
}

void Project::on_L_argZ_value_valueChanged(double arg1)
{
    if(arg1 != 0.0)
    {
        ui->L_argAlpha_value->setEnabled(false);
    }
    else if(ui->L_argX_value->value() == 0.0)
    {
        ui->L_argAlpha_value->setEnabled(true);
    }
}

void Project::on_L_argAlpha_value_valueChanged(double arg1)
{
    if(arg1 != 0.0)
    {
        ui->L_argX_value->setEnabled(false);
        ui->L_argZ_value->setEnabled(false);
    }
    else
    {
        ui->L_argX_value->setEnabled(true);
        ui->L_argZ_value->setEnabled(true);
    }
}

void Project::on_C_argR_value_valueChanged(double arg1)
{
    if(arg1 != 0.0)
    {
        ui->C_argI_value->setEnabled(false);
        ui->C_argJ_value->setEnabled(false);
    }
    else
    {
        ui->C_argI_value->setEnabled(true);
        ui->C_argJ_value->setEnabled(true);
    }
}

void Project::on_C_argI_value_valueChanged(double arg1)
{
    if(arg1 != 0.0)
    {
        ui->C_argR_value->setEnabled(false);
    }
    else if(ui->C_argJ_value->value() == 0.0)
    {
        ui->C_argR_value->setEnabled(true);
    }
}

void Project::on_C_argJ_value_valueChanged(double arg1)
{
    if(arg1 != 0.0)
    {
        ui->C_argR_value->setEnabled(false);
    }
    else if(ui->C_argI_value->value() == 0.0)
    {
        ui->C_argR_value->setEnabled(true);
    }
}

void Project::on_argR_value_valueChanged(double arg1)
{
    if(arg1 != 0.0)
    {
        ui->argAlpha_value->setEnabled(false);
        ui->argFSX_value->setEnabled(false);
        ui->argFSZ_value->setEnabled(false);
    }
    else
    {
        ui->argAlpha_value->setEnabled(true);
        ui->argFSX_value->setEnabled(true);
        ui->argFSZ_value->setEnabled(true);
    }
}

void Project::on_argFSX_value_valueChanged(double arg1)
{
    if(arg1 != 0.0)
    {
        ui->argR_value->setEnabled(false);
        ui->argFSZ_value->setEnabled(false);
    }
    else if(ui->argAlpha_value->currentIndex() == 0)
    {
        ui->argR_value->setEnabled(true);
        ui->argFSZ_value->setEnabled(true);
    }
}

void Project::on_argFSZ_value_valueChanged(double arg1)
{
    if(arg1 != 0.0)
    {
        ui->argR_value->setEnabled(false);
        ui->argFSX_value->setEnabled(false);
    }
    else if(ui->argAlpha_value->currentIndex() == 0)
    {
        ui->argR_value->setEnabled(true);
        ui->argFSX_value->setEnabled(true);
    }
}

void Project::on_argAlpha_value_currentIndexChanged(int index)
{
    if(index != 0)
    {
        ui->argR_value->setEnabled(false);
    }
    else if((ui->argFSX_value->value() == 0.0) && (ui->argFSZ_value->value() == 0.0))
    {
        ui->argR_value->setEnabled(true);
        ui->argFSX_value->setEnabled(true);
        ui->argFSZ_value->setEnabled(true);
    }
}

void Project::on_Show3D_clicked()
{
    if(blank.isEmpty())
    {
        logAction("Невозможно отобразить 3D вид : пустой контур", Qt::red);
        return;
    }

    if(isContinuousLoop())
    {
        Viewer3D *viewer = new Viewer3D();//открытие стартового окна
        viewer->show();
        connect(this, SIGNAL(sendPointsData(QVector <double>, QVector <double>)), viewer, SLOT(receivePointsData(QVector <double>, QVector <double>)));

        QVector <double> xPoints, zPoints;
        xPoints.append(blank[0]->getStartPoint().y());
        zPoints.append(blank.last()->getEndPoint().x());
        xPoints.append(blank[0]->getStartPoint().x());
        zPoints.append(blank[0]->getStartPoint().y());

        for(int i = 1; i < blank.size(); i++)
        {
            if(dynamic_cast<Circle*>(blank[i]))
            {
                QList<QCPCurveData> data = blank[i]->getGraph()->data()->values();
                int delta = data.size() / 4;
                if(data[0].key < data.last().key)
                {
                    for(int j = delta; j < data.size(); j += delta)
                    {
                        xPoints.append(data[j].value);
                        zPoints.append(data[j].key);
                    }
                }
                else
                {
                    for(int j = data.size() - 1; j >= delta; j -= delta)
                    {
                        xPoints.append(data[j].value);
                        zPoints.append(data[j].key);
                    }
                }
            }
            else
            {
                xPoints.append(blank[i]->getEndPoint().x());
                zPoints.append(blank[i]->getEndPoint().y());
            }
        }

        xPoints.append(blank[0]->getStartPoint().y());
        zPoints.append(blank.last()->getEndPoint().x());

        emit sendPointsData(xPoints, zPoints);
    }
    else
    {
        logAction("Невозможно отобразить 3D вид : контур содержит разрыв", Qt::red);
    }
}

void Project::on_ChangeAxis_clicked()
{
    axis++;
    axis = axis < 3 ? axis : 0;
    extremumValues.fill(0.0, 2);
    switch(axis)
    {
        case 0:
            for(int i = 1; i < blank.size(); i++)
            {
                delete graphs[i - 1];
                blank[i]->getGraph()->setVisible(true);
                setAxesRange(blank[i]->getGraph()->data()->values());
            }
            graphs.clear();
            ui->widget->xAxis->setLabel("Z");
            ui->widget->yAxis->setLabel("X");
            break;
        case 1:
            for(int i = 1; i < blank.size(); i++)
            {
                blank[i]->getGraph()->setVisible(false);
                QVector <double> xPoints, zPoints;
                double R = blank[i]->getEndPoint().x();
                for(double t = 0.0; t < 360.0; t += 0.025)
                {
                    xPoints.append(R * qCos(t * M_PI / 180.0));
                    zPoints.append(R * qSin(t * M_PI / 180.0));
                }
                graphs.append(new QCPCurve(ui->widget->xAxis, ui->widget->yAxis));
                graphs[i - 1]->setData(zPoints,xPoints);
                setAxesRange(graphs[i - 1]->data()->values());
            }
            ui->widget->xAxis->setLabel("Y");
            ui->widget->yAxis->setLabel("X");
            break;
        case 2:
            for(int i = 1; i < blank.size(); i++)
            {
                QVector <double> xPoints, zPoints;
                double R = blank[i]->getEndPoint().y();
                for(double t = 0.0; t < 360.0; t += 0.025)
                {
                    xPoints.append(R * qCos(t * M_PI / 180.0));
                    zPoints.append(R * qSin(t * M_PI / 180.0));
                }
                graphs[i - 1]->setData(zPoints,xPoints);
                setAxesRange(graphs[i - 1]->data()->values());
            }
            ui->widget->xAxis->setLabel("Z");
            ui->widget->yAxis->setLabel("Y");
            break;
    }
    ui->widget->replot();
}
