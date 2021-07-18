#-------------------------------------------------
#
# Project created by QtCreator 2015-01-10T10:21:39
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TFStubViz
TEMPLATE = app
QMAKE_CXXFLAGS += -std=c++11 -Wall -Wignored-qualifiers -Wparentheses -Woverloaded-virtual -Wsign-compare -Werror

SOURCES += main.cpp\
    digitalin.cpp \
    mainwindow.cpp \
    touchpad.cpp \
    lcd.cpp \
    serverthread.cpp \
    lcddisplayarea.cpp \
    sensor.cpp \
    relay.cpp \
    buttonwidget.cpp \
    motionsensor.cpp \
    sensorinterface.cpp \
    dualsensor.cpp \
    visualizationwidget.cpp \
    ledstrip.cpp \
    dualbutton.cpp \
    oled.cpp \
    statusled.cpp \
    multisensor.cpp \
    ledbutton.cpp

HEADERS  += mainwindow.h \
    digitalin.h \
    touchpad.h \
    lcd.h \
    serverthread.h \
    lcddisplayarea.h \
    sensor.h \
    relay.h \
    buttonwidget.h \
    motionsensor.h \
    sensorinterface.h \
    dualsensor.h \
    visualizationwidget.h \
    ledstrip.h \
    dualbutton.h \
    oled.h \
    statusled.h \
    multisensor.h \
    ledbutton.h

FORMS    += mainwindow.ui touchpad.ui lcd.ui \
    digitalin.ui \
    sensor.ui \
    relay.ui \
    motionsensor.ui \
    poti.ui \
    ledstrip.ui \
    dualbutton.ui \
    oled.ui \
    ledbutton.ui

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
