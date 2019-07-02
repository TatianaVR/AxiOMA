#include "widget.h"
#include "project.h"
#include "viewer3d.h"
#include <QApplication>

#include <QIcon>
#include <blankelement.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/data/icon.png"));
    //Widget w;
    Viewer3D w;

    w.show();

    return a.exec();
}
