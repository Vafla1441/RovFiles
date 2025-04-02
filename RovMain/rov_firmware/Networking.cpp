#include "Networking.h"
#include "Config.h"

void Networking::init() {
    Ethernet.init(config::networking::cs_pin);
    Ethernet.begin(inst().mac, inst().m_self_ip);
    inst().Udp.begin(inst().m_self_port);
}

void Networking::read_write_udp(rov::RovTelimetry &tel, rov::RovControl &ctrl) {
    static unsigned long lastTimestamp = 0;
    static constexpr size_t buffer_size = 128;

    uint8_t buffer[buffer_size];
    int size = inst().read(buffer, buffer_size);

    if (size > 0) {
        auto error = ctrl.fromRovControlMsg(buffer, size);

        if (error != rov::RovControl::RovControlErrorCode::NoError) {
            SerialUSB.println(ctrl.fromErrorToString(error));
        }

        if (ctrl.version == 2) {
            size = tel.toRovTelemetryMsgV2(buffer);
        } else {
            size = tel.toRovTelemetryMsgV1(buffer);
        }

        inst().write(buffer, size);

        unsigned long curTimestamp = millis();
        if (abs(curTimestamp - lastTimestamp) > 1000) {
            lastTimestamp = curTimestamp;

            size = rov::RovHello::toRovHelloMsg(buffer);
            inst().write(buffer, size);
        }
    }
}


Networking & Networking::inst() {
    static Networking net;
    return net;
}

int Networking::read(uint8_t * buffer, int size) {
    int packetSize = Udp.parsePacket();

    if (packetSize) {
        Udp.read(buffer, size);
        return packetSize;
    }
    return 0;
}

void Networking::write(uint8_t * buffer, int size) {
    Udp.beginPacket(m_remote_ip, m_remote_port);
    Udp.write(buffer, size);
    Udp.endPacket();
}

Networking::Networking() {
    auto &sp = config::networking::selfip;
    m_self_ip = IPAddress(sp[0], sp[1], sp[2], sp[3]);
    m_self_port = config::networking::self_port;

    auto &rp = config::networking::remoteip;
    m_remote_ip = IPAddress(rp[0], rp[1], rp[2], rp[3]);
    m_remote_port = config::networking::remote_port;
}


