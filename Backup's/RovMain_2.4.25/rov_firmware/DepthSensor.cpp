#include "Arduino.h"
#include "DepthSensor.h"
#include "Wire.h"

bool DepthSensor::init() {
    auto &ds = inst();
    bool ok = ds.sensor.init();

    if (!ok) {
        SerialUSB.println("Depth sensor init failed!");
        return false;
    }

    ds.sensor.setModel(MS5837::MS5837_30BA);
    ds.sensor.setFluidDensity(997); // kg/m^3 (freshwater, 1029 for seawater)
    return true;
}

bool DepthSensor::getUpdateStatus() {
    auto &ds = inst();
    return ds.m_update_status;
}

bool DepthSensor::update() {
    auto &ds = inst();
    bool status = false; // false = error, true = ok

    if ((millis() - ds.time_update) > 200) {
        ds.sensor.read();
        ds.time_update = millis();

        if (abs(ds.sensor.depth()) < 150) {
            status = true;
            ds.m_depth = ds.sensor.depth();
            ds.m_temp = ds.sensor.temperature();
        } else { // don't accept value if value is too big (probably corrupted data)
            // SerialUSB.println("\n\nDepth sensor read error.\n\n");
            status = false;

            // trying to reset I2C and init sensor again.
            Wire.end();
            Wire.begin();
            Wire.setTimeout(1000);
            Wire.setClock(10000);
            init();
        }

        ds.m_update_status = status;
    }

    return status;
}

float DepthSensor::getDepth() {
    update();

    auto &ds = inst();
    return ds.m_depth;
}

float DepthSensor::getTemp() {
    update();

    auto &ds = inst();
    return ds.m_temp;
}


DepthSensor & DepthSensor::inst() {
    static DepthSensor ds;
    return ds;
}
