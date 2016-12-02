#include "odbccomm.h"

odbcComm::odbcComm(QObject *parent):QObject(parent)
{

        rollInterv = 2500;
        timer = new QTimer(this);
        timer->setInterval( rollInterv );
        timer->start();

        connect(timer,SIGNAL(timeout()),this,SLOT(update()));

        errorCount = 0;
        odbcConnected = false;
        db = QSqlDatabase::addDatabase("QODBC");
        dbOpen();
        sqlSentence = "select NAME,IP_VALUE,IP_VALUE_QUALITY,IP_VALUE_TIME from IP_AnalogDef where NAME LIKE '%0720%'";
        query = new QSqlQuery(db);
        query->setForwardOnly(true);

        model = new QStandardItemModel(this);
        QStringList title;
        title<<"Address"<<"Name"<<"Value"<<"Quailty"<<"LastUpdate";
        model->setHorizontalHeaderLabels( title );

        query->exec( sqlSentence );
        int index = 0;
        while(query->next() ){
            model->setItem(index,0, new QStandardItem( QString::number( index * 2 + 40000 ) ) );
            model->setItem(index,1, new QStandardItem( query->value(0).toString() ) );
            model->setItem(index,2, new QStandardItem( QString::number( query->value(1).toFloat() ) ) );
            model->setItem(index,3, new QStandardItem( query->value(2).toString() ) );
            model->setItem(index,4, new QStandardItem( query->value(3).toString() ) );
            values<<0.0<<0.0;
            index ++;
        }
        queryLength = index;
        model->setRowCount(index * 2 );
}

odbcComm::~odbcComm(){
        delete query;
        delete timer;
        delete model;
        dbClose();
}

bool  odbcComm::dbOpen(){
  if( db.isValid() && !db.isOpen() ){
     db.setDatabaseName("jjsh");
     db.setUserName("");
     db.setPassword("");
     if(db.open()){
         //qDebug()<<"Database Open Successed!";
         odbcConnected = true;
         emit odbcCommStateChanged(odbcConnected);
         errorCount = 0;
         return true;
     }
     else{
         //qDebug()<<"Database Open Failed!";
         odbcConnected = false;
         emit odbcCommStateChanged(odbcConnected);
         return false;
     }
  }
  else{
    //qDebug()<<"Database Open Fault!";
    odbcConnected = false;
    emit odbcCommStateChanged(odbcConnected);
    return false;
  }
}

void  odbcComm::dbClose(){
    if( db.isValid() ){
        if(db.isOpen()){
            db.close();
        }
    }
    odbcConnected = false;
    emit odbcCommStateChanged(odbcConnected);
    query->finish();
}

bool  odbcComm::dbReOpen(){
    dbClose();
    return dbOpen();
}

QStandardItemModel *odbcComm::getModel(){
    return model;
}

bool odbcComm::getStates(){
    return odbcConnected;
}

void odbcComm::update(){
    if(odbcConnected){
        if( query->exec( sqlSentence ) ){
            //qDebug()<<"odbc::update"<<"query ok";
            errorCount = 0;
            int index = 0;
            QStandardItem* temp;
            while(query->next() ){
                temp = model->item(index,2);
                temp->setText( QString::number(query->value(1).toFloat() ));
                temp = model->item(index,3);
                temp->setText( query->value(2).toString() );
                temp = model->item(index,4);
                temp->setText( query->value(3).toString() );
                values[index] = query->value(1).toFloat();
                //query->value(0).toFloat();
                index ++;
            }
            queryLength = index;
            query->finish();
        }
        else{
            errorCount++;
            //qDebug()<<query->lastError().databaseText();
       }
       if(errorCount >= 10){
            odbcConnected = false;
            emit odbcCommStateChanged(odbcConnected);
       }
    }
    else{
        //qDebug()<<"update error, reOpening";
        dbReOpen();
    }
}

void odbcComm::exportFile(QWidget* w){

    //获取创建的csv文件名
    QString fileName = QFileDialog::getSaveFileName(w, tr("Excel file"),QDir::currentPath() ,
                                            tr("Files (*.csv)"));
    if (fileName.isEmpty())
    return;
    //打开.csv文件
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)){
        qDebug()<< "Cannot open file for writing: "
        <<file.errorString();
        return;
    }
    QTextStream out(&file);
    //获取数据
    out<<"Address,"<<"Name,"<<"Value,"<<"Quailty,"<<"LastUpdate,"<<"\n";
    //获取表格内容
    int index = 0;
    QStandardItem* temp;
    while( index < queryLength ){
        temp = model->item(index,0);
        out<<temp->text()<<",";
        temp = model->item(index,1);
        out<<temp->text()<<",";
        temp = model->item(index,2);
        out<<temp->text()<<",";
        temp = model->item(index,3);
        out<<temp->text()<<",";
        temp = model->item(index,4);
        out<<temp->text()<<"\n";
        index ++;
    }
    QMessageBox::information(w, tr("data export successed!"), tr("data export successed!/nto CSV file: %1").arg(fileName), tr("OK"));
    file.close();

}
