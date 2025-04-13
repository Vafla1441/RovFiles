#include "RegulatorsWidget.hpp"
#include "RovSingleton.hpp"

#include <QVBoxLayout>
#include <bitset>

RegulatorsWidget::RegulatorsWidget(QWidget* parent)
    : QWidget(parent)
    , m_depthRegulator(new QCheckBox("Глубина", this))
{
    createLayout();
    createConnections();
}

void RegulatorsWidget::createConnections()
{
    QObject::connect(m_depthRegulator.data(), &QCheckBox::stateChanged, [this](int index) {
        Q_UNUSED(index)
        std::bitset<8> regulator;
        regulator[0] = m_depthRegulator.data()->isChecked();
        RovSingleton::instance()->control().regulators = static_cast<quint8>(regulator.to_ulong());
    });
}

void RegulatorsWidget::createLayout()
{
    QHBoxLayout* layout = new QHBoxLayout;
    layout->addWidget(m_depthRegulator.data());
    setLayout(layout);
}
