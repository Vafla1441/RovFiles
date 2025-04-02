#pragma once
class RotaryCameras
{
public:
    static void init();
    static void set_angle(int idx, int angle);
    static void select_cam(bool state);
    static int get_cam_index();
private:
    static RotaryCameras &inst();
    int angles[2] = { 90, 90 };
    long long times_update[2] = { 0, 0 };
    bool cam_switch = false;
};

