#pragma once

#include "RovDataTypes.hpp"

#include <QDateTime>
#include <QLabel>
#include <QMap>
#include <QPushButton>
#include <QWidget>

class LogWidget : public QWidget {
    Q_OBJECT
public:
    explicit LogWidget(QWidget* parent = nullptr);

signals:

public slots:
private:
    void createConnections();
    void createLayout();

    QVector<QPair<QDateTime, RovTelimetry>> m_logData;
    QScopedPointer<QPushButton> m_startStopButton;
    QScopedPointer<QPushButton> m_saveLogButton;
    QScopedPointer<QLabel> m_logRecordsCount;

    bool m_isStarted = false;
};
