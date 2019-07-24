#include "widget.h"
#include "ui_widget.h"

#include <newproject.h>
#include <project.h>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::FramelessWindowHint); //нет рамки

    //получение файлов моделей
    getFileList();

    //отслеживание изменений в папке с моделями
    QFileSystemWatcher *watcher = new QFileSystemWatcher(this);
    watcher->addPath(QApplication::applicationDirPath() + "/models");
    connect(watcher, SIGNAL(directoryChanged(QString)), this, SLOT(getFileList()));

    connect(ui->BlanksFiles, SIGNAL(doubleClicked(QModelIndex)), ui->SelectButton, SLOT(click())); //открытие файла по двойному нажатию клавиши мыши

}

//удаление формы
Widget::~Widget()
{
    delete ui;
}

//снятие активного выделения при нажатии мышки
void Widget::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    if (!ui->BlanksFiles->selectedItems().isEmpty())
    {
        ui->SelectButton->setEnabled(false);
        ui->RemoveButton->setEnabled(false);
        ui->BlanksFiles->clearSelection();
    }
}

//активация кнопок при активации
void Widget::on_BlanksFiles_clicked(const QModelIndex &index)
{
    if(index.row() > 0)
    {
        ui->SelectButton->setEnabled(true);
        ui->RemoveButton->setEnabled(true);
    }
}

//открытие файла
void Widget::on_SelectButton_clicked()
{
    Project *window = new Project();
    window->show();
    connect(this, SIGNAL(sendOptions(QString, bool)), window, SLOT(receiveOptions(QString, bool)));
    emit sendOptions(ui->BlanksFiles->item(ui->BlanksFiles->currentRow(), 1)->text(), true);
    close();
}

//открытие окна для ввода имени новой заготовки
void Widget::on_CreateButton_clicked()
{
    NewProject *window = new NewProject();
    window->show();
    connect(window, SIGNAL(destroyed()), this, SLOT(close()));
}

//удаление файлов
void Widget::on_RemoveButton_clicked()
{
    QString fileName = ui->BlanksFiles->item(ui->BlanksFiles->currentRow(), 1)->text();
    QMessageBox messageBox;
    messageBox.setWindowTitle("Удалить файл");
    messageBox.setIcon(QMessageBox::Question);
    messageBox.setText("Вы действительно хотите безвозвратно удалить \"" + fileName + ".nc\" файл ?");
    messageBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    messageBox.setButtonText(QMessageBox::Ok, tr("Да"));
    messageBox.setButtonText(QMessageBox::Cancel, tr("Нет"));
    messageBox.setDefaultButton(QMessageBox::Ok);
    switch (messageBox.exec())
    {
        case QMessageBox::Ok:
        {
            QFile file(QApplication::applicationDirPath() + "/models/" + fileName + ".nc");
            file.setPermissions(QFile::ReadOwner | QFile::WriteOwner);
            file.remove();
            break;
        }
        case QMessageBox::Cancel:
            ui->BlanksFiles->clearSelection();
            break;
        default:
            break;
    }
    ui->SelectButton->setEnabled(false);
    ui->RemoveButton->setEnabled(false);
}

//закрытие окна
void Widget::on_ExitButton_clicked()
{
    close();
}

//вычисление размера файла в соответствующих единицах
QString getFileSize(qint64 nSize)
{
    qint64 i = 0;
    for (; nSize > 1023; nSize /= 1024, ++i) { }
    return QString().setNum(nSize) + " " +" KMGT"[i] + "b";
}

//создание заголовка
void Widget::updateFilesList()
{
    ui->BlanksFiles->clear();
    ui->BlanksFiles->setRowCount(1);
    ui->BlanksFiles->setColumnCount(5);

    QStringList headers = QString(";File Name;Modification Date;Modification Time;Size").split(";");
    for(int iColumn = 0; iColumn < ui->BlanksFiles->columnCount(); iColumn++)
    {
        ui->BlanksFiles->setItem(0, iColumn, new QTableWidgetItem(tr("%1").arg(headers[iColumn]))); //имя
        ui->BlanksFiles->item(0, iColumn)->setBackground(QColor(65, 105, 225)); //цвет ячейки
        ui->BlanksFiles->item(0, iColumn)->setData(Qt::FontRole, QFont("Arial", 12, QFont::Bold)); //шрифт
        ui->BlanksFiles->item(0, iColumn)->setTextColor(Qt::white); //цвет текста
        ui->BlanksFiles->item(0, iColumn)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter); //выравнивание по центру
        ui->BlanksFiles->item(0, iColumn)->setFlags(Qt::ItemIsSelectable); //запрет на выделение
    }
}

//получение файлов моделей
void Widget::getFileList()
{
    updateFilesList();

    QDir dir(QApplication::applicationDirPath() + "/models"); //директория, в которой хранятся заготовки
    QStringList file_extension;//расширение файлов
    file_extension << "*.nc";
    QFileInfoList list = dir.entryInfoList(file_extension, QDir::Files);//формирование списков по расширению
    QFileInfo current_file;
    QBrush brush;
    brush.setTextureImage(QImage(":/data/file.png"));
    foreach (current_file, list) //вывод файлов
    {
        QTableWidgetItem *item = new QTableWidgetItem();
        item->setBackground(brush);
        ui->BlanksFiles->insertRow(ui->BlanksFiles->rowCount());
        ui->BlanksFiles->setItem(ui->BlanksFiles->rowCount() - 1, 0, item);
        ui->BlanksFiles->setItem(ui->BlanksFiles->rowCount() - 1, 1, new QTableWidgetItem(tr("%1").arg(current_file.baseName())));
        ui->BlanksFiles->setItem(ui->BlanksFiles->rowCount() - 1, 2, new QTableWidgetItem(tr("%1").arg(current_file.lastModified().toString("dd.MM.yyyy"))));
        ui->BlanksFiles->setItem(ui->BlanksFiles->rowCount() - 1, 3, new QTableWidgetItem(tr("%1").arg(current_file.lastModified().toString("hh:mm"))));
        ui->BlanksFiles->setItem(ui->BlanksFiles->rowCount() - 1, 4, new QTableWidgetItem(tr("%1").arg(getFileSize(current_file.size()))));
    }

    ui->BlanksFiles->setColumnWidth(0,32);
    for(int iColumn = 1; iColumn < ui->BlanksFiles->columnCount(); iColumn++)
    {
        ui->BlanksFiles->horizontalHeader()->setSectionResizeMode(iColumn, QHeaderView::Stretch);
    }
}
