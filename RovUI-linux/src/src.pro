QT       += core gui network multimedia multimediawidgets
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RovUI
TEMPLATE = app
DEFINES += QT_DEPRECATED_WARNINGS
CONFIG += static
PKGCONFIG += QtGStreamer-1.0

SOURCES += \
    main.cpp \
    MainWindow.cpp \
    Crc.cpp \
    RovTelimetryWidget.cpp \
    RovCameraWidget.cpp \
    ScaleFactorWidget.cpp \
    SfJoystick.cpp \
    SfJoystickSettingsDialog.cpp \
    RovSingleton.cpp \
    RovUdpConnection.cpp \
    RegulatorsWidget.cpp \
    DebugDialog.cpp \
    LogWidget.cpp

HEADERS += \
    MainWindow.hpp \
    CallOnce.hpp \
    Singleton.hpp \
    Crc.hpp \
    RovTelimetryWidget.hpp \
    RovCameraWidget.hpp \
    ScaleFactorWidget.hpp \
    SfJoystick.hpp \
    SfJoystickSettingsDialog.hpp \
    RovSingleton.hpp \
    RovUdpConnection.hpp \
    RovDataTypes.hpp \
    RegulatorsWidget.hpp \
    DebugDialog.hpp \
    LogWidget.hpp

include(deps/deps.pri)

INCLUDEPATH += ../SFML-2.4.1/include/

LIBS += -L../SFML-2.4.1/lib/ \
        -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lsfml-network

LIBS += -L../usr/local/lib/ -lVLCQtCore -lVLCQtWidgets