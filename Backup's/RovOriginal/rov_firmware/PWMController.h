#pragma once

#include <Wire.h>
#include <SparkFun_PCA9536_Arduino_Library.h>
#include <Servo.h>

class PWMController
{
public:
    static void init();
    static void set_servo_angle(int idx, int angle);
    static void set_servo_power(int idx, int power);
    static void set_thruster(int idx, int power);
    static void set_manipulator(int ch, int power);
    static void set_video_multiplexer(bool state);
    PWMController(const PWMController&) = delete;
private:
    static const int pulse_min = 1000;
    static const int pulse_med = 1500;
    static const int pulse_max = 2000;

    static PWMController &inst();
    PWMController();

    PCA9536 m_pca;

    /* controlled by SAMD_ISR_SERVO lib: */
    int m_pins[8];
    int m_isr_servos[8];

    /* controlled by Arduino Servo.h lib:*/
    Servo m_servos[4];
};

