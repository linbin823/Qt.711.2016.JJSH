#ifndef MODBUSTCPCOMM_H
#define MODBUSTCPCOMM_H


#include <QObject>
#include <QList>
#include <QtNetwork>
#include <QUdpSocket>
#include <QTimer>
#include <QHostAddress>
#include <QThread>
#include <QDebug>
#include "modbus/modbus.h"

class ModbusTCPComm: public QThread
{
Q_OBJECT
public:
    ModbusTCPComm();
    ~ModbusTCPComm();

    void setMbBit(qint16 off, uint8_t value);
    void setMbInputBit(qint16 off, uint8_t value);
    void setMbInputRegisters(qint16 off, uint16_t value);
    void setMbRegisters(qint16 off, uint16_t value);

    uint8_t  getMbBit(qint16 off);
    uint16_t getMbRegisters(qint16 off);
    bool getStates();
signals:
    void modbusCommStateChanged(bool);
    void modbusCommErr(const QString& s);
private:
    modbus_mapping_t* mbData;
    modbus_t* mbConnection;
    uint8_t * receiveBuff;
    void run();
    int listenSocket;
    bool mdConnected;
};




#endif // MODBUSTCPCOMM_H
