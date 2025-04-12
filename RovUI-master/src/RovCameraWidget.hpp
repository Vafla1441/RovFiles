#pragma once

#include <QCamera>
#include <QCameraViewfinder>
#include <QComboBox>
#include <QPushButton>
#include <QStackedWidget>
#include <QWidget>

class RovCameraWidget : public QStackedWidget {
    Q_OBJECT
public:
    explicit RovCameraWidget(QWidget* parent = nullptr);

signals:

public slots:
    void startCapture();

    void stopCapture();

    QCamera::Status cameraStatus();

private:
    enum WidgetType {
        CameraSelectionWidget = 0,
        CameraViewWidget = 1
    };

    void createCameraSelectionWidget();

    void createCameraViewWidget();

    void createConnections();

    QScopedPointer<QCameraViewfinder> m_cameraView;
    QScopedPointer<QCamera> m_camera;
    QScopedPointer<QComboBox> m_cameraList;
    QScopedPointer<QPushButton> m_refreshButton;
};
