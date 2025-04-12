#include "LogWidget.hpp"
#include "RovSingleton.hpp"
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QVBoxLayout>
#include <numeric>

LogWidget::LogWidget(QWidget* parent)
    : QWidget(parent)
    , m_startStopButton(new QPushButton(tr("Начать запись"), this))
    , m_saveLogButton(new QPushButton(tr("Сохранить лог"), this))
    , m_logRecordsCount(new QLabel(tr("Количество записей: 0"), this))
{
    createLayout();
    createConnections();
    m_logData.reserve(1024);
}

void LogWidget::createConnections()
{
    QObject::connect(m_startStopButton.data(), &QPushButton::clicked, [this]() {
        if (m_isStarted) {
            m_isStarted = false;
            m_startStopButton.data()->setText(tr("Начать запись"));
            qInfo() << "Логирование остановлено!";

        } else {
            m_isStarted = true;
            m_startStopButton.data()->setText(tr("Остановить запись"));
            qInfo() << "Логирование начато!";
        }
    });

    QObject::connect(RovSingleton::instance(), &RovSingleton::telimetryUpdated, [this]() {
        if (!m_isStarted) {
            return;
        }

        if (m_logData.size() == std::numeric_limits<int>::max()) {
            qWarning() << "Максимальный размер лога достигнут!";
            return;
        }
        m_logData.push_back({ QDateTime::currentDateTime(), RovSingleton::instance()->telimetry() });
        m_logRecordsCount.data()->setText(QString("Количество записей: %1").arg(m_logData.size()));
    });

    QObject::connect(m_saveLogButton.data(), &QPushButton::clicked, [this]() {
        QString fileName = QFileDialog::getSaveFileName(this, tr("Сохранить файл"), "LogFile.csv", tr("Log (*.csv)"));

        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly)) {
            QTextStream stream(&file);
            for (const auto& r : m_logData) {
                stream << r.first.toString("hh.mm.ss.z") << ",";
                stream << r.second.yaw << ",";
                stream << r.second.pitch << ",";
                stream << r.second.roll << ",";
                stream << r.second.ammeter << ",";
                stream << r.second.voltmeter << ",";
                stream << r.second.depth << ",";
                stream << r.second.regulatorsFeedback << endl;
            }
            qInfo() << "Лог файл успешно записан.";

        } else {
            qWarning() << "Невозможно создать лог файл!";
        }
    });
}

void LogWidget::createLayout()
{
    QHBoxLayout* layout = new QHBoxLayout;

    layout->addWidget(m_saveLogButton.data());
    layout->addWidget(m_startStopButton.data());

    QVBoxLayout* manLayout = new QVBoxLayout;
    manLayout->addWidget(m_logRecordsCount.data());
    m_logRecordsCount.data()->setAlignment(Qt::AlignCenter);
    manLayout->addLayout(layout);
    setLayout(manLayout);
}
