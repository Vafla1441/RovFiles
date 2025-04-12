#include "RovSingleton.hpp"

#include <QDebug>
#include <QTimer>

RovSingleton::~RovSingleton() {}

RovSingleton* RovSingleton::instance()
{
    return Singleton<RovSingleton>::instance(RovSingleton::createInstance);
}

RovTelimetry& RovSingleton::telimetry() { return m_telimetry; }

RovControl& RovSingleton::control() { return m_controlData; }

void RovSingleton::setTransmit(bool transmit) { m_isTransmit = transmit; }

void RovSingleton::setTransmitTime(int time)
{
    m_transmitTimer->start(time);
}

void RovSingleton::setThrustScaleFactor(float scale)
{
    m_scaleFactor = scale;
}

RovSingleton::RovSingleton(QObject* parent)
    : m_transmitTimer(new QTimer(this))
    , m_udpConnection(new RovUdpConnection(this))

{
    Q_UNUSED(parent)
    m_transmitTimer->start(100);
    createConnections();
}

RovSingleton* RovSingleton::createInstance()
{
    return new RovSingleton();
}

void RovSingleton::createConnections()
{
    QObject::connect(m_udpConnection.data(), &RovUdpConnection::dataReceived,
        [this](QByteArray datagram) {
            if (datagram.size() == 4 && quint8(datagram[0]) == RovHello::header_hello) {
                m_telimetry.version = RovHello::getVersion(datagram);
                return;
            }

            RovTelimetry::RovTelimetryErrorCode ec = RovTelimetry::RovTelimetryErrorCode::WrongDataSize; // TODO?

            if (datagram.size() == 29) { // probably v1 telemetry msg
                ec = m_telimetry.fromRangerTelimetryMsgV1(datagram);
            } else if (quint8(datagram[0]) == RovTelimetry::header_telemetry) {
                ec = m_telimetry.fromRangerTelimetryMsgV2(datagram);
            }

            if (ec != RovTelimetry::RovTelimetryErrorCode::NoError) {
                qInfo() << RovTelimetry::fromErrorToString(ec);
                return;
            }

            emit telimetryUpdated();
        });

    QObject::connect(m_transmitTimer.data(), &QTimer::timeout, [this]() {
        if (m_isTransmit) {
            m_controlData.axisW *= m_scaleFactor;
            m_controlData.axisX *= m_scaleFactor;
            m_controlData.axisZ *= m_scaleFactor;
            m_controlData.axisY *= m_scaleFactor;

            if (m_telimetry.version == 2) {
                auto data = m_controlData.toRangerControlMsgV2();
                m_udpConnection->transmitDatagram(data);
            } else {
                m_udpConnection->transmitDatagram(m_controlData.toRangerControlMsgV1());
            }
        }
    });
}
