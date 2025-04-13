#include "MainWindow.hpp"
#include "RovSingleton.hpp"

#include <QDockWidget>
#include <QMenu>
#include <QMenuBar>
#include <QApplication>

QTextEdit* MainWindow::logWidget = nullptr;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_telimetryWidget(new RovTelimetryWidget(this))
    , m_cameraWidget(new RovCameraWidget(this))
    , m_scaleWidget(new ScaleFactorWidget(this))
    , m_joystick(new SfJoystick(this))
    , m_regulatorsWidget(new RegulatorsWidget(this))
    , m_debugDialog(new DebugDialog(this))
    , m_logFileWidget(new LogWidget(this))
{
    logWidget = new QTextEdit;
    createActions();
    createMenus();
    createConnections();
    createDocks();
    setCentralWidget(m_cameraWidget.data());
    setMinimumSize(640, 480);
}

MainWindow::~MainWindow() {}

void MainWindow::createMenus()
{
    QMenu* camera = menuBar()->addMenu(tr("Камера"));
    camera->addAction(m_startCameraAct.data());
    camera->addAction(m_stopCameraAct.data());
    camera->addAction(m_switchCameraAct.data());

    QMenu* settings = menuBar()->addMenu(tr("Настройки"));
    settings->addAction(m_openJoystickSettings.data());
    settings->addAction(m_openDebugDialog.data());
}

void MainWindow::createDocks()
{
    QMenu* view = menuBar()->addMenu(tr("&Вид"));

    QDockWidget* consDock = new QDockWidget(tr("Телеметрия"), this);
    consDock->setObjectName("TelimetryDockWidget");
    consDock->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);
    consDock->setWidget(m_telimetryWidget.data());
    addDockWidget(Qt::LeftDockWidgetArea, consDock);
    view->addAction(consDock->toggleViewAction());

    QDockWidget* logDock = new QDockWidget(tr("Информация"), this);
    logDock->setObjectName("LogDockWidget");
    logDock->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);
    logDock->setWidget(logWidget);
    logWidget->setFont(QFont("Consolas"));
    addDockWidget(Qt::LeftDockWidgetArea, logDock);
    view->addAction(logDock->toggleViewAction());

    QDockWidget* scaleDock = new QDockWidget(tr("Ограничитель тяги"), this);
    scaleDock->setObjectName("ScaleWidget");
    scaleDock->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);
    scaleDock->setWidget(m_scaleWidget.data());
    addDockWidget(Qt::LeftDockWidgetArea, scaleDock);
    view->addAction(scaleDock->toggleViewAction());

    QDockWidget* regulatorsDock = new QDockWidget(tr("Регуляторы"), this);
    regulatorsDock->setObjectName("RegulatorsWidget");
    regulatorsDock->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);
    regulatorsDock->setWidget(m_regulatorsWidget.data());
    addDockWidget(Qt::LeftDockWidgetArea, regulatorsDock);
    view->addAction(regulatorsDock->toggleViewAction());

    QDockWidget* logFileDock = new QDockWidget(tr("Логи"), this);
    logFileDock->setObjectName("RegulatorsWidget");
    logFileDock->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);
    logFileDock->setWidget(m_logFileWidget.data());
    addDockWidget(Qt::LeftDockWidgetArea, logFileDock);
    view->addAction(logFileDock->toggleViewAction());
}

void MainWindow::createConnections()
{
    QObject::connect(m_startCameraAct.data(), &QAction::triggered, [this](bool) {
        m_cameraWidget->startCapture();
    });

    QObject::connect(m_stopCameraAct.data(), &QAction::triggered, [this](bool) {
        m_cameraWidget->stopCapture();
    });

    QObject::connect(m_switchCameraAct.data(), &QAction::triggered, [this](bool) {
        RovSingleton::instance()->control().cameraIndex = (RovSingleton::instance()->control().cameraIndex + 1) % 2;
    });

    QObject::connect(m_openJoystickSettings.data(), &QAction::triggered, [this](bool) {
        m_joystick.data()->settingsDialog()->show();
    });

    QObject::connect(m_openDebugDialog.data(), &QAction::triggered, [this](bool) {
        m_debugDialog.data()->show();
    });
}

void MainWindow::createActions()
{
    m_startCameraAct.reset(new QAction(tr("Начать захват изображения"), this));
    m_stopCameraAct.reset(new QAction(tr("Остановить захват изображения"), this));
    m_switchCameraAct.reset(new QAction(tr("Переключить камеру (мультиплексор)"), this));
    m_openJoystickSettings.reset(new QAction(tr("Настройки джойстика"), this));
    m_openDebugDialog.reset(new QAction(tr("Отладка движителей"), this));
}
