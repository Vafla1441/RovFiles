#include "PWMController.h"
#include "Config.h"

#include "src/SAMD_ISR_Servo/SAMD_ISR_Servo.h"

void PWMController::init() {
    using namespace config::pwm;
    using namespace config;

    auto &pwm = inst();

    /* SAMD ISR Servos init */

    SAMD_ISR_Servos.useTimer(TIMER_TCC);

    pwm.m_pins[thrusters::horizontal_front_left]  = th_horizontal_front_left_pin;
    pwm.m_pins[thrusters::horizontal_front_right] = th_horizontal_front_right_pin;
    pwm.m_pins[thrusters::horizontal_back_left]   = th_horizontal_back_left_pin;
    pwm.m_pins[thrusters::horizontal_back_right]  = th_horizontal_back_right_pin;
    pwm.m_pins[thrusters::vertical_front]         = th_vertical_front_pin;
    pwm.m_pins[thrusters::vertical_back]          = th_vertical_back_pin;
    pwm.m_pins[thrusters::custom_0]               = th_custom_0_pin;
    pwm.m_pins[thrusters::custom_1]               = th_custom_1_pin;

    for (int i = 0; i < 8; i++) {
        pwm.m_isr_servos[i] = SAMD_ISR_Servos.setupServo(pwm.m_pins[i], pulse_min, pulse_max);
    }

    SAMD_ISR_Servos.setReadyToRun();

    for (int i = 0; i < 8; i++) {
        SAMD_ISR_Servos.setPulseWidth(pwm.m_isr_servos[i], pulse_med);
    }

    /* Arduino Servos init */

    pwm.m_servos[config::servos::front].attach(servo_front_pin);
    pwm.m_servos[config::servos::back].attach(servo_back_pin);
    pwm.m_servos[config::servos::pwm_a2].attach(pwm_custom_a2_pin);
    pwm.m_servos[config::servos::pwm_a3].attach(pwm_custom_a3_pin);

    pwm.m_servos[config::servos::front].writeMicroseconds(pulse_med);
    pwm.m_servos[config::servos::back].writeMicroseconds(pulse_med);
    pwm.m_servos[config::servos::pwm_a2].writeMicroseconds(pulse_med);
    pwm.m_servos[config::servos::pwm_a3].writeMicroseconds(pulse_med);

    /* I2C PCA init */

    if (pwm.m_pca.begin() == false) {
        SerialUSB.println("ERROR: PCA9536 not detected! Check I2C connection.");
    }

    for (int i = 0; i < 4; i++) {
        pwm.m_pca.pinMode(i, OUTPUT);
        pwm.m_pca.write(i, LOW);
    }

    pinMode(config::pwm::cam_select_pin, OUTPUT);

    delay(10);
}

void PWMController::set_servo_angle(int idx, int angle) {
    int pulse = map(angle, 0, 180, 1010, 1990);
    pulse = constrain(pulse, 1010, 1990);
    inst().m_servos[idx].writeMicroseconds(pulse);
}

void PWMController::set_servo_power(int idx, int power) {
    int pulse = map(power, -100, 100, 1010, 1990);
    pulse = constrain(pulse, 1010, 1990);
    inst().m_servos[idx].writeMicroseconds(pulse);
}

void PWMController::set_thruster(int idx, int power) {
    int pulse = map(power, -100, 100, 1010, 1990);
    pulse = constrain(pulse, 1010, 1990);
    SAMD_ISR_Servos.setPulseWidth(inst().m_isr_servos[idx], pulse);
}

void PWMController::set_manipulator(int ch, int power) {
    int state = LOW;

    if (abs(power) >= 50) {
        state = HIGH;
    }

    inst().m_pca.write(ch, state);
}

void PWMController::set_video_multiplexer(bool state) {
    digitalWrite(config::pwm::cam_select_pin, state);
}

PWMController & PWMController::inst() {
    static PWMController pwm;
    return pwm;
}

PWMController::PWMController() {

}

