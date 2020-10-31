#ifndef GIT_H
#define GIT_H

#include <QString>
#include <QProcess>

/* POLECENIA GIT'A WYKORZYSTANE W GitUI */
class git
{
public:
    git();
    ~git();
    QString init(QString sciezka); //inicjuje repozytorium git'a w podanej lokalizacji
    QString status(QString sciezkaGita); //  git status
    QString add(QString sciezkaGita, QString sciezkaWorkTree,QString sciezkaPliku);  //dodanie pliku do śledzonych przez repo
    QString commit(QString sciezkaGita); // git commit, zatwierdzenie zmian
    QString log(QString sciezkaGita);    //git log, sprawdzenie aktualnego stanu śledzonych i nie śledzonych plików
    QString branch(QString sciezkaGita); // wyswietla info o galeziach
    QString checkout(QString sciezkaGita,QString nazwaBrancha);  //przeskok na gałąź
    QString addBranch(QString sciezkaGita,QString nazwaBrancha);//dodaje branch
    QString deleteBranch(QString sciezkaGita,QString nazwaBrancha);//usuwa branch
    QString remote(QString sciezkaGita);//wyswietla info o zdalnych repo
    QString remoteV(QString sciezkaGita);        //rozszerzona opcja remote -V
    QString remoteRemove(QString sciezkaGita, QString nazwaRepo);    //usuwa zdalne repo
    QString remoteAdd(QString sciezkaGita, QString nazwaRepo, QString URL);  //dodaje nowe zdalne repo
    QString clone(QString URL,QString nazwa);    //klonuje repo w nowe
    QString push(QString sciezkaGita,QString nazwaRemote,QString nazwaBranch);   //wciska zmiany na zdalne repo
    QString fetch(QString sciezkaGita,QString nazwaRemote);  //uaktualnia nasze repo ze zdalnym, nie merguje
    QString pull(QString sciezkaGita,QString nazwaRemote);   //uaktualnia nasze repo ze zdalnym, merguje
    QString branchRemote(QString sciezkaGita);   //branche repo zdalnego
    QString merge(QString sciezkaGita,QString branchToMerge);// scala zmiany z jednej gałezi z inna
    QString config(QString sciezkaGita); //ustawienia
    QString configUserName(QString sciezkaGita,QString userName);
    QString configUserEmail(QString sciezkaGita,QString userEmail);
    QString configChangeEditor(QString sciezkaGita,QString edytor);
    QString checkGit();   /* sprawdza gita , weryfikujac czy jest zainstalowany i dostepny */
};
#endif // GIT_H
