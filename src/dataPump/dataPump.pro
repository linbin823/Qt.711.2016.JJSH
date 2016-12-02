#-------------------------------------------------
#
# Project created by QtCreator 2016-06-08T09:14:43
#
#-------------------------------------------------

QT       += core gui sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = dataPump
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    modbustcpcomm.cpp \
    modbus/modbus-data.c \
    modbus/modbus-rtu.c \
    modbus/modbus-tcp.c \
    modbus/modbus.c \
    odbccomm.cpp

HEADERS  += widget.h \
    modbustcpcomm.h \
    modbus/modbus-private.h \
    modbus/modbus-rtu-private.h \
    modbus/modbus-rtu.h \
    modbus/modbus-tcp-private.h \
    modbus/modbus-tcp.h \
    modbus/modbus-version.h \
    modbus/modbus.h \
    odbccomm.h

FORMS    += widget.ui

LIBS += -lgdi32\
        -lWs2_32
