QT       += core gui network


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets



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
    main.cpp \
    mainwindow.cpp \
    ../shared/romkin.cpp\
    moduleemulator.cpp


HEADERS += \
    ../include/RomerinMessage.h \
    mainwindow.h \  
    ../shared/romkin.h \
    moduleemulator.h \

#source adapated from the original modules
SOURCES += \
       rom_adapted/RomerinMsgA.cpp \
       rom_adapted/RomSuctionCupA.cpp \
       rom_adapted/RomJointsA.cpp

HEADERS += \
       rom_adapted/RomerinDefinitionsA.h \
       rom_adapted/RomerinMsgA.h \
       rom_adapted/RomJointsA.h \
       rom_adapted/RomSuctionCupA.h \
       rom_adapted/RomMotorInfoA.h

FORMS += \
    mainwindow.ui 
 
INCLUDEPATH += "../include/"
INCLUDEPATH += "rom_adapted/"


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
