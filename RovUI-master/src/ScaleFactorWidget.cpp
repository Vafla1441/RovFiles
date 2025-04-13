#include "ScaleFactorWidget.hpp"
#include "RovSingleton.hpp"
#include <QGroupBox>
#include <QHBoxLayout>
ScaleFactorWidget::ScaleFactorWidget(QWidget* parent)
    : QWidget(parent)
    , m_fiftyButton(new QRadioButton("50", this))
    , m_twentyFive(new QRadioButton("25", this))
    , m_oneHundred(new QRadioButton("100", this))
{
    createLayout();
    createConnections();
}

void ScaleFactorWidget::createLayout()
{
    QHBoxLayout* layout = new QHBoxLayout;
    layout->addWidget(m_twentyFive.data());
    layout->addWidget(m_fiftyButton.data());
    layout->addWidget(m_oneHundred.data());

    m_oneHundred->setChecked(true);
    setLayout(layout);
}

void ScaleFactorWidget::createConnections()
{
    QObject::connect(m_fiftyButton.data(), &QRadioButton::clicked, [this](bool isCheced) {
        if (isCheced) {
            RovSingleton::instance()->setThrustScaleFactor(0.5f);
            qInfo() << "Установленио ограничение в 50% тяги";
        }
    });

    QObject::connect(m_twentyFive.data(), &QRadioButton::clicked, [this](bool isCheced) {
        if (isCheced) {
            RovSingleton::instance()->setThrustScaleFactor(0.25f);
            qInfo() << "Установленио ограничение в 25% тяги";
        }
    });

    QObject::connect(m_oneHundred.data(), &QRadioButton::clicked, [this](bool isCheced) {
        if (isCheced) {
            RovSingleton::instance()->setThrustScaleFactor(1.0f);
            qInfo() << "Установленио ограничение в 100% тяги";
        }
    });
}
