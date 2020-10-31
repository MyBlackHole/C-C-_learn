#ifndef USERSETTINGS_H
#define USERSETTINGS_H

#include <QDialog>
#include <QProcess>

#include "git.h"

namespace Ui {
class UserSettings;
}

class UserSettings : public QDialog,public git
{
    Q_OBJECT

public:
    explicit UserSettings(QString sciezkaGita,QWidget *parent = 0);
    ~UserSettings();

    void checkDefaultRepository();/* sprawdza czy plik ma zawartosc , jesli tak ustawia to jako domyslnne repo */

private:
    QString sciezkaGita;    /* sciezka do lokalnego repo git'a */
    QProcess * proces;      /* proces git */
    Ui::UserSettings *ui;
    bool flagaConfigu;      /* informuje kiedy wywolano funkcje checkConfig, ktora sprawdza informacje o developerze, gicie itp */
    void readConfig(QByteArray zwrot);  /* interpretuje zwrot od git config --list na podstawie ktorego wypelnia pola */

private slots:
    void changeDefaultRepository(); /* ustawia domyslne repozytorium na ktorym bedzie sie pracowac, nie trzeba go ciagle otwierac */
    void setNoDefaultRepository();  /* czysci plik zawierajace sciezke domyslnego repo, dzieki czemu nie bedzie go - nie ma domyslnego repo */
    void checkConfig();         /* uruchamia git config --list i przez co dalej sa zczytywane wszystkie najwazniejsze ustawienia git */
    void zczytajZwrot();        /* zczytuje zwrot od git'a */
    void changeUserName();      /* zmienia globalna nazwe user'a gita , na ta z lineEditUserChange */
    void changeUserEmail();      /* zmienia globalna email gita , na ta z lineEditUserEmail */
    void changeEditor();        /* zmienia globalny edytor do commitow */
};

#endif // USERSETTINGS_H
