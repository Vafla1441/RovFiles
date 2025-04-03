#include "DebugDialog.hpp"
#include "RovSingleton.hpp"
#include <QVBoxLayout>

DebugDialog::DebugDialog(QWidget* parent)
    : QDialog(parent)
    , m_isDebug(new QCheckBox("Отладка", this))
{
    setMinimumSize(240, 240);

    for (int i = 0; i < m_thurstersCount; i++) {
        m_thrusterSliders.append({
                    QSharedPointer<QSlider>(new QSlider(Qt::Horizontal, this)),
                    QSharedPointer<QLabel>(new QLabel(sliderTemplate.arg(i).arg(0), this))});

        m_thrusterSliders[i].first.data()->setEnabled(false);
        m_thrusterSliders[i].first.data()->setValue(0);
        m_thrusterSliders[i].first.data()->setRange(-100, 100);
    }

    startTimer(90);

    createLayout();
    createConnections();
}

void DebugDialog::timerEvent(QTimerEvent* event)
{
    Q_UNUSED(event)

    if (!m_isDebug.data()->isChecked()) {
        return;
    }
    RovControl& rc = RovSingleton::instance()->control();

    for (int i = 0; i < m_thurstersCount; i++) {
        rc.thrusterPower[i] = m_thrusterSliders[i].first.data()->value();
        m_thrusterSliders[i].second.data()->setText(sliderTemplate.arg(i).arg(rc.thrusterPower[i]));
    }
}

void DebugDialog::createConnections()
{
    QObject::connect(m_isDebug.data(), &QCheckBox::stateChanged, [this](int index) {
        Q_UNUSED(index)
        if (m_isDebug.data()->isChecked()) {
            RovSingleton::instance()->control().debugFlag = true;
            for (auto &slider: m_thrusterSliders) {
                slider.first.data()->setEnabled(true);
            }
        } else {
            RovSingleton::instance()->control().debugFlag = false;
            for (auto &slider: m_thrusterSliders) {
                slider.first.data()->setEnabled(false);
            }
        }
    });
}

void DebugDialog::createLayout()
{
    QVBoxLayout* layout = new QVBoxLayout;

    auto addToLayout = [](QSlider* slider, QLabel* label, QVBoxLayout* layout) {
        QVBoxLayout* hLayout = new QVBoxLayout;
        hLayout->addWidget(label);
        hLayout->addWidget(slider);
        layout->addLayout(hLayout);
    };

    for (auto &slider: m_thrusterSliders) {
        addToLayout(slider.first.data(), slider.second.data(), layout);
    }

    layout->addWidget(m_isDebug.data());

    setLayout(layout);
}
