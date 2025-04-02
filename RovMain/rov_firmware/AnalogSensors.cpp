#include "AnalogSensors.h"
#include "Config.h"
#include "Arduino.h"

void AnalogSensors::init() {
    using namespace config::analog;
    auto &sensors = inst();

    pinMode(voltmeter_pin, INPUT);
    pinMode(ammeter_pin, INPUT);
}

void AnalogSensors::update() {
    using namespace config::analog;
    auto &sensors = inst();

    int rawVoltage = analogRead(voltmeter_pin);
    int rawAmperage = analogRead(ammeter_pin);

    float curVoltage = round(rawVoltage * (voltage_multiplier * 100.0)) / 100.0;
    float curAmperage = round((rawAmperage - amperage_deflection) * (amperage_multiplier * 100.0)) / 100.0;

    sensors.m_voltage_samples[sensors.m_counter] = curVoltage;
    sensors.m_amperage_samples[sensors.m_counter] = curAmperage;

    float resultVoltage = 0.0;
    float resultAmperage = 0.0;

    for (int i = 0; i < ANALOG_SAMPLES_COUNT; i++) {
        resultVoltage += sensors.m_voltage_samples[i];
        resultAmperage += sensors.m_amperage_samples[i];
    }

    resultVoltage /= ANALOG_SAMPLES_COUNT;
    resultAmperage /= ANALOG_SAMPLES_COUNT;

    sensors.m_voltage = resultVoltage;
    sensors.m_amperage = resultAmperage;

    sensors.m_counter++;
    if (sensors.m_counter >= ANALOG_SAMPLES_COUNT) {
        sensors.m_counter = 0;
    }
}

double AnalogSensors::getVoltage() {
    return inst().m_voltage;
}

double AnalogSensors::getAmperage() {
    return inst().m_amperage;
}

AnalogSensors & AnalogSensors::inst() {
    static AnalogSensors controller;
    return controller;
}

AnalogSensors::AnalogSensors() {

}
