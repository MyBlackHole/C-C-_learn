#include <iostream>
#include <QPushButton>

#include "repository.h"
#include "mainwindow.h"
#include "ui_repository.h"

Repository::Repository(QString opcja, MainWindow *parent)
    : QDialog(parent)
    , ui(new Ui::Repository)
    , opcja(opcja)
    , model(NULL)
    , proces(NULL)
{
    ui->setupUi(this);
    /* W zaleznosci od wybranej opcji odpowiednie funkcjonalnosci */
    if(opcja == "otworz")
    {
        ui->labelOperacja->setText("Opening the\n repository...");
        ui->pushButtonWybierz->setText("Open");
        connect(ui->pushButtonWybierz,SIGNAL(clicked()),this,SLOT(otworz()));
    }

    if(opcja == "stworz")
    {
        ui->labelOperacja->setText("Creating new\n repository...");
        ui->pushButtonWybierz->setText("Create");
        connect(ui->pushButtonWybierz,SIGNAL(clicked()),this,SLOT(stworz()));
    }

    if(opcja == "usun")
    {
        ui->labelOperacja->setText("Deleting the \n repository...");
        ui->pushButtonWybierz->setText("Delete");
        connect(ui->pushButtonWybierz,SIGNAL(clicked()),this,SLOT(usun()));
    }

    drzewiakPlikow();   //wyswietlam drzewo plikow w oknie

    connect(ui->pushButtonAnuluj,SIGNAL(clicked()),this,SLOT(close()));
}

Repository::Repository(QWidget * parent)
    : QDialog(parent)
    , model(NULL)
    , proces(NULL)
{

}

Repository::~Repository()
{
    delete ui;
    delete model;

    if(proces->isOpen())
    {
        proces->close();
    }

    delete proces;
    delete okno;
    delete tekscik;
    delete tak;
    delete zamknij;
}

void Repository::drzewiakPlikow()
{
    model = new QFileSystemModel;
    model->setRootPath(QDir::currentPath());
    this->ui->wyswietlaczDrzewiak->setModel(model);
    model->setFilter(QDir::Hidden| QDir::AllDirs |QDir::AllEntries|QDir::NoDotDot | QDir::NoDotAndDotDot);  //opcje pozwalajce zobaczyc katalog .git
    this->ui->wyswietlaczDrzewiak->setRootIndex(model->index(QDir::rootPath()));
    this->ui->wyswietlaczDrzewiak->setColumnHidden( 1, true ); //nie wiem jak to działa, ale dzieki temu widze wszystkie pliki w przegladarce :D
    this->ui->wyswietlaczDrzewiak->setColumnHidden( 2, true );
    this->ui->wyswietlaczDrzewiak->setColumnHidden( 3, true );
}

void Repository::otworz()
{
    QString sciezka = model->filePath(ui->wyswietlaczDrzewiak->currentIndex());
    if(sciezka.endsWith(".git",Qt::CaseInsensitive)==true)
    {
        //sprawdzam czy sciezka konczy się znakami ".git"
        ((MainWindow*)this->parentWidget())->sciezkaGita = sciezka; //rodzic tj. MainWindow dostaje sciezke do folderu .git na ktorym pracujemy
        ((MainWindow*)this->parentWidget())->sprawdzPierwszyCommit();
        ((MainWindow*)this->parentWidget())->ustawLabelGit(true);
        this->close();
    }
    else
    {
        ui->labelInformacja->setText("Unfortunately the bad folder...\nSelect.git folder!");
    }

}

void Repository::stworz()
{
    if(model->isDir(ui->wyswietlaczDrzewiak->currentIndex()))
    {
        //sprawdzam czy tworze w folderze a nie w jakims pliku
        QString sciezka = model->filePath(this->ui->wyswietlaczDrzewiak->currentIndex());
        proces = new QProcess(this);
        proces->setReadChannel(QProcess::StandardOutput);
        proces->start(init(sciezka));
        ((MainWindow*)this->parentWidget())->sciezkaGita=(sciezka+"/.git"); //rodzic tj. MainWindow dostaje sciezke do folderu .git na ktorym pracujemy
        ((MainWindow*)this->parentWidget())->sprawdzPierwszyCommit();
        connect(proces, SIGNAL(readyReadStandardOutput()), this, SLOT(zczytajZwrot()));
        ((MainWindow*)this->parentWidget())->ustawLabelGit(true);
        this->close();
    }
    else
    {
        ui->labelInformacja->setText("Sorry, but you can\n create only in folder!");
    }
}

void Repository::usun()
{
    wyswietlOstrzezenie();
}

void Repository:: usunFolder()
{
    QString sciezka = model->filePath(this->ui->wyswietlaczDrzewiak->currentIndex());
    if(sciezka.endsWith(".git",Qt::CaseInsensitive))
    {
        //sprawdzam czy tworze w folderze a nie w jakims pliku
        proces = new QProcess(this);
        proces->setReadChannel(QProcess::StandardOutput);
        proces->start("rm -r " + sciezka);

        if(((MainWindow*)this->parentWidget())->sciezkaGita == sciezka)
        {
            //zeby glowne okno miala pusta sciezke, jesli usuwam wlasnie ta
            ((MainWindow*)this->parentWidget())->sciezkaGita = "";
            ((MainWindow*)this->parentWidget())->ustawLabelGit(false);
        }
        ((MainWindow*)this->parentWidget())->sprawdzPierwszyCommit();
        wyswietlZwrot("Repository was deleted from: "+sciezka+" .");
        this->close();
    }
    else
    {
        ui->labelInformacja->setText("Unfortunately the bad folder...\nSelect.git folder!");
    }
}

void Repository:: zczytajZwrot()
{
    QByteArray bytes = proces->readAllStandardOutput();
    wyswietlZwrot(bytes);
}

void Repository:: wyswietlZwrot(QString zwrot)
{
    ((MainWindow*)this->parentWidget())->wyswietl(QString(zwrot));  //wysylam rodzicowi zwrot od git'a ktory on wyswietli w glownym oknie
}

void Repository:: wyswietlOstrzezenie()
{
    okno = new QDialog(this);
    okno->setGeometry(1000,1000,450,100);
    okno->show();

    tekscik = new QLabel("              Are you sure to delete the selected repository?",okno);
    tekscik->setGeometry(20,1,500,40);
    tekscik->show();

    tak = new QPushButton("Tak",okno);
    tak->setGeometry(40,50,80,25);
    tak->show();

    zamknij = new QPushButton("Zamknij",okno);
    zamknij->setGeometry(330,50,80,25);
    zamknij->show();
    connect(zamknij,SIGNAL(clicked()),okno,SLOT(close()));
    connect(tak,SIGNAL(clicked()),this,SLOT(usunFolder()));
    connect(tak,SIGNAL(clicked()),okno,SLOT(close()));
}

