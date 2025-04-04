#include "RovCameraWidget.hpp"
#include <QMediaContent>
#include <QNetworkRequest>
#include <QUrlQuery>
#include <QNetworkConfigurationManager>
#include <QDebug>

RovCameraWidget::RovCameraWidget(QWidget* parent)
    : QStackedWidget(parent)
    , m_player(new QMediaPlayer(this, QMediaPlayer::VideoSurface))
    , m_videoWidget(new QVideoWidget(this))
{
    m_player->setProperty("video", true);
    m_player->setProperty("priority", 0);
    m_player->setProperty("fast", true);
    m_player->setProperty("low-delay", true);
    
    QNetworkConfigurationManager manager;
    auto configs = manager.allConfigurations(QNetworkConfiguration::Active);
    if(!configs.empty()) {
        m_player->setNetworkConfigurations(configs);
    }

    setupVideoOutput();
    startCapture();
}

void RovCameraWidget::setupVideoOutput()
{
    m_videoWidget->setAspectRatioMode(Qt::IgnoreAspectRatio);
    m_videoWidget->setAttribute(Qt::WA_OpaquePaintEvent);
    m_videoWidget->setAttribute(Qt::WA_NoSystemBackground);
    m_player->setVideoOutput(m_videoWidget.data());
    
    QUrl url("rtsp://192.168.1.6/stream=0");
    QUrlQuery query;
    query.addQueryItem("udp_port", "5006");
    query.addQueryItem("buffer_size", "65535");
    query.addQueryItem("tcp_nodelay", "1");
    query.addQueryItem("rtsp_transport", "udp");
    url.setQuery(query);
    
    QNetworkRequest request(url);
    request.setAttribute(QNetworkRequest::Http2AllowedAttribute, false);
    request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork);
    request.setAttribute(QNetworkRequest::CacheSaveControlAttribute, false);
    request.setAttribute(QNetworkRequest::DoNotBufferUploadDataAttribute, true);
    
    QMediaContent media(request.url());
    m_player->setMedia(media);
    
    connect(m_player.data(), QOverload<QMediaPlayer::Error>::of(&QMediaPlayer::error),
            this, [this](QMediaPlayer::Error error) {
                qDebug() << "MediaPlayer error:" << error << m_player->errorString();
            });
    
    addWidget(m_videoWidget.data());
}

void RovCameraWidget::startCapture()
{
    if (!m_player) {
        m_player.reset(new QMediaPlayer(this, QMediaPlayer::VideoSurface));
        setupVideoOutput();
    }
    
    m_player->setPlaybackRate(1.0);
    m_player->play();
}

void RovCameraWidget::stopCapture()
{
    if (m_player) {
        m_player->disconnect();
        m_player->stop();
    }
}

void RovCameraWidget::cleanupPlayer()
{
    stopCapture();
}

RovCameraWidget::~RovCameraWidget()
{
    cleanupPlayer();
}