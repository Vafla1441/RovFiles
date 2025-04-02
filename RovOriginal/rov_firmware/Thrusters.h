#pragma once

#include "Data.h"
class Thrusters
{
public:
    static void init();
    static void update_thrusters(rov::RovControl & ctrl, rov::RovTelimetry &tel);

private:
    static Thrusters &inst();
    int directions[6] = { 1,1,1,1,1,1 };
};

