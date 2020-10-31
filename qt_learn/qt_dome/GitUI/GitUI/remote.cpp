#include "remote.h"
#include "ui_remoting.h"
Remote::Remote(QString sciezkaGit,QWidget *parent)
    : Remoting(sciezkaGit,parent)
    , flagaPoleceniaRemote(true)
{
    ui->setupUi(this);
    ui->wyswietlacz->setText("No tracked repositories found.");
    ui->radioButton->close();
    ui->pushButton->setText("Remote");     //modyfikuje widoczne nazwy przyciskow itp.
    ui->pushButton_2->setText("Add repo");
    ui->pushButton_3->setText("Delete");
    ui->labelOperacja->setText("Manage set of\n tracked repositories");
    ui->labelGit->setText("Your repositories in "+sciezkaGita);
    ui->label->setText("Name:");
    ui->label_2->setText("URL:");

    proces->setProcessChannelMode(QProcess::MergedChannels);
    proces->start(remote(sciezkaGita));

    connect(proces,SIGNAL(readyRead()), this, SLOT(zczytajZwrot()));
    connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(otworz()));
    connect(ui->pushButton_3,SIGNAL(clicked()),this,SLOT(usunRepo()));
    connect(ui->pushButton_2,SIGNAL(clicked()),this,SLOT(dodajRepo()));
    connect(ui->pushButtonBack,SIGNAL(clicked()),this,SLOT(close()));
}

Remote::~Remote()
{

}

void Remote::wyswietlZwrot(QByteArray zwrot)
{
    ui->wyswietlacz->clear();
    ui->wyswietlacz->setText(zwrot);

    if(flagaPoleceniaRemote==true)
    {
        skanujRepo(zwrot);
    }
}

void Remote::otworz()
{
    proces->close();
    proces->start(remoteV(sciezkaGita));
    flagaPoleceniaRemote=false;     //niech nie aktualizuje comboBoxa bo go wypelnia program na bieżąco, nie trzeba za kazdym razem uaktualniac
}

void Remote::skanujRepo(QByteArray zwrot)
{
    int i;
    QString repo = "";       //nazwa zdalnego repozytorium
    for(i = 0; i < zwrot.length()-1; i++)
    {
        while(zwrot[i] != '\n' && zwrot[i] != ' ')
        {
            //dopoki nie ma ani tego ani tego, to wyrazenie jest prawda i robi
            repo += zwrot[i];                         //jak tylko ktores sie zmieni to przestaje  bo 1*0 = 0
            ++i;
        }
        ui->comboBox->addItem(repo);
        repo = "";
    }
}

void Remote::usunRepo()
{
    if(ui->comboBox->count()>0)
    {
        //jezeli jest w comboBoxie przynajmniej jeden element to go usune
        QString nazwaRepo = ui->comboBox->currentText();
        int indeks = ui->comboBox->currentIndex();
        proces->close();
        proces->execute(remoteRemove(sciezkaGita,nazwaRepo));
        ui->comboBox->removeItem(indeks);
        otworz();
    }

    if(ui->comboBox->count()<=1)  //jesli jest 1 lub 0 to usun ale/lub wyswietl ze nie ma wiecej
    {
        ui->wyswietlacz->setText("No tracked repositories found");
    }
}

void Remote::dodajRepo()
{
    bool flaga = true;
    if(ui->lineEdit->text()!="" && ui->lineEdit_2->text()!="")
    {
        //jesli oba lineEdity zawieraja dane
        if(ui->lineEdit->text().contains(" ")|| ui->lineEdit_2->text().contains(" "))   //nazwa nie moze zawierac spacji
        {
            ui->wyswietlacz->append("The name or URL can not contain space character!");
        }
        else
        {
            for(int i = 0; i < ui->comboBox->count(); i++)
            {
                if(ui->comboBox->itemText(i) == ui->lineEdit->text())
                {        //jesli lineEdit= comboBox czyli istnieje takie repo z ta nazwa
                    flaga = false;                                            //to nie executuj remoteAdd
                    ui->wyswietlacz->append("The repository with the given name already exists.");
                }
            }

            if(flaga)
            {
                proces->close();
                proces->execute(remoteAdd(sciezkaGita,ui->lineEdit->text(),ui->lineEdit_2->text()));
                ui->comboBox->addItem(ui->lineEdit->text());
                otworz();
            }
        }
    }
    else
    {
        //jesli puste pola, daj info
        ui->wyswietlacz->append("Insert the repository name and URL.");
    }
}
