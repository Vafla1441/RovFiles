#include "RovTelimetryWidget.hpp"
#include "RovSingleton.hpp"

#include <QVBoxLayout>

RovTelimetryWidget::RovTelimetryWidget(QWidget* parent)
    : QWidget(parent)
    , m_pitch(new QLabel("Крен: -", this))
    , m_roll(new QLabel("Диффирент: -", this))
    , m_yaw(new QLabel("Курс: -", this))
    , m_depth(new QLabel("Глубина: -", this))
    , m_temperature(new QLabel("Температура: -", this))
    , m_regulatorsFeedback(new QLabel("Регуляторы: -", this))
    , m_ammeter(new QLabel("Ток: - ", this))
    , m_voltmeter(new QLabel("Напряжение: -", this))
{
    createConnections();
    createLayout();
}

void RovTelimetryWidget::createConnections()
{
    connect(RovSingleton::instance(), &RovSingleton::telimetryUpdated, this,
        &RovTelimetryWidget::onTelimetryUpdate);
}

void RovTelimetryWidget::createLayout()
{
    QVBoxLayout* layout = new QVBoxLayout;
    QWidget* spacer = new QWidget;
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    layout->addWidget(m_pitch.data());
    layout->addWidget(m_roll.data());
    layout->addWidget(m_yaw.data());
    layout->addWidget(m_depth.data());
    layout->addWidget(m_temperature.data());
    layout->addWidget(m_regulatorsFeedback.data());
    layout->addWidget(m_ammeter.data());
    layout->addWidget(m_voltmeter.data());

    QFont monospace("Consolas");
    m_pitch.data()->setFont(monospace);
    m_yaw.data()->setFont(monospace);
    m_roll.data()->setFont(monospace);
    m_regulatorsFeedback.data()->setFont(monospace);
    m_depth.data()->setFont(monospace);
    m_temperature.data()->setFont(monospace);
    m_ammeter.data()->setFont(monospace);
    m_voltmeter.data()->setFont(monospace);

    layout->addWidget(spacer);

    setLayout(layout);
}

void RovTelimetryWidget::onTelimetryUpdate()
{
    RovTelimetry& rt = RovSingleton::instance()->telimetry();

    m_pitch.data()->setText(QString(tr("Крен: %1").arg(QString::number(rt.pitch, 'f', 2))));

    m_yaw.data()->setText(QString(tr("Курс: %1").arg(QString::number(rt.yaw, 'f', 2))));

    m_roll.data()->setText(QString(tr("Диффирент: %1").arg(QString::number(rt.roll, 'f', 2))));

    m_regulatorsFeedback.data()->setText(
        QString(tr("Регуляторы: %1").arg(rt.regulatorsFeedback)));

    m_depth.data()->setText(QString(tr("Глубина: %1").arg(QString::number(rt.depth, 'f', 2))));

    m_temperature.data()->setText(QString(tr("Температура: %1").arg(QString::number(rt.temperature, 'f', 2))));

    m_ammeter.data()->setText(QString(tr("Ток: %1 А").arg(QString::number(rt.ammeter, 'f', 2))));

    m_voltmeter.data()->setText(QString(tr("Напряжение: %1 В").arg(QString::number(rt.voltmeter, 'f', 2))));
}
