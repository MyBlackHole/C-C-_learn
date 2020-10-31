#include "pull.h"
#include "ui_pull.h"

Pull::Pull(QString sciezkaGita,QWidget *parent) :
    Remoting(sciezkaGita,parent),
    ui(new Ui::Pull)
{
    ui->setupUi(this);
}

Pull::~Pull()
{
    delete ui;
}
void Pull::otworz(){

}
void Pull::stworz(){

}
void Pull::wyswietlZwrot(QByteArray zwrot){

}
