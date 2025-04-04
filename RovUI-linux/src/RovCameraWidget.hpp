#pragma once

#include <QVideoWidget>
#include <QStackedWidget>
#include <QMediaPlayer>
#include <QCameraViewfinder>

class RovCameraWidget : public QStackedWidget {
    Q_OBJECT
public:
    explicit RovCameraWidget(QWidget* parent = nullptr);
    ~RovCameraWidget();

public slots:
    void startCapture();
    void stopCapture();

private:
    void cleanupPlayer();
    void setupVideoOutput();

    QScopedPointer<QMediaPlayer> m_player;
    QScopedPointer<QVideoWidget> m_videoWidget;
    const QUrl m_streamUrl{"rtsp://192.168.1.6/stream=0"};
};