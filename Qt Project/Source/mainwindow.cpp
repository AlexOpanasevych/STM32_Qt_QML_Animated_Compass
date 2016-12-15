#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPort>
#include <QTimer>
#include <QLabel>
#include <QLineEdit>
#include <QFile>
#include <QString>
#include <QMessageBox>
#include <QFileDialog>
#include <QtSerialPort/QSerialPortInfo>
#include <QDialog>
#include <QThread>
#include <QFontMetrics>
#include <QHBoxLayout>
#include <QUrl>
#include <QQmlApplicationEngine>
#include <QQuickWidget>
#include <QtQuick>

//Variables
QSerialPort *serial;
QTimer *timer;
QByteArray ReceivedData;
QByteArray TransmitData;
qint8 id = 0;
int portstatus = 0;
unsigned char zerop = 0x00;
char datax[400];
char datax1[40];
char datax2[40];
char datax3[40];
int X = 0;
int Y = 0;
int Z = 0;

int MainWindow:: compare_array(unsigned char *arr1, unsigned char *arr2, int count, int position){
    int i = 0;
    while (i != count){
        if (arr2[i] != arr1[position+i]){
            return 0;
        }
        i++;
    }
    return 1;
}
void MainWindow:: copy_array_an_to_bn(unsigned char *source, int *destination, int count, int aposition, int bposition){
    int i = 0;
    while (i != count){
        destination[bposition+i] = source[aposition+i];
        i++;
    }
}
void cparr(char *source, char *destination, int count, int aposition, int bposition){
    int i = 0;
    while (i != count){
        destination[bposition+i] = source[aposition+i];
        i++;
    }
}
void MainWindow::timerOverflowed(){
    timer->stop();
    ReceivedData.clear();
    on_actionSetButtonAction_triggered();
}

void MainWindow::serialReceived(){
    ReceivedData.append(serial->readAll());
    timer->stop();
    timer->start(50);

    if (ReceivedData.length() == 28){
        if (ReceivedData.at(0) == 'S'){
            int i = 0;
            while (i != 27){
                datax[i] = ReceivedData.at(i);
                i++;
            }
            datax[27] = 0;

            cparr(datax,datax1,5,4,0);
            datax1[5] = 0x00;
            X = (int)strtol(datax1, NULL, 10);

            cparr(datax,datax2,5,12,0);
            datax2[5] = 0x00;
            Z = (int)strtol(datax2, NULL, 10);

            cparr(datax,datax3,5,20,0);
            datax3[5] = 0x00;
            Y = (int)strtol(datax3, NULL, 10);

            float Hdeg = atan2(X,Y)/0.0174532925;
            if (Hdeg<0) Hdeg+=360;
            Hdeg=360-Hdeg-90;
            if (Hdeg<0) Hdeg=Hdeg+360;
            rect->setProperty("angle", (int)Hdeg);

            float VXdeg = atan2(X,Z)/0.0174532925;
            if (VXdeg<0) VXdeg+=360;
            VXdeg=360-VXdeg-180;
            if (VXdeg<0) VXdeg=VXdeg+360;

            float VYdeg = atan2(Y,Z)/0.0174532925;
            if (VYdeg<0) VYdeg+=360;
            VYdeg=360-VYdeg-180;
            if (VYdeg<0) VYdeg=VYdeg+360;

            QString ppX = QString::number((int)Hdeg,'i',0);
            QString ppY = QString::number((int)VXdeg,'i',0);
            QString ppZ = QString::number((int)VYdeg,'i',0);
        }
        ReceivedData.clear();
        on_actionSetButtonAction_triggered();
    }
}

void MainWindow::on_actionConnectButtonAction_triggered(){
    uint16_t vid = 0;
    uint16_t pid = 0;
    int fr = 0;
    if (portstatus == 0){
        fr = 1;
        QString RPT;
        for (QSerialPortInfo port : QSerialPortInfo::availablePorts()){
            vid = port.vendorIdentifier();
            pid = port.productIdentifier();
            if (vid == 0x0403){
                if(pid == 0x6001){
                    RPT.clear();
                    RPT.append(port.portName());
                    serial->setPortName(RPT);
                    serial->setBaudRate(9600);
                    serial->setDataBits(QSerialPort::Data8);
                    serial->setParity(QSerialPort::NoParity);
                    serial->setStopBits(QSerialPort::OneStop);
                    serial->setFlowControl(QSerialPort::NoFlowControl);
                    if((serial->open(QIODevice::ReadWrite) == true)){
                        ui->ConnectButton->setText("Разъединять");
                        portstatus = 1;
                        ui->StatusLabel->setText("Соединено!");
                        fr = 2;
                        on_actionSetButtonAction_triggered();
                    }
                }
            }
        }
    }
    else{
        serial->close();
        ui->ConnectButton->setText("Соединять");
        portstatus = 0;
        ui->StatusLabel->setText("Разъединено!");
    }
    if (fr == 1){
        ui->StatusLabel->setText("Устройство не обнаружено!");
    }
}
void MainWindow::on_actionSetButtonAction_triggered(){
    if (portstatus == 1){
        TransmitData.clear();
        TransmitData.append("R");
        serial->write(TransmitData);
        timer->stop();
        timer->start(50);
    }
}
void MainWindow::handleBytesWritten(qint64 bytes){
}
QObject *rect;
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow){
    //Error reject timer
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timerOverflowed()));
    //Serial port
    serial = new QSerialPort(this);
    connect(serial,SIGNAL(readyRead()),this,SLOT(serialReceived()));
    connect(serial, SIGNAL(bytesWritten(qint64)), SLOT(handleBytesWritten(qint64)));
    //UI
    setFixedSize(450, 448);
    ui->setupUi(this);
    ui->quickWidget->setSource(QUrl::fromLocalFile("./compass_qml.qml"));

    QObject *object = ui->quickWidget->rootObject();

    rect = object->findChild<QObject*>("rect");
}

MainWindow::~MainWindow(){
    delete ui;
    serial->close();
}


