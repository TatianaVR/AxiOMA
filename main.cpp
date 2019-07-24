#include "widget.h"
#include "headers.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/data/icon.png"));
    Widget w;
    w.show();

    return a.exec();
}
