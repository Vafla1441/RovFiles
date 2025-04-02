#include "RotaryCameras.h"
#include "Config.h"
#include "PWMController.h"

void RotaryCameras::init() {
    auto &data = inst();
    PWMController::set_servo_angle(config::servos::front, data.angles[0]);
    PWMController::set_servo_angle(config::servos::back, data.angles[1]);
}

void RotaryCameras::set_angle(int idx, int angle) {
    int servo_idx = (idx == 0) ? config::servos::front : config::servos::back;
    auto &data = inst();

    if ((millis() - data.times_update[idx]) > 17) {
        data.times_update[idx] = millis();
        // data.angles[idx] += angle;
        data.angles[idx] = constrain(data.angles[idx] + angle, 0, 180);
        PWMController::set_servo_angle(servo_idx, data.angles[servo_idx]);
    }
}

void RotaryCameras::select_cam(bool state) {
    auto &data = inst();

    data.cam_switch = state;
    PWMController::set_video_multiplexer(state);
}

int RotaryCameras::get_cam_index() {
    return inst().cam_switch ? 1 : 0;
}

RotaryCameras & RotaryCameras::inst() {
    static RotaryCameras rc;
    return rc;
}
