#ifndef PUSH_H
#define PUSH_H

#include <QDialog>

#include "branch.h"
#include "remoting.h"

namespace Ui {
class Push;
}

class Push : public Remoting
{
    Q_OBJECT

public:
    explicit Push(QString sciezkaGita, QWidget *parent = 0);
    ~Push();

private:
    Ui::Push *ui;
    virtual void wyswietlZwrot(QByteArray zwrot);  /* wyswietla zwrot od git'a */
    virtual void otworz();                  /* wywoluje git push, ktory "pcha" zmiany na serwer */
//    Branch * brancz;
    void zczytajBranche(QByteArray zwrot);          /* wywoluje polecenie branch i zczytuje branche do comboBoxBranch */
    void zczytajRepo(QByteArray zwrot);             /* wywoluje polecenie remote i zczytuje branche do comboBoxRemote  */
    int wyswietlnikZwrotu; /* gdy bedzie rowny 2, wtedy dopiero bedzie mozna wyswietlic zwrot bo wczesniej to git branch i remote a tego nie */

private slots:
    void pull();           /* polecenie pull git'a - pobiera wszystko z serwera do lokalnego repo */
};

#endif // PUSH_H
