#include "widget.h"
#include "ui_widget.h"


Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    ui->db->setText("Remote Database Connection Failed!");
    ui->com->setText("Wincc Not Connected!");
    db = new odbcComm();
    dbThread =new QThread(this);
    db->moveToThread(dbThread);
    dbThread->start();
    ui->tableView->setModel( db->getModel() );

    comm = new ModbusTCPComm();
    comm->start();

    rollInterv = 2000;
    timer = new QTimer(this);
    timer->setInterval(rollInterv);
    timer->start();
    connect(timer,SIGNAL(timeout()),this,SLOT(update()));
    connect(comm,SIGNAL(modbusCommStateChanged(bool)),this,SLOT(stateUpdate(bool)));
    connect(db,SIGNAL(odbcCommStateChanged(bool)),this,SLOT(stateUpdate(bool)));
    connect(comm,SIGNAL(modbusCommErr(QString)),ui->com,SLOT(setText(QString)));

    int current = 0;
    foreach( QHostAddress t, QNetworkInterface::allAddresses()){
        if (t.protocol() == QAbstractSocket::IPv4Protocol && !t.isLoopback() &&
                t.toString().left(7) == "192.168" ){
            ui->IP->addItem(t.toString(),QVariant( t.toIPv4Address() ));
            current ++;
        }
    }
}

Widget::~Widget()
{
    delete ui;
    delete timer;
    comm->terminate();
    comm->deleteLater();
    dbThread->terminate();
    dbThread->deleteLater();
}


void Widget::paintEvent(QPaintEvent * e){
    ui->tableView->resize( this->width()-40, this->height()-90 );
    QWidget::paintEvent( e);
}

void Widget::on_exit_clicked()
{
    close();
}

void Widget::update(){
    int index = 0;
    float temp;
    uint16_t res_low,res_high;
    uint16_t* res;
    //qDebug()<<count;
    for(;index < db->queryLength; index++){
        temp = db->values.at(index);
        res = ( uint16_t* )&temp;
        res_low = *res;
        res_high = *(res+1);
        comm->setMbRegisters(index *2 , res_low);
        comm->setMbRegisters(index *2 +1 , res_high);
    }
}

void Widget::stateUpdate(bool state){
    Q_UNUSED(state)
//    if(!comm->getStates() )
//        ui->com->setText("Wincc Not Connected!");
//    else
//        ui->com->setText("Wincc Connected!");

    if(!db->getStates() )
        ui->db->setText("Remote Database Connection Failed!");
    else
        ui->db->setText("Remote Database Connected!");

}

void Widget::on_save_clicked(){
    db->exportFile(this);
}

void Widget::on_about_clicked()
{
    QMessageBox::information(this,tr("'data pump'"),tr("SMDERI Automation Dept\nVersion 1.0\n2016.08"), tr("OK"));
}
