#pragma once

#include <QCheckBox>
#include <QWidget>

class RegulatorsWidget : public QWidget {
    Q_OBJECT
public:
    explicit RegulatorsWidget(QWidget* parent = nullptr);

signals:

public slots:
private:
    void createConnections();
    void createLayout();

    QScopedPointer<QCheckBox> m_depthRegulator;
};
