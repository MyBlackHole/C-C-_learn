#ifndef FILE_H
#define FILE_H

#include <QDialog>
#include <QProcess>
#include "repository.h"
#include "QFileSystemModel"

namespace Ui {
    class File;
}

class File : public Repository
{
    Q_OBJECT

public:
    explicit File(QString sciezkaGita, QWidget *parent = 0); /* przekazujemy sciezke do repozytorium ".git" */
    ~File();

private:
    Ui::File *ui;
    void drzewiakPlikow();          /* wyswietla drzewo plikow */
    void wyswietlZwrot(QString zwrot);  /* wyswietla zwrot z programu, u nas od git'a */
    QString sciezkaGita;        /* sciezka folderu git, ktory poprzednio otwarlismy i na ktorym pracujemy */
    QString sciezkaWorkTree(QString sciezkaPliku);  /* wylicza sciezke katalogu w ktorym jest folder, potrzebna ona jako opcja git'a work-tree */

private slots:
    void zczytajZwrot();        /* zczytuje zwrot z programu */
    void dodaj();               /* dodaje wybrany plik do sledzonych */
    void commit();              /* wyswietla commit */
};

#endif // FILE_H
