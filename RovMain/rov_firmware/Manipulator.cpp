#include "Manipulator.h"
#include "Config.h"
#include "PWMController.h"

void Manipulator::init() {
    using namespace config::pwm;
    PWMController::set_manipulator(manip_relese_ch, 0);
    PWMController::set_manipulator(manip_grab_ch, 0);
    PWMController::set_manipulator(manip_right_ch, 0);
    PWMController::set_manipulator(manip_left_ch, 0);
}

void Manipulator::set_power(int rotate, int grab) {
    using namespace config::pwm;
    int maxSpeed = 100;
    set(manip_relese_ch, manip_grab_ch, constrain(grab * 100, -maxSpeed, maxSpeed));
    set(manip_left_ch, manip_right_ch, constrain(rotate * 100, -maxSpeed, maxSpeed));
}

void Manipulator::set(int ch1, int ch2, int power) {
    if (power == 0) {
        PWMController::set_manipulator(ch1, 0);
        PWMController::set_manipulator(ch2, 0);
        return;
    }
    if (power > 0) {
        PWMController::set_manipulator(ch1, power);
        PWMController::set_manipulator(ch2, 0);
        return;
    }
    if (power < 0) {
        PWMController::set_manipulator(ch1, 0);
        PWMController::set_manipulator(ch2, power);
        return;
    }

}
