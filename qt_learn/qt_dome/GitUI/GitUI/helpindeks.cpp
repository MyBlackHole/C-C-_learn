#include <QUrl>
#include <QDir>
#include <QFile>
#include <QTextStream>

#include "helpindeks.h"
#include "ui_helpindeks.h"

HelpIndeks::HelpIndeks(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::HelpIndeks)
{
    ui->setupUi(this);
    ui->webView->load(QUrl("file://" + okreslAktualnaSciezke() + "/HelpIndeksGitUi.html"));
    connect(ui->pushButtonBack,SIGNAL(clicked()),this,SLOT(close()));
}

HelpIndeks::~HelpIndeks()
{
    delete ui;
}

QString HelpIndeks::okreslAktualnaSciezke()
{
    return QDir::currentPath();
}
