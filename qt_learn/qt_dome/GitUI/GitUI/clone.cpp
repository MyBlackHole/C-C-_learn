#include "clone.h"
#include "ui_clone.h"
#include "QThread"
Clone::Clone(QString sciezkaGit,QWidget *parent)
    : Remoting(sciezkaGit,parent)
    , ui(new Ui::Clone)
    , flagaOwnNaming(true)
{
    ui->setupUi(this);
    drzewiakPlikow();
    ui->wyswietlacz->setText("Here you can clone repository into the new directory. Select destination folder, from work tree where you want"
                             " to save cloning repository. Paste the URL of repo you want to clone. If you want to select name of folder"
                             " you are going to save the repository - select option Own naming.\n");
    ui->labelOperacja->setText("Clone repository \n into the new directory");
    ui->labelURL->setText("URL:");
    ui->radioButton->setText("Own naming");
    ui->label_2Nazwa->setText("Insert the new repository name:");
    ui->lineEdit_2Nazwa->hide();
    ui->pushButton->setText("Clone");
    ui->label_2Nazwa->hide();
    connect(this->ui->radioButton, SIGNAL(toggled(bool)), this, SLOT(zmianaRadioButton()));
    connect(proces, SIGNAL(readyRead()), this, SLOT(zczytajZwrot()));
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(otworz()));
    connect(ui->pushButton_2Back, SIGNAL(clicked()), this, SLOT(close()));
}

Clone::~Clone(){
    delete model;
    delete ui;
}

void Clone::otworz()
{
    proces->close();
    QModelIndexList index = ui->wyswietlaczDrzewiak->selectionModel()->selectedIndexes();   //wybieram index gdzie stworzyc katalog
    bool flag = false;     //informuje czy przy wyborze wlasnego nazewnictwa, lineEdit2 zawiera nazwe galezi wpisana

    if(!flagaOwnNaming)
    {
        flag=true;
    }

    if(ui->lineEditURL->text() != "" && (ui->lineEditURL->text().contains(" ")!=true))
    {
        if(index.isEmpty() == false)
        {
            QString sciezkaZapisu; //gdzie bedzie zapisane zklonowane repozytorium

            if(model->isDir(ui->wyswietlaczDrzewiak->currentIndex()))
            {
                //sprawdzam czy wybrano folder jako sciezke
                sciezkaZapisu+=model->filePath(ui->wyswietlaczDrzewiak->currentIndex());        //mam sciezke do folderu teraz jeszcze nazwa

                if(!flagaOwnNaming)
                {
                    //jesli wybrano wlasne nazewnictwo
                    if(ui->lineEdit_2Nazwa->text() != "")
                    {
                        flag = true;
                        sciezkaZapisu += "/"+ui->lineEdit_2Nazwa->text();
                    }
                    else
                    {
                        flag = false;
                        ui->wyswietlacz->append("Insert own name of destination folder.");
                    }
                }
                else
                {
                    sciezkaZapisu += "/" + wyznaczNazwe(ui->lineEditURL->text());
                    flag = true;
                }

                if(flag)
                {
                    proces->setProcessChannelMode(QProcess::MergedChannels);
                    proces->start(clone(ui->lineEditURL->text(),sciezkaZapisu));           //dziala
                    ostrzezenie();     //jesli nie wyswietl info ze cos jest nie tak, git to tez robi ale na wszelki wypadek ;/ to wyswietlam zawsze
                    //jezeli bedzie zwrot od gita, to i tak wyczysci caly ekran i to zniknie :D
                }
            }
            else
            {
                ui->wyswietlacz->append("<-You must select folder!");
            }
        }
        else
        {
            ui->wyswietlacz->append(" <- Select destination folder for saving repository from work tree");
        }
    }
    else
    {
        if((ui->lineEditURL->text().contains(" ")))
        {
            ui->wyswietlacz->append("The URL contains space!");
        }
        else
        {
            ui->wyswietlacz->append("Insert the URL of repository you want to clone into a new directory.");
        }
    }
}

QString Clone::wyznaczNazwe(QString sciezka)
{
    QString ciag;
    ciag = sciezka;

    if(sciezka.endsWith("/"))
    {
        while(ciag.endsWith("/"))
        {
            //gdyby bylo wiecej // na koncu sciezki , nie wiem po co no ale takie zabezpiecznie
            ciag.remove(ciag.length()-1,1);
        }

    }

    if(ciag.endsWith(".git"))
    {
        ciag.remove(ciag.length()-4,4);         //usuwam ".git"
    }

    int i = ciag.length();
    QString ciagPomocniczy = ciag;

    while(i>0)
    {
        //teraz szukamy "/" zeby wiedziec jaka nazwa folderu bo bedzie /nasza_szukana_nazwa a my chcemy nasza_szukana_nazwa
        if(ciagPomocniczy.endsWith("/"))
        {
            break;
        }
        else
        {
            i--;
            ciagPomocniczy.remove(i,1);         //usuwam po literce, az dojde do /
        }
    }

    ciag.remove(0,i);       //usuwam wszystko przed / i zostaje mi sama nazwa folderu
    return ciag;
}

void Clone::wyswietlZwrot(QByteArray zwrot)
{
    ui->wyswietlacz->setText(zwrot);
}

void Clone::zmianaRadioButton()
{
    if(flagaOwnNaming)
    {
        ui->lineEdit_2Nazwa->show();
        ui->label_2Nazwa->show();
        flagaOwnNaming = false;
    }
    else
    {
        ui->lineEdit_2Nazwa->hide();
        ui->label_2Nazwa->hide();
        flagaOwnNaming = true;
    }
}

void Clone::drzewiakPlikow()
{
    model = new QFileSystemModel;
    model->setRootPath(QDir::currentPath());
    this->ui->wyswietlaczDrzewiak->setModel(model);
    model->setFilter(QDir::Hidden| QDir::AllDirs |QDir::AllEntries|QDir::NoDotDot | QDir::NoDotAndDotDot);  //opcje pozwalajce zobaczyc katalog .git
    this->ui->wyswietlaczDrzewiak->setRootIndex(model->index(QDir::rootPath()));
    this->ui->wyswietlaczDrzewiak->setColumnHidden(1, true); //nie wiem jak to działa, ale dzieki temu widze wszystkie pliki w przegladarce :D
    this->ui->wyswietlaczDrzewiak->setColumnHidden(2, true);
    this->ui->wyswietlaczDrzewiak->setColumnHidden(3, true);
}

void Clone::ostrzezenie()
{
    ui->wyswietlacz->append("Something is missing. Check all fields, your permissions for saving or if the folder already exists");
}
