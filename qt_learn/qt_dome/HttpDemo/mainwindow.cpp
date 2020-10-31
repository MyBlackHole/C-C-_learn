#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "getdatathread.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    ui->textEdit_2->clear();

    getDataThread* dataThread = new getDataThread(ui->lineEdit->text(),1);
    connect(dataThread, SIGNAL(getDataThreadFinish(QString*)), this, SLOT(showData(QString*)));
    dataThread->start();
}

void MainWindow::on_pushButton_2_clicked()
{
    ui->textEdit_2->clear();

    getDataThread* dataThread = new getDataThread(ui->lineEdit->text(), 2, ui->textEdit->toPlainText());
    connect(dataThread, SIGNAL(getDataThreadFinish(QString*)), this, SLOT(showData(QString*)));
    dataThread->start();

}

void MainWindow::replyFinished(QNetworkReply *reply)
{
    qDebug("replyFinished\n");

    if(reply->error() == QNetworkReply::NoError)
    {
        QByteArray bytes = reply->readAll();
        QString str = QString::fromUtf8(bytes);
        ui->textEdit_2->setText(str);

        qDebug("str[%s]\n",str.toStdString().c_str());
    }
}

void MainWindow::showData(QString *res)
{
    qDebug("res[%s]\n",(*res).toStdString().c_str());
    ui->textEdit_2->setPlainText(*res);
}
