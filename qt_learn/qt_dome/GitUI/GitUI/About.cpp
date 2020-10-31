#include "About.h"
#include "ui_about.h"

About::About(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::About)
{
    ui->setupUi(this);
    setAuthorInfo();
    setProgramInfo();
    connect(ui->pushButtonBack,SIGNAL(clicked()),this,SLOT(close()));
}

About::~About()
{
    delete ui;
}

void About::setAuthorInfo()
{
    ui->labelAuthor->setText("Author: Przemysław Thomann,PolSl AEI INF sem.4 gr.5");
    ui->labelEmail->setText("E-mail: peerzet3@gmail.com");
}

void About::setProgramInfo()
{
    ui->labelProgramName->setText("GitUI - user friendly (or not) interface for GIT");
    ui->labelVersion->setText("version 1.0");
}
