#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

signals:
    void sendOptions(QString project_name, bool isExist);

private slots:
    void getFileList();

    void on_SelectButton_clicked();
    void on_CreateButton_clicked();
    void on_RemoveButton_clicked();
    void on_ExitButton_clicked();
    void on_BlanksFiles_clicked(const QModelIndex &index);
    void on_BlanksFiles_doubleClicked(const QModelIndex &index);

private:
    Ui::Widget *ui;

    void mousePressEvent(QMouseEvent *event);
    void updateFilesList();
};

#endif // WIDGET_H
