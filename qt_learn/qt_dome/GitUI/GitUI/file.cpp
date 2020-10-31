#include <string>
#include <iostream>
#include <QAbstractItemModel>

#include "file.h"
#include "ui_file.h"
#include "mainwindow.h"

File::File(QString sciezkaGita,QWidget *parent)
    : Repository(parent)//   DOBRE DZIEDZICZY TERAZ ! ten zostaw jak je a zrob kolejny konstruktor
    , ui(new Ui::File)
    , sciezkaGita(sciezkaGita)
{
    ui->setupUi(this);
    Repository * rodzic = this;       //polimorfizm - wywołanie metody wirtualnej ;]
    rodzic->drzewiakPlikow();
    ui->labelGit->setText("Your " + sciezkaGita + " repository");  //wyswietlam sciezke w jakiej sie znajduje
    proces = new QProcess(this);      //tworze obiekt procesu i go ustawiam odpowiednio
    proces->setReadChannel(QProcess::StandardOutput);
    proces->setProcessChannelMode(QProcess::MergedChannels);
    proces->start(status(sciezkaGita));        //od razu wyswietlam status repo jakie wczesniej otworzylem
    connect(proces, SIGNAL(readyRead()), this, SLOT(zczytajZwrot())); //wyswietlam je w wyswietlaczu
    connect(ui->pushButtonAdd, SIGNAL(clicked()), this, SLOT(dodaj()));
    connect(ui->pushButtonCommit, SIGNAL(clicked()), this, SLOT(commit()));
    connect(ui->pushButtonCancel, SIGNAL(clicked()), this, SLOT(close()));
}

File::~File()
{
    delete ui;
}

void File::drzewiakPlikow()
{
    model = new QFileSystemModel();
    model->setRootPath(QDir::currentPath());
    this->ui->wyswietlaczDrzewiak->setModel(model);
    model->setFilter(QDir::Hidden| QDir::AllDirs |QDir::AllEntries|QDir::NoDotDot | QDir::NoDotAndDotDot);  //opcje pozwalajce zobaczyc katalog .git
    ui->wyswietlaczDrzewiak->setSelectionMode(QAbstractItemView::ExtendedSelection);  //tego sZUkaj
    this->ui->wyswietlaczDrzewiak->setRootIndex(model->index(QDir::rootPath()));
    this->ui->wyswietlaczDrzewiak->setColumnHidden( 1, true ); //nie wiem jak to działa, ale dzieki temu widze wszystkie pliki w przegladarce :D
    this->ui->wyswietlaczDrzewiak->setColumnHidden( 2, true );
    this->ui->wyswietlaczDrzewiak->setColumnHidden( 3, true );
}

void File:: zczytajZwrot()
{
    QByteArray bytes = proces->readAllStandardOutput();
    wyswietlZwrot(bytes);
}

void File::wyswietlZwrot(QString zwrot)
{
    ui->wyswietlacz->clear();
    ui->wyswietlacz->setText(zwrot);
    ((MainWindow*)this->parentWidget())->sprawdzPierwszyCommit();
}

void File::dodaj()
{
    proces->close();
    QModelIndexList lista = ui->wyswietlaczDrzewiak->selectionModel()->selectedIndexes();   //tworze liste zaznaczonych indexow

    if(!lista.isEmpty())
    {
        //sprawdzam czy zaznaczono jakikolwiek indeks
        proces->execute(add(sciezkaGita,sciezkaWorkTree(model->filePath(lista[0])),model->filePath(lista[0])));
        proces->close();    //nie mam pojecia czemu execute dziala a start nie -.- ale ok...
        int i = 4;

        while(i<lista.length())
        {
            //dlatego wyswietlam co 4 element, troche strata pamieci ale nie wiem czemu tak je -.-
            proces->execute(add(sciezkaGita,sciezkaWorkTree(model->filePath(lista[0])),model->filePath(lista[i])));
            i += 4;
        }
        proces->start(status(sciezkaGita));
    }
}

QString File:: sciezkaWorkTree(QString sciezkaPliku)
{
    while(!sciezkaPliku.endsWith("/",Qt::CaseInsensitive))
    {
        //usuwam po 1 znaku tak dlugo az znajde / czyli koniec nazwy pliku
        sciezkaPliku.remove(sciezkaPliku.length()-1,1);
    }

    return sciezkaPliku;    //zwracam nazwe folderu
}

void File:: commit()
{
    proces->close();
    proces->start(git::commit(sciezkaGita));
}
