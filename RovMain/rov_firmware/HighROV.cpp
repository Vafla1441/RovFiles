#include "HighROV.h"
#include "PWMController.h"
#include "Networking.h"
#include "Data.h"
#include "Thrusters.h"
#include "RotaryCameras.h"
#include "DepthSensor.h"
#include "Manipulator.h"
#include "Config.h"
#include "IMUSensor.h"
#include "USB/USBAPI.h"
#include "AnalogSensors.h"

rov::RovControl control;
rov::RovTelimetry telimetry;

void HighROV::init() {
    //SerialUSB.println("HighROV init!");

    pinMode(LED_BUILTIN, OUTPUT);
    analogWrite(LED_BUILTIN, 100);

    PWMController::init();
    Networking::init();
    Thrusters::init();
    RotaryCameras::init();
    DepthSensor::init();
    Manipulator::init();
    IMUSensor::init();
    AnalogSensors::init();

    delay(3000);
}

void debug(rov::RovControl &ctrl) {
    using namespace config;

    PWMController::set_thruster(thrusters::horizontal_front_left,  ctrl.thrusterPower[0]);
    PWMController::set_thruster(thrusters::horizontal_front_right, ctrl.thrusterPower[1]);
    PWMController::set_thruster(thrusters::horizontal_back_left,   ctrl.thrusterPower[2]);
    PWMController::set_thruster(thrusters::horizontal_back_right,  ctrl.thrusterPower[3]);
    PWMController::set_thruster(thrusters::vertical_front,         ctrl.thrusterPower[4]);
    PWMController::set_thruster(thrusters::vertical_back,          ctrl.thrusterPower[5]);

    PWMController::set_servo_power(servos::front,  ctrl.thrusterPower[6]);
    PWMController::set_servo_power(servos::back,   ctrl.thrusterPower[7]);
    PWMController::set_servo_power(servos::pwm_a2, ctrl.thrusterPower[8]);
    PWMController::set_servo_power(servos::pwm_a3, ctrl.thrusterPower[9]);
}

void HighROV::run() {
    AnalogSensors::update();

    telimetry.yaw = IMUSensor::getYaw();
    telimetry.roll = IMUSensor::getRoll();
    telimetry.pitch = IMUSensor::getPitch();
    telimetry.depth = DepthSensor::getDepth();
    telimetry.temperature = DepthSensor::getTemp();
    telimetry.ammeter = AnalogSensors::getAmperage();
    telimetry.voltmeter = AnalogSensors::getVoltage();
    telimetry.cameraIndex = RotaryCameras::get_cam_index();

    // SerialUSB.print("\tDepth: "); SerialUSB.print(telimetry.depth);
    // SerialUSB.print("\tTemp: "); SerialUSB.print(telimetry.temperature);
    // delay(50);

    // // SerialUSB.print("\tYaw: "); SerialUSB.print(telimetry.yaw);
    // // SerialUSB.print("\tRoll: "); SerialUSB.print(telimetry.roll);
    // // SerialUSB.print("\tPitch: "); SerialUSB.print(telimetry.pitch);
    // SerialUSB.println();

    Networking::read_write_udp(telimetry, control);
    if (!control.debugFlag) {
        Thrusters::update_thrusters(control, telimetry);
        RotaryCameras::set_angle(config::servos::front, constrain(control.cameraRotation[0], -1, 1) * 3.0);
        RotaryCameras::set_angle(config::servos::back,  constrain(control.cameraRotation[1], -1, 1) * 3.0);
        RotaryCameras::set_angle(config::servos::pwm_a2,  constrain(control.cameraRotation[2], -1, 1) * 3.0);
        RotaryCameras::select_cam(control.cameraIndex == 1 ? true : false);
        Manipulator::set_power(control.manipulatorRotation, control.manipulatorOpenClose);
        AnalogSensors::set_power(control.pumpPower);
    } else {
        debug(control);
    }

    if (DepthSensor::getUpdateStatus() == true) {
        analogWrite(LED_BUILTIN, sin(millis() * 0.01) * 127 + 127);
    } else {
        analogWrite(LED_BUILTIN, 255);
    }

    delay(20);
}

