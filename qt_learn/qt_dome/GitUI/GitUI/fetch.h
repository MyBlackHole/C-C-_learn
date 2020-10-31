#ifndef FETCH_H
#define FETCH_H

#include <QDialog>
#include "remoting.h"

namespace Ui {
class Fetch;
}

class Fetch : public Remoting
{
    Q_OBJECT

public:
    explicit Fetch(QString sciezkaGita,QWidget *parent = 0);
    ~Fetch();
    
private:
    Ui::Fetch *ui;
    void otworz();  /* wykonuje git fetch z podanego w comboBoxie remote'a */
    void wyswietlZwrot(QByteArray zwrot);       /* wyswietla zwrot od git'a */
    void zczytajRepo(QByteArray zwrot);     /* zczytuje repozytoria */
    void zczytajBrancheRemote(QByteArray zwrot);    /* zczytuje Branche z repozytoriow zdalnych */
    void zczytajBranche(QByteArray zwrot);      /* zczytuje branche z lokalnego repo git'a na ktorym pracujemy */
    bool flagaPoczatku; /*na poczatku wykonuje szereg polecen gita, zeby wyplenic poszczegolne pola, a potem juz inny zestaw, dlatego musze okreslic*/
    bool flagaBranchuRemoting; /*gdy zczytuje branche i mam pusty odbior, to potem zle dziala i wypelnia nieprawidlowo comboBoxa tak zabezpiecze */
    void aktualizujBrancheRemote(QByteArray zwrot); /* gdy fetchuje to moga mi dojsc nowe galezie, funkcja je aktualizuje po fetchu */

private slots:
    void merge();           /* git merge po prostu */
    void zmianaBrancha();   /* zmiana brancha, git checkout */
};

#endif // FETCH_H
