QT       += core gui serialport network
#libqt5serialport5 libqt5serialport5-dev

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
lessThan(QT_MAJOR_VERSION, 6): QT += gamepad
#libqt5gamepad5 libqt5gamepad5-dev

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
    btport.cpp \
    configdlg.cpp \
    emulationwnd.cpp \
    main.cpp \
    mainwindow.cpp \
    modulecontroller.cpp \
    moduleshandler.cpp \
    motorwidget.cpp \
    romerinmodel.cpp \
    romerinmodule.cpp \
    apoloconection.cpp \
    ../shared/romkin.cpp \
    ../shared/apoloMessage.cpp \
    components/joypad.cpp

HEADERS += \
    ../include/RomerinMessage.h \
    ../shared/romkin.h \
    ../shared/apoloMessage.h \
    apoloconection.h \
    btport.h \
    configdlg.h \
    emulationwnd.h \
    mainwindow.h \
    modulecontroller.h \
    moduleshandler.h \
    motorwidget.h \
    romerinmodel.h \
    romerinmodule.h \
    components/joypad.h \
    components/QtGamepadFake.h \
    components/utils.h

FORMS += \
    configdlg.ui \
    emulationwnd.ui \
    mainwindow.ui \
    motorwidget.ui \
    romerinmodule.ui


INCLUDEPATH += "../include/"
INCLUDEPATH += "../shared/"
#logo created with: magick.exe https://doc.qt.io/qt-6/appicon.html
RC_ICONS =romerin.ico
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
