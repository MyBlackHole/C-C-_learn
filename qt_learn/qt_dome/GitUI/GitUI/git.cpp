#include <QProcess>
#include <QByteArray>

#include "git.h"

git::git()
{

}

git::~git()
{

}

QString  git::init(QString sciezka)
{
    return "git --git-dir="+sciezka+"/.git init";
}

QString git::status(QString sciezkaGita)
{
    return "git --git-dir="+sciezkaGita+" status";
}

QString git::add(QString sciezkaGita,QString sciezkaWorkTree, QString sciezkaPliku)
{
    return "git --work-tree="+sciezkaWorkTree+" --git-dir="+sciezkaGita+" add "+sciezkaPliku;
}

QString git::commit(QString sciezkaGita)
{
    return "git --git-dir="+sciezkaGita+" commit";
}

QString git::log(QString sciezkaGita)
{
    return "git --git-dir="+sciezkaGita+ " log ";
}

QString git::branch(QString sciezkaGita){
    return "git --git-dir="+sciezkaGita+ " branch ";
}

QString git:: checkout(QString sciezkaGita, QString nazwaBrancha)
{
    return "git --git-dir="+sciezkaGita+ " checkout "+nazwaBrancha;
}

QString git:: addBranch(QString sciezkaGita, QString nazwaBrancha)
{
    return "git --git-dir="+sciezkaGita+ " branch "+nazwaBrancha;
}
QString git:: deleteBranch(QString sciezkaGita,QString nazwaBrancha)
{
    return "git --git-dir="+sciezkaGita+ " branch -D "+nazwaBrancha;
}

QString git:: remote(QString sciezkaGita)
{
    return "git --git-dir="+sciezkaGita+" remote ";
}

QString git:: remoteV(QString sciezkaGita)
{
    return "git --git-dir="+sciezkaGita+" remote -v";
}

QString git:: remoteRemove(QString sciezkaGita, QString nazwaRepo)
{
    return "git --git-dir="+sciezkaGita+" remote remove "+nazwaRepo;
}

QString git:: remoteAdd(QString sciezkaGita, QString nazwaRepo,QString URL)
{
    return "git --git-dir="+sciezkaGita+" remote add "+nazwaRepo+" "+URL;
}

QString git::clone(QString URL,QString nazwa)
{
    return "git clone "+URL+" "+nazwa;
}

QString git::push(QString sciezkaGita,QString nazwaRemote,QString nazwaBranch)
{
    return "git --git-dir="+sciezkaGita+" push "+nazwaRemote+" "+nazwaBranch;
}

QString git::fetch(QString sciezkaGita, QString nazwaRemote)
{
    return "git --git-dir="+sciezkaGita+" fetch "+nazwaRemote;
}

QString git::pull(QString sciezkaGita, QString nazwaRemote)
{
    return "git --git-dir="+sciezkaGita+" pull "+nazwaRemote;
}

QString git::branchRemote(QString sciezkaGita)
{
    return "git --git-dir="+sciezkaGita+ " branch -r";
}
QString git::merge(QString sciezkaGita,QString branchToMerge)
{
    return "git --git-dir="+sciezkaGita+" merge "+branchToMerge;
}

QString git::config(QString sciezkaGita)
{
    return "git --git-dir="+sciezkaGita+" config --list";
}

QString git::configUserName(QString sciezkaGita,QString userName)
{
    return "git --git-dir="+sciezkaGita+" config --global --replace-all user.name \""+userName+"\"";
}

QString git::configUserEmail(QString sciezkaGita,QString userEmail)
{
    return "git --git-dir="+sciezkaGita+" config --global user.email "+userEmail;
}

QString git::configChangeEditor(QString sciezkaGita, QString edytor)
{
    return "git --git-dir="+sciezkaGita+" config --global core.editor "+edytor;
}

QString git::checkGit()
{
    return "git";
}
