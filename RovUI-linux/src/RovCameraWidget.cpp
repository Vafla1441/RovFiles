#include "RovCameraWidget.hpp"
#include <QNetworkRequest>
#include <QNetworkConfigurationManager>
#include <QDebug>

RovCameraWidget::RovCameraWidget(QWidget* parent)
    : QStackedWidget(parent)
    , m_player(new QMediaPlayer(this, QMediaPlayer::StreamPlayback))
    , m_videoWidget(new QVideoWidget(this))
{
    qputenv("QT_DEBUG_PLUGINS", "1");
    setupVideoOutput();
    startCapture();
}

RovCameraWidget::~RovCameraWidget()
{
    cleanupPlayer();
}

void RovCameraWidget::setupVideoOutput()
{
    m_videoWidget->setAspectRatioMode(Qt::KeepAspectRatio);
    m_player->setVideoOutput(m_videoWidget.data());
    
    QNetworkRequest request(m_streamUrl);
    request.setRawHeader("User-Agent", "RovCameraWidget");
    request.setRawHeader("Connection", "Keep-Alive");
    request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork);
    
    m_player->setMedia(request);
    addWidget(m_videoWidget.data());
    
    connect(m_player.data(), QOverload<QMediaPlayer::Error>::of(&QMediaPlayer::error),
            [this](QMediaPlayer::Error error) {
                qDebug() << "Media player error:" << error << m_player->errorString();
            });
}

void RovCameraWidget::startCapture()
{
    if (!m_player) {
        m_player.reset(new QMediaPlayer(this, QMediaPlayer::StreamPlayback));
        setupVideoOutput();
    }
    
    qputenv("GST_DEBUG", "2");
    qputenv("GST_BUFFER_SIZE", "2000000");
    
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
    stopCapture();
}