#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <QLabel>
#include <QDialog>
#include <QFileSystemModel>

#include "git.h"

class MainWindow;

namespace Ui {
class Repository;
}
class Repository : public QDialog,public git//, public Iinterface
{
    Q_OBJECT

public:
    Repository(QString opcja,MainWindow *parent = 0); /*dostepne opcje: "otworz","stworz","usun" */
    Repository(QWidget * parent);
    virtual ~Repository();
    virtual void drzewiakPlikow();    /* funkcja wyswietla drzewo plikow w TreeList */

protected:
    QString opcja;              /* informuje o wybranej opcji otwracia okna Repozytorium */
    QFileSystemModel * model;   /* zmienna do tworzenia drzewa plików */
    QProcess * proces;          /* wywolywany proces - git */

private:
    Ui::Repository *ui;
    void wyswietlOstrzezenie(); /* wyswietla ostrzezenie dotyczace usuniecia repozytorium */
    QDialog *okno;          /* 4 ponizsze wskazniki wykorzystywane do tworzenia okienka wyswietlajace ostrzezenie usuniecia repozytorium */
    QLabel * tekscik;
    QPushButton * tak;
    QPushButton * zamknij;

private slots:
    void otworz();       /* otwiera podana sciezke do repo GITa */
    void stworz();  /* tworzy nowe repozytorium git w folderze za pomoca .git init */
    void usun();    /* umozliwa usuniecie folderu .git za pomoca odpowiednich connect'ow */
    void usunFolder();      /* bezposrednio usuwa folder .git */
    void zczytajZwrot();       /* slot zczytuje zwrot od procesu */
    void wyswietlZwrot(QString zwroty);  /* wysyla zwrot do rodzica, u nas do MainWindow */
};

#endif // REPOSITORY_H
