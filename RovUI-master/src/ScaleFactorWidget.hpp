#pragma once

#include <QDialog>
#include <QRadioButton>
#include <QWidget>

class ScaleFactorWidget : public QWidget {
    Q_OBJECT
public:
    explicit ScaleFactorWidget(QWidget* parent = nullptr);

signals:

public slots:
private:
    void createLayout();
    void createConnections();
    QScopedPointer<QRadioButton> m_fiftyButton;
    QScopedPointer<QRadioButton> m_twentyFive;
    QScopedPointer<QRadioButton> m_oneHundred;
};
