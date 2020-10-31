#ifndef BRANCH_H
#define BRANCH_H

#include <QDialog>
#include <QProcess>

#include "git.h"

namespace Ui {
class Branch;
}

class Branch : public QDialog, public git
{
    Q_OBJECT

public:
    explicit Branch(QString sciezka,QWidget *parent = 0);
    ~Branch();

    void zczytajBranche(QByteArray zwrot);      //funkcja z polecenia branch , tworzy comboBoxa gdzie mozna wybrac poszczegolna galaz :D

private:
    Ui::Branch *ui;
    QString sciezkaGita;                /* sciezka do folderu .git */
    QProcess * proces;                  /* proces git */
    void wyswietlZwrot(QString zwrot);  /* wyswietla zwrot odebrany od programu */
    bool flagaPoleceniaBranch;          /* bedzie informowac, kiedy wybrano polecenie branch, zeby segregowac elementy w comboBox */
    bool hideOrShow;                /* w zaleznosci od wartosci albo pokaze elementy edycji albo nie;] bede go zmienial za kazdym kliknieciem */
    QString lokalnaGalaz;           /* przy aktualizowaniu pozostalych branchow pomocne, zeby nie dodawac do usuwanych tego na ktorym jest */

private slots:
    void zczytajZwrot();            /* zczytuje zwrot od procesu proces */
    void zmianaBrancha();           /* slot obslugujacy zmiane brancha w comboBoxie ;] */
    void dodajBranch();             /* slot na dodanie nowego brancha   */
    void showHideElements();        /* po kliknieciu w radioButtona odkrywa elementy badz chowa w zaleznosci od hideOrShow(true=pokaz, false=schowaj)*/
    void usunBranch();              /* usuwa branch z ComboBoxa i z repo git'a */
    void aktualizujBrancheCombo2();     /* bedzie dodawal do comboboxa2 galezie ktore mozna usunac, czyli wszystkie procz *glownej */
    void merge();           /* robi merge, laczy galezie */
    void checkoutToHash();  /* robi checkout do danej migawki na podstawie hashu SHA1 */
};

#endif // BRANCH_H
