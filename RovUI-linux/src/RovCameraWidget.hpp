#pragma once

#include <QVideoWidget>
#include <QStackedWidget>
#include <QMediaPlayer>
#include <QMediaPlaylist>

class RovCameraWidget : public QStackedWidget {
    Q_OBJECT
public:
    explicit RovCameraWidget(QWidget* parent = nullptr);
    ~RovCameraWidget();

public slots:
    void startCapture();
    void stopCapture();

private:
    void setupVideoOutput();
    void cleanupPlayer();

    QScopedPointer<QMediaPlayer> m_player;
    QScopedPointer<QVideoWidget> m_videoWidget;
};