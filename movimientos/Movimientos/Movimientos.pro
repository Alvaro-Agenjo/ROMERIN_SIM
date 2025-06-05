# QT       += core gui
QT       += core gui network charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
#//--------------------------------------------//
lessThan(QT_MAJOR_VERSION, 6): QT += gamepad



CONFIG += c++17

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    components/joypad.cpp \
    configdlg.cpp \
    main.cpp \
    mainwindow.cpp \
    module.cpp \
    modulecontroller.cpp \
    moduleshandler.cpp \
    ../shared/romkin.cpp \
    motor.cpp \
    trayectorygenerator.cpp

HEADERS += \
    ../include/RomerinMessage.h \
    ../shared/romkin.h \
    components/QtGamepadFake.h \
    components/joypad.h \
    components/utils.h \
    configdlg.h \
    module.h \
    modulecontroller.h \
    moduleshandler.h \
    mainwindow.h \
    motor.h \
    trayectorygenerator.h

FORMS += \
    configdlg.ui \
    mainwindow.ui \
    module.ui \
    motor.ui

INCLUDEPATH += "../include/"
INCLUDEPATH += "../shared/"


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    components/LICENSE
