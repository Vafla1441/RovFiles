#pragma once

#include <QLabel>
#include <QWidget>

class RovTelimetryWidget : public QWidget {
    Q_OBJECT
public:
    explicit RovTelimetryWidget(QWidget* parent = nullptr);

signals:

public slots:
private:
    void createConnections();

    void createLayout();

    void onTelimetryUpdate();

    QScopedPointer<QLabel> m_pitch;
    QScopedPointer<QLabel> m_roll;
    QScopedPointer<QLabel> m_yaw;
    QScopedPointer<QLabel> m_depth;
    QScopedPointer<QLabel> m_temperature;
    QScopedPointer<QLabel> m_regulatorsFeedback;
    QScopedPointer<QLabel> m_ammeter;
    QScopedPointer<QLabel> m_voltmeter;
};
