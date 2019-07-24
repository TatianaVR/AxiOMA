#ifndef NEWPROJECT_H
#define NEWPROJECT_H

namespace Ui {
class NewProject;
}

class NewProject : public QWidget
{
    Q_OBJECT

public:
    explicit NewProject(QWidget *parent = 0);
    ~NewProject();

signals:
    void sendOptions(QString project_name, bool isExist);

private slots:
    void on_YesButton_clicked();
    void on_NoButton_clicked();

    void clearErrorMessage();

private:
    Ui::NewProject *ui;
};

#endif // NEWPROJECT_H
