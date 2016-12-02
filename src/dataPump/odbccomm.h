#ifndef ODBCCOMM_H
#define ODBCCOMM_H


#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QStandardItemModel>
#include <QSqlError>
#include <QTimer>
#include <QList>
#include <QDateTime>
#include <QDebug>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QWidget>
#include <QDir>

class odbcComm : public QObject
{
    Q_OBJECT
public:
    explicit odbcComm(QObject* parent=0);
    ~odbcComm();

    QStandardItemModel* getModel();
    QList<float> values;
    bool getStates();
    qint32 queryLength;
    void exportFile(QWidget* w);


private:
    bool odbcConnected;
    QStandardItemModel* model;
    QSqlDatabase db;
    QSqlQuery* query;
    QTimer * timer;
    qint32 rollInterv,errorCount;
    QString sqlSentence;

    bool dbOpen();
    void dbClose();
    bool dbReOpen();
public slots:
    void update();
signals:
    void odbcCommStateChanged(bool);
};


#endif // ODBCCOMM_H
