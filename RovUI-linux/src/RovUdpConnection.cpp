#include "RovUdpConnection.hpp"

RovUdpConnection::RovUdpConnection(QObject* parent)
    : QObject(parent)
    , m_rovIp("192.168.1.5")
    , m_socket(new QUdpSocket(this))

{
    connect(m_socket.data(), &QUdpSocket::readyRead, this,
        &RovUdpConnection::onDatareceived);
    m_socket->bind(m_port);
}

RovUdpConnection::~RovUdpConnection() {}

void RovUdpConnection::transmitDatagram(QByteArray datagram)
{
    m_socket->writeDatagram(datagram, m_rovIp, m_rovPort);
}

void RovUdpConnection::onDatareceived()
{
    QByteArray datagram;
    do {
        datagram.resize(m_socket->pendingDatagramSize());
        m_socket->readDatagram(datagram.data(), datagram.size());
    } while (m_socket->hasPendingDatagrams());

    emit dataReceived(datagram);
}
