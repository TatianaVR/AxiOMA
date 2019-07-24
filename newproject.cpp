#include "newproject.h"
#include "ui_newproject.h"

#include "project.h"

NewProject::NewProject(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NewProject)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::FramelessWindowHint); //нет рамки

    ui->project_name->setFocus(); //автоматическое помещение курсора в строку ввода
    ui->project_name->setValidator(new QRegExpValidator(QRegExp("[A-Za-z\\d]+"), this)); //ограничение на ввод

    connect(ui->project_name, SIGNAL(textChanged(QString)), this, SLOT(clearErrorMessage()));
}

//удаление формы
NewProject::~NewProject()
{
    delete ui;
}

//нажатие кнопки "Да"
void NewProject::on_YesButton_clicked()
{
    if(ui->project_name->text() == "")
    {
        ui->error_label->setText("Имя не может быть пустым");
    }
    else if(QFile(QApplication::applicationDirPath() + "/models/" + ui->project_name->text() + ".nc").exists())
    {
        ui->error_label->setText("Файл с таким именем уже существует");
    }
    else
    {
        Project *project = new Project();
        project->show();
        connect(this, SIGNAL(sendOptions(QString, bool)), project, SLOT(receiveOptions(QString, bool)));
        emit sendOptions(ui->project_name->text(), false);
        emit destroyed();
        close();
    }
}

//нажатие кнопки "Нет"
void NewProject::on_NoButton_clicked()
{
    close();
}

void NewProject::clearErrorMessage()
{
    ui->error_label->setText("");
}
