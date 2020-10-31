#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <iostream>
#include <QProcess>
#include <QMainWindow>

#include "git.h"
#include "file.h"
#include "clone.h"
#include "About.h"
#include "branch.h"
#include "remote.h"
#include "remoting.h"
#include "repository.h"
#include "helpindeks.h"
#include "usersettings.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow, public git
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    virtual  ~MainWindow();
    QString sciezkaGita;    /* sciezka do folderu .git, zawierajacego repozytorium na ktorym operujemy */
    void sprawdzPierwszyCommit();   /* sprawdza czy pierwszy commmit wykonany, tj uruchamia git log i sprawdza jaki zwrot */
    void ustawLabelGit(bool flaga);   /* ustawia labelGit ze sciezka do repozytorium gdy flaga=true lub usuwa label gdy flaga=false */

public slots:
    void wyswietl(QString tekst);   /* wyswietla w wyswietlaczu podany tekst */

private:
    Ui::MainWindow *ui;
    QProcess * proces;  /* proces git */
    Repository * repozytorium;  /* wskaznik na obiekt tworzący okienko tworzenia/otwiernia/usuwania Repozytorium */
    File * plik;     /* wskaznik na obiekt tworzący okienko do dodawania/usuwania plikow z Repozytorium */
    Branch * brancz;    /* wskaznik na obiekt tworzący okienko do zarzadzania galeziami */
    Remoting * remot;   /* wskaznik na obiekt tworzący okienko do operacji sieciowych, wykorzystywany do fetch,clone,push */
    UserSettings * user_settings;   /* wskaznik na obiekt tworzący  okienko do ustawien: git'a, uzytkownika git'a itp. */
    HelpIndeks * help_indeks;
    About * about;      /* okienko z informacjami o autorze, wersji itp */
    bool flagaPierwszegoCommitu;    /* informuje o tym czy jest galaz master, ktora pojawia sie po pierwszym commicie i jest potrzebna */
    bool flagaSprawdzania;  /* informuje o tym kiedy sprawdzamy czy jest galaz master, i zeby nie wyswietlac zwrotu */
    bool flagaSprawdzeniaIstnieniaGita;
    bool flagaWyswietlania; /* zeby nie wyswietlil zwrotu gdy sprawdzam czy istnieje git, na poczatku */
    //    bool flagaIstnieniaGita; /* sprawdza czy istnieje git zainstlowany na komputerze */
    void weryfikujPierwszyCommit(QString zwrot); /*na podstawie zwrotu git branch weryfikuje czy byl juz pierwszy commit , jest wtedy * */
    void wyswietlZwrot(QString zwrot);  /* wyswietla zwrot od programu procesu proces */

private slots:
    void otworzRepozytorium();  /* otwiera okienko repozytorium z podanymi parametrami OTWORZ */
    void stworzRepozytorium();  /* otwiera okienko repozytorium z podanymi parametrami STWORZ */
    void usunRepozytorium();    /* otwiera okienko repozytorium z podanymi parametrami USUN */

    void dodajPlik();   /* otwiera okienko pozwalajce na dodawanie plikow do commitu */

    void branch();          /* otwiera okienko umozliwiajace dzialania na galeziach */

    void remote();          /* otwiera okineko umozliwiajace modyfikacje zdalnych repozytoriow */
    void fetch();            /* otwiera okienko umozliwiajace*/
    void clone();           /* umozliwa otwarcie okienka do klonowania repozytoriow */
    void push();            /* opcja do "wciskania" swoich zmian na zdalny serwer */

    void userSettings();    /* otwiera okienko pozawalajace na zmiane ustawien */

    void helpIndeks();
    void aboutProgram();    /* wyswietla okienko z informacji o programie i autorze */
    void commit();          /* commit dla lokalnego repo */
    void status();          /* git status na lokalnym repo */
    void log();             /* git log dla lokalnego repo */
    void zczytajZwrot();    /* zczytuje zwrot z programu */

    void sprawdzCzyIstniejeGit();   /* uruchamia program git, potem weryfikujIstnienieGita sprawdzi na podstawie zwrotu czy on jest zainstalowany*/
    void weryfikujIstnienieGita(QString zwrot); /* na podstawie zwrotu weryfikuje czy istnieje git w komputerze */
//    void checkIfGitExists();    /* sprawdza czy git jest w ogole zainstlowany na komputerze */
//    void weryfikujIstnienieGita(QString zwrot);
//    void ustawWszystko();   /* funkcja ustawia wszystkie connecty i wsio, wywolywana przez weryfikujIstnienieGita, jesli on oczywiscie zainstalowany*/
};

#endif // MAINWINDOW_H
