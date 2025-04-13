#pragma once
#include <QCheckBox>
#include <QDialog>
#include <QLabel>
#include <QSlider>
#include <QWidget>

class DebugDialog : public QDialog {
    Q_OBJECT
public:
    explicit DebugDialog(QWidget* parent = nullptr);

signals:
private slots:
    void timerEvent(QTimerEvent* event);
public slots:
private:
    void createConnections();
    void createLayout();

    QScopedPointer<QCheckBox> m_isDebug;

    static constexpr int m_thurstersCount = 8;
    const QString sliderTemplate = tr("Движитель %1: %2");
    QList<QPair<QSharedPointer<QSlider>, QSharedPointer<QLabel>>> m_thrusterSliders;

};
