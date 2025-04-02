#pragma once
class Manipulator
{
public:
    static void init();
    static void set_power(int rotate, int grab);
private:
    static void set(int ch1, int ch2, int power);
};

