#-------------------------------------------------
#
# Project created by QtCreator 2019-02-13T21:57:29
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CHPU
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += main.cpp\
    project.cpp \
    widget.cpp \
    newproject.cpp \
    qcustomplot.cpp \
    math_addition.cpp \
    blankelement.cpp \
    circle.cpp \
    horizontalline.cpp \
    verticalline.cpp \
    line.cpp \
    startingpoint.cpp \
    viewer3d.cpp \
    viewer_math.cpp \
    glwidget.cpp

HEADERS  += \
    project.h \
    widget.h \
    newproject.h \
    qcustomplot.h \
    math_addition.h \
    blankelement.h \
    verticalline.h \
    horizontalline.h \
    line.h \
    circle.h \
    startingpoint.h \
    viewer3d.h \
    viewer_math.h \
    glwidget.h

FORMS    += \
    project.ui \
    widget.ui \
    newproject.ui \
    viewer3d.ui

RESOURCES += \
    resources.qrc

win32
{
 LIBS += -lopengl32
}

