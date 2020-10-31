#include "git.h"
#include "fetch.h"
#include "ui_fetch.h"

Fetch::Fetch(QString sciezkaGita,QWidget *parent)
    : Remoting(sciezkaGita,parent)
    , ui(new Ui::Fetch)
    , flagaPoczatku(true)
    , flagaBranchuRemoting(true)
{
    ui->setupUi(this);
    ui->labelGit->setText("Your repository in "+sciezkaGita);
    ui->pushButtonFetch->hide();//na poczatku wszystko schowane, gdy beda remote jakies dodane , wszystko sie bedzie odslaniac powoli ( w metodach jest)
    ui->comboBoxRemotes->hide();
    ui->comboBoxBranch->hide();
    ui->labelBranch->hide();
    ui->comboBox_2BranchRemote->hide();
    ui->label_2BranchRemote->hide();
    ui->pushButton_2Merge->hide();
    ui->wyswietlacz->setText("No remotes found..."); //poczatkowy tekst, gdy nie bedzie remote
    proces->start(git::remote(sciezkaGita));
    proces->setProcessChannelMode(QProcess::MergedChannels);                //dzieki temu dostaje wiecej zwrotu od GIT'a
    connect(proces, SIGNAL(readyRead()), this, SLOT(zczytajZwrot()));
    connect(ui->pushButtonFetch, SIGNAL(clicked()), this, SLOT(otworz()));
    connect(ui->pushButtonBack, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->pushButton_2Merge, SIGNAL(clicked()), this, SLOT(merge()));
}

Fetch::~Fetch()
{
    delete ui;
}

void Fetch::otworz()
{
    flagaPoczatku = false;
    ui->wyswietlacz->setText("Nothing changed. Haven't you the newest version?");
    proces->close();
    proces->start(git::fetch(sciezkaGita,ui->comboBoxRemotes->currentText())); //uruchamiam fetch z remote zczytanego z comboBoxa
}

void Fetch::wyswietlZwrot(QByteArray zwrot)
{
    if(flagaPoczatku)
    {
        if(flagaBranchRemot == 0)
        {
            zczytajRepo(zwrot);
        }
        else if(flagaBranchRemot == 1)
        {
            zczytajBranche(zwrot);
        }
        else if(flagaBranchRemot == 2 && flagaBranchuRemoting)
        {
            zczytajBrancheRemote(zwrot);
        }
        else if(flagaBranchRemot == 3)
        {
            ui->wyswietlacz->append(zwrot);
            flagaPoczatku = false;
        }
    }
    else
    {
        ui->wyswietlacz->append(zwrot);
        aktualizujBrancheRemote(zwrot);
    }
}

void Fetch::zczytajRepo(QByteArray zwrot)
{
    ui->pushButtonFetch->show();
    ui->comboBoxRemotes->show();
    ui->wyswietlacz->clear();
    int i;
    QString repo = "";       //nazwa zdalnego repozytorium
    for(i = 0; i < zwrot.length()-1; i++)
    {
        while(zwrot[i] != '\n' && zwrot[i] != ' ')
        {
            //dopoki nie ma ani tego ani tego, to wyrazenie jest prawda i robi
            repo += zwrot[i];                   //jak tylko ktores sie zmieni to przestaje  bo 1*0 = 0
            ++i;
        }
        ui->comboBoxRemotes->addItem(repo);
        flagaBranchRemot=1;
        repo = "";
    }

    proces->close();
    proces->start(branch(sciezkaGita));
}

void Fetch::zczytajBrancheRemote(QByteArray zwrot)
{
    int i;
    QString brancz = "";       //nazwa galezi
    for(i = 0; i < zwrot.length()-1; i++)
    {
        while(zwrot[i] != ' ' && zwrot[i] != '\n')
        {
            //dopoki nie ma ani tego ani tego, to wyrazenie jest prawda i robi
            brancz += zwrot[i];                   //jak tylko ktores sie zmieni to przestaje  bo 1*0 = 0
            ++i;
        }

        if(brancz != "" && brancz != " ")
        {
            bool flag = true;
            for(int i = 0; i<ui->comboBox_2BranchRemote->count(); i++)
            {
                if(ui->comboBox_2BranchRemote->itemText(i) == brancz)
                {
                    flag = false;
                }
            }

            if(flag)
            {
                ui->comboBox_2BranchRemote->addItem(brancz);
            }
        }

        brancz = "";
    }

    flagaBranchRemot = 3;
    ui->comboBox_2BranchRemote->show();
    ui->label_2BranchRemote->show();
    ui->pushButton_2Merge->show();
    ui->comboBoxBranch->show();
    ui->labelBranch->show();
}

