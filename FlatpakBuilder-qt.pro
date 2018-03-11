#-------------------------------------------------
#
# Project created by QtCreator 2018-01-26T16:28:25
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FlatpakBuilder-qt
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


SOURCES += main.cpp \
    buildwizard.cpp \
    manifestcontainer.cpp \
    exeprober.cpp \
    flatpaklauncher.cpp \
    builderinstance.cpp \
    dialogconfigmodule.cpp

HEADERS  += \
    buildwizard.h \
    manifestcontainer.h \
    manifestcontainerprivate.h \
    manifest_format.h \
    exeprober.h \
    exeproberprivate.h \
    flatpaklauncher.h \
    flatpaklauncherprivate.h \
    builderinstance.h \
    builderinstanceprivate.h \
    dialogconfigmodule.h

FORMS    += \
    buildwizard.ui \
    dialogconfigmodule.ui

target.path = $${PREFIX}/bin/

INSTALLS += target
