#-------------------------------------------------
#
# Project created by QtCreator 2018-01-26T16:28:25
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FlatpakBuilder-qt
TEMPLATE = app

VER_MAJ = 1
VER_MIN = 0
VER_PAT = 0
VERSION = 1.0.0

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS APP_VERSION=\\\"$$VERSION\\\"

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp \
    buildwizard.cpp \
    utils/manifestcontainer.cpp \
    utils/exeprober.cpp \
    utils/flatpaklauncher.cpp \
    utils/builderinstance.cpp \
    dialogconfigmodule.cpp \
    aboutwindow.cpp

HEADERS  += \
    buildwizard.h \
    utils/manifestcontainer.h \
    utils/manifestcontainerprivate.h \
    utils/manifest_format.h \
    utils/exeprober.h \
    utils/exeproberprivate.h \
    utils/flatpaklauncher.h \
    utils/flatpaklauncherprivate.h \
    utils/builderinstance.h \
    utils/builderinstanceprivate.h \
    dialogconfigmodule.h \
    aboutwindow.h

FORMS    += \
    buildwizard.ui \
    dialogconfigmodule.ui \
    aboutwindow.ui

target.path = $${PREFIX}/bin/

INSTALLS += target

RESOURCES += \
    icon.qrc
