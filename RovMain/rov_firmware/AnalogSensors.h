#pragma once

#define ANALOG_SAMPLES_COUNT 50

class AnalogSensors {
public:
    static void init();
    static void update();

    static double getVoltage();
    static double getAmperage();

    AnalogSensors(const AnalogSensors&) = delete;

private:
    AnalogSensors();
    static AnalogSensors &inst();

    double m_voltage = 0.0;
    double m_amperage = 0.0;

    double m_voltage_samples[ANALOG_SAMPLES_COUNT] = { 0.0 };
    double m_amperage_samples[ANALOG_SAMPLES_COUNT] = { 0.0 };

    int m_counter = 0;
};
