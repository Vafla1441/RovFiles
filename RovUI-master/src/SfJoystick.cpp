#include "SfJoystick.hpp"
#include "RovSingleton.hpp"
#include <QDebug>
#include <SFML/Window.hpp>

SfJoystick::SfJoystick(QObject* parent)
    : QObject(parent)
    , m_joystickSettingsDialog(new SfJoystickSettingsDialog)
{
    startTimer(80);
}

SfJoystick::~SfJoystick()
{
}

int SfJoystick::xAxis()
{
    return std::abs(m_x) > MinAxisValue ? m_x : 0;
}

int SfJoystick::yAxis()
{
    return std::abs(m_y) > MinAxisValue ? m_y : 0;
}

int SfJoystick::zAxis()
{
    return std::abs(m_z) > MinAxisValue ? m_z : 0;
}

int SfJoystick::wAxis()
{
    return std::abs(m_w) > MinAxisValue ? m_w : 0;
}

int SfJoystick::servoXAxis()
{
    return m_servoX;
}

int SfJoystick::servoYAxis()
{
    return m_servoY;
}

void SfJoystick::timerEvent(QTimerEvent*)
{
    sf::Joystick::update();

    if (isConnected()) {
        m_x = (int)sf::Joystick::getAxisPosition(0, m_joystickSettingsDialog.data()->xAxis());
        m_joystickSettingsDialog.data()->isXInvese() ? m_x *= -1 : m_x *= 1;

        m_y = (int)sf::Joystick::getAxisPosition(0, m_joystickSettingsDialog.data()->yAxis());
        m_joystickSettingsDialog.data()->isYInvese() ? m_y *= -1 : m_y *= 1;

        m_z = (int)sf::Joystick::getAxisPosition(0, m_joystickSettingsDialog.data()->zAxis());
        m_joystickSettingsDialog.data()->isZInvese() ? m_z *= -1 : m_z *= 1;

        m_w = (int)sf::Joystick::getAxisPosition(0, m_joystickSettingsDialog.data()->wAxis());
        m_joystickSettingsDialog.data()->isWInvese() ? m_w *= -1 : m_w *= 1;

        m_servoX = (int)sf::Joystick::getAxisPosition(0, m_joystickSettingsDialog.data()->servoXAxis());
        m_joystickSettingsDialog.data()->isServoXInvese() ? m_servoX *= -1 : m_servoX *= 1;

        m_servoY = (int)sf::Joystick::getAxisPosition(0, m_joystickSettingsDialog.data()->servoYAxis());
        m_joystickSettingsDialog.data()->isServoYInvese() ? m_servoY *= -1 : m_servoY *= 1;

        m_manipulatorRotate = (int)sf::Joystick::getAxisPosition(0, m_joystickSettingsDialog.data()->manipulatorAxis());
        m_manipulatorRotate = abs(m_manipulatorRotate) > 50 ? m_manipulatorRotate : 0;
        m_joystickSettingsDialog.data()->isManipulatorInvese() ? m_manipulatorRotate *= -1 : m_manipulatorRotate *= 1;

        m_joystickSettingsDialog.data()->updateJoystickTest();
    }

    if (isConnected()) {
        RovSingleton::instance()->control().axisW = static_cast<qint8>(m_w);
        RovSingleton::instance()->control().axisX = static_cast<qint8>(m_x);
        RovSingleton::instance()->control().axisY = static_cast<qint8>(m_y);
        RovSingleton::instance()->control().axisZ = static_cast<qint8>(m_z);
        RovSingleton::instance()->control().cameraRotation[0] = static_cast<qint8>(m_servoX);
        RovSingleton::instance()->control().cameraRotation[1] = static_cast<qint8>(m_servoY);
        RovSingleton::instance()->control().manipulatorRotation = m_manipulatorRotate;
        RovSingleton::instance()->control().manipulatorOpenClose = sf::Joystick::isButtonPressed(0, m_joystickSettingsDialog.data()->manipulatorOpenButton()) - sf::Joystick::isButtonPressed(0, m_joystickSettingsDialog.data()->manipulatorCloseButton());

        bool camSelectButtonState = sf::Joystick::isButtonPressed(0, m_joystickSettingsDialog.data()->cameraSelectButton());
        if (m_camSelectOldState != camSelectButtonState) {
            if (camSelectButtonState) {
                RovSingleton::instance()->control().cameraIndex = (RovSingleton::instance()->control().cameraIndex + 1) % 2;
            }
            m_camSelectOldState = camSelectButtonState;
        }

    } else {
        RovSingleton::instance()->control().axisW = static_cast<qint8>(0);
        RovSingleton::instance()->control().axisX = static_cast<qint8>(0);
        RovSingleton::instance()->control().axisY = static_cast<qint8>(0);
        RovSingleton::instance()->control().axisZ = static_cast<qint8>(0);
        RovSingleton::instance()->control().cameraRotation[0] = static_cast<qint8>(0);
        RovSingleton::instance()->control().cameraRotation[1] = static_cast<qint8>(0);
        RovSingleton::instance()->control().manipulatorRotation = 0;
    }

//    qDebug() << "\tX: " << xAxis()
//             << "\tY: " << yAxis()
//             << "\tZ: " << zAxis()
//             << "\tW: " << wAxis()
//             << "\t cam: " << RovSingleton::instance()->control().cameraIndex;
}

bool SfJoystick::isConnected()
{
    return sf::Joystick::isConnected(0);
}

SfJoystickSettingsDialog* SfJoystick::settingsDialog()
{
    return m_joystickSettingsDialog.data();
}
