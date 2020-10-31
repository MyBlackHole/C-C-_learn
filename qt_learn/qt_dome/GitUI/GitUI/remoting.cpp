#include "remoting.h"
#include "ui_remoting.h"
#include "remote.h"
Remoting::Remoting(QString sciezkaGita,QWidget *parent)
    : QDialog(parent)
    , sciezkaGita(sciezkaGita)
    , ui(new Ui::Remoting)
    , flagaBranchRemot(0)
{
    proces = new QProcess(this);
}

Remoting::~Remoting()
{
    delete ui;
    delete proces;
}

void Remoting:: zczytajZwrot()
{
    QByteArray bytes = proces->readAllStandardOutput();
    wyswietlZwrot(bytes);               /* wykorzystanie polimorfizmu - metoda w tej klasie czysto wirtualna, ale ze tak na prawde jest to obiekt
                                           Remote, to wywołuje polimorficznie funkcję z klasy Remote, a tam jest zdefiniowana */
}

void Remoting::usunRepo()
{

}

void Remoting::dodajRepo()
{

}

void Remoting::zmianaRadioButton()
{

}
