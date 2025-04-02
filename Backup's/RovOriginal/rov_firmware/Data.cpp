
#include "Data.h"

namespace rov {

} // namespace rov

void rov::prvt::swap_endian_for(RovControl & rc) {
    rc.axisX = swap_endian(rc.axisX);
    rc.axisY = swap_endian(rc.axisY);
    rc.axisZ = swap_endian(rc.axisZ);
    rc.axisW = swap_endian(rc.axisW);

    rc.cameraRotation[0] = swap_endian(rc.cameraRotation[0]);
    rc.cameraRotation[1] = swap_endian(rc.cameraRotation[1]);

    rc.thrusterPower[0] = swap_endian(rc.thrusterPower[0]);
    rc.thrusterPower[1] = swap_endian(rc.thrusterPower[1]);
    rc.thrusterPower[2] = swap_endian(rc.thrusterPower[2]);
    rc.thrusterPower[3] = swap_endian(rc.thrusterPower[3]);
    rc.thrusterPower[4] = swap_endian(rc.thrusterPower[4]);
    rc.thrusterPower[5] = swap_endian(rc.thrusterPower[5]);
    rc.thrusterPower[6] = swap_endian(rc.thrusterPower[6]);
    rc.thrusterPower[7] = swap_endian(rc.thrusterPower[7]);

    rc.debugFlag = swap_endian(rc.debugFlag);

    rc.desiredDepth = swap_endian(rc.desiredDepth);
    rc.manipulatorOpenClose = swap_endian(rc.manipulatorOpenClose);
    rc.manipulatorRotation = swap_endian(rc.manipulatorRotation);
    rc.regulators = swap_endian(rc.regulators);

    rc.cameraIndex = swap_endian(rc.cameraIndex);
}

void rov::prvt::swap_endian_for(RovTelimetry & rt) {
    rt.yaw = swap_endian(rt.yaw);
    rt.pitch = swap_endian(rt.pitch);
    rt.roll = swap_endian(rt.roll);

    rt.ammeter = swap_endian(rt.ammeter);
    rt.voltmeter = swap_endian(rt.voltmeter);

    rt.manipulatorAngle = swap_endian(rt.manipulatorAngle);
    rt.manipulatorState = swap_endian(rt.manipulatorState);
    rt.regulatorsFeedback = swap_endian(rt.regulatorsFeedback);
    rt.depth = swap_endian(rt.depth);

    rt.cameraIndex = swap_endian(rt.cameraIndex);
    rt.temperature = swap_endian(rt.temperature);
}

void rov::prvt::swap_endian_for(RovHello & rh) {
    rh.header_hello = swap_endian(rh.header_hello);
    rh.version = swap_endian(rh.version);
}