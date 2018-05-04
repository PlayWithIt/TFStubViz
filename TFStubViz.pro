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
    statusled.cpp

HEADERS  += mainwindow.h \
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
    statusled.h

FORMS    += mainwindow.ui touchpad.ui lcd.ui \
    sensor.ui \
    relay.ui \
    motionsensor.ui \
    poti.ui \
    ledstrip.ui \
    dualbutton.ui \
    oled.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../TFStubserver/lib/Debug/release/ -lstubserver -lutils
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../TFStubserver/lib/Debug/debug/ -lstubserver -lutils
else:unix:!macx: LIBS += -L$$PWD/../TFStubserver/lib/Debug/ -lstubserver -lutils -lasound

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../TFStubserver/lib/Debug/release/libstubserver.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../TFStubserver/lib/Debug/debug/libstubserver.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../TFStubserver/lib/Debug/release/stubserver.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../TFStubserver/lib/Debug/debug/stubserver.lib
else:unix:!macx: PRE_TARGETDEPS += $$PWD/../TFStubserver/lib/Debug/libstubserver.a

RESOURCES += \
    images.qrc

INCLUDEPATH += $$PWD/../TFStubserver/src
DEPENDPATH += $$PWD/../TFStubserver/src

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../TFStubserver/lib/Debug/release/libutils.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../TFStubserver/lib/Debug/debug/libutils.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../TFStubserver/lib/Debug/release/utils.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../TFStubserver/lib/Debug/debug/utils.lib
else:unix: PRE_TARGETDEPS += $$PWD/../TFStubserver/lib/Debug/libutils.a
