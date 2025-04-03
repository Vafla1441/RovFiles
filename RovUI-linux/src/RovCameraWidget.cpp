#include "RovCameraWidget.hpp"

RovCameraWidget::RovCameraWidget(QWidget* parent)
    : QStackedWidget(parent)
    , m_player(new QMediaPlayer(this))
    , m_videoWidget(new QVideoWidget(this))
{
    setupVideoOutput();
    startCapture();
}

RovCameraWidget::~RovCameraWidget()
{
    cleanupPlayer();
}

void RovCameraWidget::setupVideoOutput()
{
    m_player->setVideoOutput(m_videoWidget.data());
    m_player->setMedia(QUrl("rtsp://192.168.1.6/stream=0"));
    addWidget(m_videoWidget.data());
}

void RovCameraWidget::startCapture()
{
    if (!m_player) {
        m_player.reset(new QMediaPlayer(this));
    }
    m_player->play();
}

void RovCameraWidget::stopCapture()
{
    if (m_player) {
        m_player->stop();
    }
}

void RovCameraWidget::cleanupPlayer()
{
    if (m_player) {
        m_player->stop();
    }
}