void Fetch::zczytajBranche(QByteArray zwrot)
{
    int i;
    QString brancz = "";       //nazwa galezi
    int index = 0;             //bedzie wskazywac indeks galezi z *
    bool flag = false;
    int licznik = 0;      //liczy ile elementow dodalem
    for(i = 0; i < zwrot.length()-1; i++)
    {
        while(zwrot[i] != ' ' && zwrot[i] != '\n')
        {
            //dopoki nie ma ani tego ani tego, to wyrazenie jest prawda i robi
            brancz += zwrot[i];                   //jak tylko ktores sie zmieni to przestaje  bo 1*0 = 0
            int k = i;
            if(k < (zwrot.length()-1))
            {
                if(zwrot[k] != '*' && (zwrot[++k] == ' ' || zwrot[k] == '\n'))
                {
                    //jesli nie jest gwiazdka i nastepne to spacja lub nowa linia
                    if(flag)
                    {
                        index = licznik; //zapamietuje index elementu ktory jest * ...
                        flag = false;
                    }
                    licznik += 1;
                    ui->comboBoxBranch->addItem(brancz);
                    flagaBranchRemot = 2;
                }
                else
                {
                    int l = k;

                    if(zwrot[l] == '*')
                    {
                        //jesli byla * to szukamy tego elementu , zmieniam flage zeby tam wyzej wszedl
                        flag=true;
                    }
                }
            }
            ++i;
        }

        brancz = "";
    }

    QString nazwa= ui->comboBoxBranch->itemText(0);       //teraz zamiana, zapamietuje nazwe elementu pierwszego w liscie
    ui->comboBoxBranch->setItemText(0,ui->comboBoxBranch->itemText(index)); //elementem pierwszym jest glowny branch, ten z *
    ui->comboBoxBranch->setItemText(index,nazwa);     //tam gdzie byla nazwa galezi z *, jest teraz ten spod 0 ;]
    connect(this->ui->comboBoxBranch,SIGNAL(currentIndexChanged(QString)),this,SLOT(zmianaBrancha()));
    proces->close();
    proces->start(branchRemote(sciezkaGita));
}

void Fetch::zmianaBrancha()
{
    proces->close();
    proces->start(checkout(sciezkaGita,ui->comboBoxBranch->currentText()));
}

void Fetch::merge()
{
    proces->close();
    proces->start(git::merge(sciezkaGita,ui->comboBox_2BranchRemote->currentText()));
}

void Fetch::aktualizujBrancheRemote(QByteArray zwrot)
{
    if(zwrot.contains("* [new branch]"))
    {
        for(int i = 0; i < zwrot.length()-1; i++)
        {
            while(zwrot[i] != '-' || zwrot[i+1] != '>')
            {
                i++;
            }
            i += 2;
            QString brancz = "";
            bool flaga = true;

            while(zwrot[i] == ' ')
            {
                i++;
            }

            while(zwrot[i] != '\n' && zwrot[i] != '\0')
            {
                brancz += zwrot[i];
                i++;
            }

            for(int i = 0; i < ui->comboBox_2BranchRemote->count(); i++)
            {
                if(ui->comboBox_2BranchRemote->itemText(i) == brancz)
                {
                    flaga = false;
                }
            }

            if(flaga)
            {
                ui->comboBox_2BranchRemote->addItem(brancz);
            }

            ui->comboBox_2BranchRemote->show();
            ui->label_2BranchRemote->show();
            ui->pushButton_2Merge->show();
            ui->comboBoxBranch->show();
            ui->labelBranch->show();
        }
    }
}
