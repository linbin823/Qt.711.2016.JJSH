#if defined(_WIN32)
#include <ws2tcpip.h>
#else
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include "modbustcpcomm.h"


ModbusTCPComm::ModbusTCPComm(){
   mbConnection = modbus_new_tcp(NULL, MODBUS_TCP_DEFAULT_PORT);
   if (mbConnection == NULL) {
//        qDebug()<<"Unable to allocate libmodbus context:"<<modbus_strerror(errno);
   }



   mbData = modbus_mapping_new( 500,500,500,500 );
   if (mbData == NULL) {
//       qDebug()<<"Failed to allocate the mapping:"<<modbus_strerror(errno);
       modbus_free(mbConnection);
   }

   receiveBuff= (uint8_t*) malloc( 1000 * sizeof(uint8_t));
   memset(receiveBuff, 0 , 1000 * sizeof(uint8_t));

   mdConnected = false;
   emit modbusCommStateChanged(mdConnected);
}


ModbusTCPComm::~ModbusTCPComm(){

    modbus_free (mbConnection);
    modbus_mapping_free (mbData);
}

void ModbusTCPComm::setMbBit(qint16 off, uint8_t value){
    mbData->tab_bits[off] = value;
}

void ModbusTCPComm::setMbInputBit(qint16 off, uint8_t value){
    mbData->tab_input_bits[off] = value;
}

void ModbusTCPComm::setMbInputRegisters(qint16 off, uint16_t value){
    mbData->tab_input_registers[off] = value;
}

void ModbusTCPComm::setMbRegisters(qint16 off, uint16_t value){
    mbData->tab_registers[off] = value;
}

uint8_t  ModbusTCPComm::getMbBit(qint16 off){
    return mbData->tab_bits[off];
}

uint16_t ModbusTCPComm::getMbRegisters(qint16 off){
    return mbData->tab_registers[off];
}



void ModbusTCPComm::run(){

    int rc;
    modbus_set_debug(mbConnection, false);
    listenSocket = modbus_tcp_listen(mbConnection, 1);
    //准备一个新建端口
    socklen_t addrlen;
    struct sockaddr_in clientaddr;
    int newfd;
//    qDebug()<<"ModbusTCPComm::run"<<"initial...";
    while(true){
        /* Handle new connections */
        addrlen = sizeof(clientaddr);
        memset(&clientaddr, 0, sizeof(clientaddr));
        newfd = accept(listenSocket, (struct sockaddr *)&clientaddr, &addrlen);
        if (newfd == -1) {
//            qDebug()<<"ModbusTCPComm::run"<<"accepted error";
            continue;
        }
        mdConnected = true;
        emit modbusCommStateChanged(mdConnected);
        while(true){
            modbus_set_socket(mbConnection, newfd);
            rc = modbus_receive(mbConnection, receiveBuff);
            if (rc > 0) {
                modbus_reply(mbConnection, receiveBuff, rc, mbData);
            }
            else if (rc == -1) {
//                qDebug()<<"ModbusTCPComm::run"<<"close socket";
                closesocket(newfd);
                mdConnected = false;
                emit modbusCommStateChanged(mdConnected);
                break;
            }
        }
    }

//        mdConnected = false;
//        emit modbusCommStateChanged(mdConnected);
//        modbus_tcp_accept(mbConnection, &listenSocket);
//        qDebug()<<"ModbusTCPComm::run"<<"accepted!";
//        mdConnected = true;
//        emit modbusCommStateChanged(mdConnected);
//        for (;;) {
//            do {
//                rc = modbus_receive(mbConnection, receiveBuff);
//            } while (rc == 0);
//            //发生错误，而且是要重启的错误；或者客户端关闭连接
//            if (rc == -1 && errno != EMBBADCRC) {
//                /* Quit */
//                //qDebug()<<"reveive errror!"<<errno<<rc<<modbus_strerror(errno);
//                break;
//            }
//            rc = modbus_reply(mbConnection, receiveBuff, rc, mbData);
//            if (rc == -1) {
//                qDebug()<<"send errror!"<<errno<<modbus_strerror(errno);
//                break;
//            }
//        }
//        qDebug()<<"Quit the loop:"<<modbus_strerror(errno);
//        modbus_close(mbConnection);

}

bool ModbusTCPComm::getStates(){

    return mdConnected;
}
