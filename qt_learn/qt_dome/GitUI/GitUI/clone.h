#ifndef CLONE_H
#define CLONE_H

#include "git.h"
#include "remoting.h"
#include <QFileSystemModel>

namespace Ui {
class Clone;
}

class Clone: public Remoting
{
    Q_OBJECT

public:
    Clone(QString sciezkaGit,QWidget *parent);
    ~Clone();

private:
    Ui::Clone *ui;
    QFileSystemModel * model;   /* wskaznik wykorzysytwany do tworzenia drzewa plików */
    virtual void otworz();      /* wykonuje git clone, czyli klonuje repozytorium z jakiegos serwera w nowe , na naszym dysku */
    virtual void wyswietlZwrot(QByteArray  zwrot);  /* wyswietla zwrot od git'a */
    void zmianaRadioButton();       /*gdy uzytkownik wybierze mozliwosc wlasnego nazewnictwa, odslon lineEdit i flagaOwnNaming=true*/
    void drzewiakPlikow(); //wyswietla drzewo plikow, do wybrania w ktorym katalogu zklonuje repo
    bool flagaOwnNaming;    /* wykorzystywana do weryfikacji kiedy user chce sam nazwac folder jakis zostanie zklonowany na jego dysku */
    QString wyznaczNazwe(QString sciezka); /*wyznacza nazwe folderu w jakim zostanie zainicjowane repo ".git"  w przypadku gdy uzytkownik nie
                                                wybiera swojej nazwy */
    void ostrzezenie();     /* wyswietla ostrzezenie */
};

#endif // CLONE_H
