#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QThread>
#include <QTimer>
#include <QList>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QFileDialog>
#include "odbccomm.h"
#include "modbustcpcomm.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
public slots:

    virtual void paintEvent(QPaintEvent * e);
    void stateUpdate(bool state);

private slots:
    void on_exit_clicked();
    void update();

    void on_save_clicked();

    void on_about_clicked();

private:
    Ui::Widget *ui;

    ModbusTCPComm* comm;
    odbcComm *db;
    QThread* dbThread;
    QTimer* timer;
    qint32 rollInterv;
};

#endif // WIDGET_H
