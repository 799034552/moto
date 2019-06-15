#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include<QLabel>
#include<QTimer>
#include "stdafx.h"
#include "SerialPort.h"
#include <QDebug>
#include <stdlib.h>
#include <stdio.h>
#include<mywidght.h>
#include "winsock2.h"
#include<ws2tcpip.h>
#include<QGridLayout>
#include<QPainter>
CSerialPort mySerialPort;
int way = 0;
void tcpInit();
int ttcpInit();
int l = 400;
float roll_one = 45,roll_two = 0;
struct timeval tv = {1,0};
int Result = 0;
char RecvBuf[1400] = "",SendBuf[1400] = "";
int nInputLen = 0,nIndex = 0,nLeft = 0;
fd_set fd_read;
fd_set fd_write;
MainWindow* that;
SOCKET sockClient;
SOCKET SocketClient;
WSADATA wsaData;
char buff[1024];
QTimer *timer1;
char isReady = 0x0;
QTimer *timer;
int isShow = -1;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    that = this;
    setFixedSize(this->width(),this->height());
    timer = new QTimer(this);
    timer1 = new QTimer(this);
    timer1->start(1000);
    timer->start(1000);
    connect(timer,SIGNAL(timeout()),this,SLOT(hideSuccess()));
    connect(timer1,SIGNAL(timeout()),this,SLOT(hideSuccess1()));


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_triggered()
{
    if(way == 1)
        return;
    ui->actionWifi->setChecked(false);
    way = 1;
    if (!mySerialPort.InitPort(9,CBR_115200))
    {
        qDebug("打开串口失败");
        ui->label_2->setStyleSheet("color:red");
        ui->label_2->setText("蓝牙连接失败");

    }
    else
    {
        qDebug("打开串口成功");
        ui->label_2->setStyleSheet("color:green");
        ui->label_2->setText("蓝牙连接成功");
    }
    mySerialPort.OpenListenThread();
}


void MainWindow::on_pushButton_clicked()
{

    if(way == 1)
    {



        if(!mySerialPort.BuffSend('1'))
        {

            qDebug("发送失败");
        }
    }


}

void MainWindow::on_pushButton_2_clicked()
{
    if(way == 1)
    {
        qDebug("1111");
        if(!mySerialPort.BuffSend('2'))
        {
            qDebug("发送失败");
        }
    }

}

void MainWindow::on_pushButton_3_clicked()
{
    if(way == 1)
    {
        qDebug("1111");
        if(!mySerialPort.BuffSend('0'))
        {
            qDebug("发送失败");
        }
    }

}
void MainWindow::hideSuccess()
{
    if(ui->label_3->text() == "发送成功了")
        isShow = 2;

}
void MainWindow::hideSuccess1()
{
    if(--isShow)
    {
    ui->label_3->setText("");
    }

}
void MainWindow::showMessage(QString a)
{
    ui->label_3->setText(a);
    timer->start(1000);
    connect(timer,SIGNAL(timeout()),this,SLOT(hideSuccess()));
}


void MainWindow:: paintEvent(QPaintEvent *Event)
{
    QPainter painter(this);
    painter.drawLine(500,400,860,400);
    painter.drawLine(680,400,680,200);
    painter.drawLine(680,200,680+100*sin(roll_one*3.14/180),200+100*(cos(roll_one*3.14/180)));
    painter.drawLine(680+100*sin(roll_one*3.14/180)-25*cos(roll_two*3.14/180),200+100*(cos(roll_one*3.14/180))-25*sin(roll_two*3.14/180),
                     680+100*sin(roll_one*3.14/180)+25*cos(roll_two*3.14/180),200+100*(cos(roll_one*3.14/180))+25*sin(roll_two*3.14/180));
    //painter.drawRect(10,10,l,l);
}

void MainWindow:: getAngle(QString a)
{
    //qDebug()<<"输出"<<a[0];
    if(a == "get")
    {
        ui->label_3->setStyleSheet("color:green");
        ui->label_3->setText("发送成功了");

    }

    if(a[0] == '8')
    {
        isReady |= 0x1;
        roll_one = a.mid(1).toFloat();
        update();
        //qDebug()<<"one:"<<roll_one;
    }
    else if(a[0] == '9')
    {
        isReady |= 0x2;
        roll_two = a.mid(1).toFloat();
        //qDebug()<<"two:"<<roll_two;
        update();
    }
    if(isReady == 0x3)
      {
        ui->label_4->setStyleSheet("color:green");
        ui->label_4->setText("准备就绪");
    }


}
void MainWindow::tcpGet()
{
    if(way == 2)
    {
        FD_ZERO(&fd_read);
        FD_ZERO(&fd_write);
        FD_SET(SocketClient,&fd_read);
        FD_SET(SocketClient,&fd_write);
        Result = select(0,&fd_read,&fd_write,NULL,&tv);
        if(FD_ISSET(SocketClient,&fd_read))
        {
            memset(RecvBuf,0,sizeof(RecvBuf));
            Result = recv(SocketClient,RecvBuf,sizeof(RecvBuf)-1,0);
            if(Result == SOCKET_ERROR)
            {
                printf("……recv 失败：%d\n",WSAGetLastError());

            }
            else if(Result == 0)
            {
                printf("……Server Has Closed\n",WSAGetLastError());

            }
            RecvBuf[Result] = '\0';
            printf("__Received: %s\n",RecvBuf);
            qDebug()<<RecvBuf;
        }
    }
    else
    {
//        FD_ZERO(&fd_read);
//        FD_ZERO(&fd_write);
//        FD_SET(SocketClient,&fd_read);
//        FD_SET(Sock etClient,&fd_write);
//        Result = select(0,&fd_read,&fd_write,NULL,&tv);
//        if(FD_ISSET(SocketClient,&fd_write) )
//        {
//            char bu [] = "123";
//                Result = send(SocketClient,bu,sizeof(bu),0);
//                if(Result == SOCKET_ERROR)
//                {


//                }
//                else if(Result == 0)
//                {


//                }
//        }

    }
}


void MainWindow::on_pushButton_4_clicked(bool checked)
{
        if(!mySerialPort.BuffSend('6'))
        {
            qDebug("发送失败");
        }
        ui->pushButton_4->setText("音效：开");


}

void MainWindow::on_action_triggered(bool checked)
{

}

void MainWindow::on_pushButton_5_clicked()
{
    if(!mySerialPort.BuffSend('3'))
    {
        qDebug("发送失败");
    }

}

void MainWindow::on_pushButton_4_clicked()
{

}
