#pragma once

#include "SfJoystickSettingsDialog.hpp"
#include <QObject>

class SfJoystick : public QObject {
    Q_OBJECT
public:
    SfJoystick(QObject* parent = nullptr);

    ~SfJoystick();

    int xAxis();

    int yAxis();

    int zAxis();

    int wAxis();

    int servoXAxis();

    int servoYAxis();

    bool isConnected();

    SfJoystickSettingsDialog* settingsDialog();

private slots:

    void timerEvent(QTimerEvent*);

private:
    int m_x = 0;
    int m_y = 0;
    int m_z = 0;
    int m_w = 0;
    int m_servoX = 0;
    int m_servoY = 0;
    int m_manipulatorRotate = 0;
    int m_openManipulator = 0;
    int m_closeManipulator = 0;
    bool m_camSelectOldState = false;

    enum { MinAxisValue = 10 };

    QScopedPointer<SfJoystickSettingsDialog> m_joystickSettingsDialog;
};
