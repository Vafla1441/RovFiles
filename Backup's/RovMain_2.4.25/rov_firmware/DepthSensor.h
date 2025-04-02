#pragma once

#include <MS5837.h>
class DepthSensor
{
public:
    static bool init();
    static float getDepth();
    static float getTemp();
    static bool getUpdateStatus(); // false = error, true = ok
private:
    static DepthSensor &inst();
    static bool update();

    MS5837 sensor;
    long long time_update = 0;
    float m_depth = 0.0;
    float m_temp = 0.0;
    bool m_update_status;
};

