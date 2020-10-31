#ifndef REMOTE_H
#define REMOTE_H

#include "git.h"
#include "remoting.h"

class Remote: public Remoting
{
public:
    Remote(QString sciezkaGita, QWidget *parent);
    ~Remote();

    void wyswietlZwrot(QByteArray zwrot);

private slots:
    virtual void otworz();                  /* uruchamia git remote -v , ktore wyswietla info o aktualnych zdalnych repo */
    virtual void usunRepo();                /* usuwa zdalne repozytorium o podanej nazwie z comboBox */
    void skanujRepo(QByteArray zwrot);      /* wpisuje do comboBoxa nazwy zdalnych repo */
    bool flagaPoleceniaRemote;              /* do informacji kiedy aktualizowac comboBoxa */
    virtual void dodajRepo();                       /* dodaje zdalne repozytorium do sledzonych przez git'a */
};

#endif // REMOTE_H
