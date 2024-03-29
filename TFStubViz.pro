#-------------------------------------------------
#
# Project created by QtCreator 2015-01-10T10:21:39
#
#-------------------------------------------------

#---- for Qt 5 only
QT       += core gui widgets


QMAKE_CC = gcc
QMAKE_CXX = g++

exists(/usr/bin/g++-10) {
    QMAKE_CXX = g++-10
    QMAKE_CC  = gcc-10
}
exists(/usr/bin/g++-11) {
    QMAKE_CXX = g++-11
    QMAKE_CC  = gcc-11
}

TARGET = TFStubViz
TEMPLATE = app
CONFIG += c++17
QMAKE_CXXFLAGS += -Wall -Wignored-qualifiers -Wparentheses -Woverloaded-virtual -Wsign-compare -Werror -std=c++17

SOURCES += main.cpp \
    buttonwidget.cpp \
    digitalin.cpp \
    dualbutton.cpp \
    dualsensor.cpp \
    lcd.cpp \
    lcddisplayarea.cpp \
    ledbutton.cpp \
    ledstrip.cpp \
    mainwindow.cpp \
    motionsensor.cpp \
    multisensor.cpp \
    oled.cpp \
    outdoorsensors.cpp \
    piezospeaker.cpp \
    relay.cpp \
    sensor.cpp \
    sensorinterface.cpp \
    serverthread.cpp \
    singleoutdoorsensor.cpp \
    statusled.cpp \
    touchpad.cpp \
    visualizationwidget.cpp

HEADERS += mainwindow.h \
    buttonwidget.h \
    digitalin.h \
    dualbutton.h \
    dualsensor.h \
    lcd.h \
    lcddisplayarea.h \
    ledbutton.h \
    ledstrip.h \
    motionsensor.h \
    multisensor.h \
    oled.h \
    outdoorsensors.h \
    piezospeaker.h \
    relay.h \
    sensor.h \
    sensorinterface.h \
    serverthread.h \
    singleoutdoorsensor.h \
    statusled.h \
    touchpad.h \
    visualizationwidget.h

FORMS += mainwindow.ui \
    digitalin.ui \
    dualbutton.ui \
    lcd.ui \
    ledbutton.ui \
    ledstrip.ui \
    motionsensor.ui \
    oled.ui \
    outdoorsensors.ui \
    piezo.ui \
    poti.ui \
    relay.ui \
    sensor.ui \
    touchpad.ui

unix:CONFIG(release, debug|release): LIBS += -L$$PWD/../TFStubserver/lib/Release -lstubserver -lutils -lasound
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/../TFStubserver/lib/Debug -lstubserver -lutils -lasound

unix:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../TFStubserver/lib/Release/libstubserver.a
else:unix:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../TFStubserver/lib/Debug/libstubserver.a

RESOURCES += \
    images.qrc

INCLUDEPATH += $$PWD/../TFStubserver/src
DEPENDPATH += $$PWD/../TFStubserver/src

unix:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../TFStubserver/lib/Release/libutils.a
else:unix:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../TFStubserver/lib/Debug/libutils.a
