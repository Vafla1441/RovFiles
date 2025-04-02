#pragma once

#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet2.h>
#include <EthernetUdp2.h>         // UDP library from: bjoern@cs.stanford.edu 12/30/2008
#include "Data.h"

class Networking
{
public:
    Networking();

    static void init();
    static void read_write_udp(rov::RovTelimetry &tel, rov::RovControl &ctrl);
private:
    static Networking &inst();
    int read(uint8_t *buffer, int size);
    void write(uint8_t *buffer, int size);


    byte mac[6] = {
        0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
    };
    EthernetUDP Udp;
   

    IPAddress m_self_ip;
    uint16_t m_self_port; 
    IPAddress m_remote_ip;
    uint16_t m_remote_port;
};

