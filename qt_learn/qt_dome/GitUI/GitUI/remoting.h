#ifndef REMOTING_H
#define REMOTING_H

#include <QDialog>
#include <QProcess>

#include "git.h"

namespace Ui {
class Remoting;
}

class Remoting : public QDialog, public git
{
    Q_OBJECT
    
public:
    explicit Remoting(QString sciezkaGita,QWidget *parent = 0);
     virtual ~Remoting();

protected:
    QString sciezkaGita;        /* sciezka do repo git'a */
    QProcess * proces;          /* proces, do wywolywania polecen git'a */
    Ui::Remoting *ui;
    int flagaBranchRemot;       /* wykorzysytywana przy wczytywaniu branchów z danego repo i branchow remote oraz samych remote - w klasie push*/

protected slots:
    virtual void otworz() =0;   /* slot czysto wirtualny - najwazniejsza funkcja w dziedziczacych po tej klasach ( push, remote, fetch ) */
    void zczytajZwrot();        /* zczytuje zwrot od programu git */
    virtual void wyswietlZwrot(QByteArray  zwrot)=0; /* wyswietla zwrot od git'a, definicja w klasach dziedziczacych */
    virtual void usunRepo();    /* wykorzysytwane w remote - usuwa dane repozytorium zdalne (nie fizycznie)- ze sledzonych przez git'a*/
    virtual void dodajRepo();   /* wykorzystywane w remote - dodaje zdalne repozytorium do sledzonych przez git'a */
    virtual void zmianaRadioButton();
};

#endif // REMOTING_H
