#include "MainWindow.hpp"

#include <QApplication>
#include <QDateTime>
#include <QStyleFactory>

void setMessageOutput(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    QString infoHtml = "<font color=\"Lime\">";
    QString alertHtml = "<font color=\"DeepPink\">";
    QString criticalHtml = "<font color=\"Red\">";
    QString debugHtml = "<font color=\"Aqua\">";
    QString endHtml = "</font>";
    QString infoMsg = msg;
    QString time = QDateTime::currentDateTime().toString("hh.mm.ss.z ");
    if (MainWindow::logWidget == nullptr) {
        QByteArray localMsg = msg.toLocal8Bit();
        switch (type) {
        case QtInfoMsg:
            fprintf(stdout, "Info: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
            break;
        case QtDebugMsg:
            fprintf(stdout, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
            break;
        case QtWarningMsg:
            fprintf(stdout, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
            break;
        case QtCriticalMsg:
            fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
            break;
        case QtFatalMsg:
            fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
            abort();
        }
    } else {
        switch (type) {
        case QtInfoMsg:
            if (MainWindow::logWidget != 0) {
                infoMsg = infoHtml + infoMsg;
                infoMsg = infoMsg + endHtml;
                time += infoMsg;
                MainWindow::logWidget->append(time.simplified().trimmed());
            }
            break;
        case QtDebugMsg:
            if (MainWindow::logWidget != 0) {
                infoMsg = debugHtml + infoMsg;
                infoMsg = infoMsg + endHtml;
                time += infoMsg;
                MainWindow::logWidget->append(infoMsg.trimmed());
            }
            break;
        case QtWarningMsg:
            if (MainWindow::logWidget != 0) {
                infoMsg = alertHtml + infoMsg;
                infoMsg = infoMsg + endHtml;
                time += infoMsg;
                MainWindow::logWidget->append(infoMsg.simplified().trimmed());
            }
            break;
        case QtCriticalMsg:
            if (MainWindow::logWidget != 0) {
                MainWindow::logWidget->append(QDateTime::currentDateTime().toString("HH:MM:SS"));
                infoMsg = criticalHtml + infoMsg;
                infoMsg = infoMsg + endHtml;
                time += infoMsg;
                MainWindow::logWidget->append(infoMsg.simplified().trimmed());
            }
            break;
        case QtFatalMsg:
            abort();
        default:
            break;
        }
    }
}

void makeDark(QApplication& a)
{
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(53, 53, 53));
    palette.setColor(QPalette::WindowText, Qt::white);
    palette.setColor(QPalette::Base, QColor(15, 15, 15));
    palette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    palette.setColor(QPalette::ToolTipBase, Qt::white);
    palette.setColor(QPalette::ToolTipText, Qt::white);
    palette.setColor(QPalette::Text, Qt::white);
    palette.setColor(QPalette::Button, QColor(53, 53, 53));
    palette.setColor(QPalette::ButtonText, Qt::white);
    palette.setColor(QPalette::BrightText, Qt::red);

    palette.setColor(QPalette::Highlight, QColor(61, 142, 201).lighter());
    palette.setColor(QPalette::HighlightedText, Qt::black);
    palette.setColor(QPalette::Disabled, QPalette::Text, Qt::darkGray);
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, Qt::darkGray);
    a.setStyle(QStyleFactory::create("fusion"));
    a.setPalette(palette);
}

int main(int argc, char* argv[])
{
    qInstallMessageHandler(setMessageOutput);

    QApplication a(argc, argv);
    makeDark(a);

    MainWindow w;
    w.show();
    qInfo() << "Программа запущена!";

    return a.exec();
}
