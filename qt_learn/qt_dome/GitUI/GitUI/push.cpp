#include "push.h"
#include "ui_push.h"
#include "branch.h"
Push::Push(QString sciezkaGita,QWidget *parent)
    : Remoting(sciezkaGita,parent)
    , ui(new Ui::Push)
{
    ui->setupUi(this);
    ui->labelGit->setText(" Your repository in "+sciezkaGita);
    ui->labelInfo->setText("Update remote refs along\n with associated objects ");

    proces->start(git::branch(sciezkaGita));
    proces->setProcessChannelMode(QProcess::MergedChannels);                //dzieki temu dostaje wiecej zwrotu od GIT'a

    connect(proces,SIGNAL(readyRead()), this, SLOT(zczytajZwrot()));
    connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(otworz()));
    connect(ui->pushButton_2Back,SIGNAL(clicked()),this,SLOT(close()));
    connect(ui->pushButton_3Pull,SIGNAL(clicked()),this,SLOT(pull()));
}

Push::~Push()
{
    delete ui;
}

void Push::wyswietlZwrot(QByteArray  zwrot)
{
    if(flagaBranchRemot == 2)
    {
        ui->wyswietlacz->append(zwrot);
    }

    if(flagaBranchRemot == 0)
    {
        zczytajBranche(zwrot);
    }
    else if(flagaBranchRemot == 1)
    {
        zczytajRepo(zwrot);
    }
}

void Push::otworz()
{
    ui->wyswietlacz->clear();
    proces->readAll();
    proces->close();

    if(ui->comboBoxBranch->currentText() != "" && ui->comboBox_2Remote->currentText() != "")
    {
        proces->start(push(sciezkaGita, ui->comboBox_2Remote->currentText(), ui->comboBox_2Remote->currentText()));
    }
    else
    {
        if(ui->comboBoxBranch->currentText() == "")
        {
            ui->wyswietlacz->append("There are no branches to push!");
        }

        if(ui->comboBox_2Remote->currentText() == "")
        {
            ui->wyswietlacz->append("There are no remotes to push!");
        }
    }
}

void Push::zczytajBranche(QByteArray zwrot)
{
    int i;
    QString brancz = "";       //nazwa galezi
    int index = 0;             //bedzie wskazywac indeks galezi z *
    bool flaga = false;
    int licznik = 0;      //liczy ile elementow dodalem
    for(i = 0;i < zwrot.length()-1; i++)
    {
        while(zwrot[i]!=' '&& zwrot[i]!='\n')
        {
            //dopoki nie ma ani tego ani tego, to wyrazenie jest prawda i robi
            brancz += zwrot[i];                   //jak tylko ktores sie zmieni to przestaje  bo 1*0 = 0
            int k = i;
            if(k < (zwrot.length()-1))
            {
                if(zwrot[k] != '*' && (zwrot[++k] == ' '|| zwrot[k] == '\n'))
                {
                    //jesli nie jest gwiazdka i nastepne to spacja lub nowa linia
                    if(flaga)
                    {
                        index = licznik; //zapamietuje index elementu ktory jest * ...
                        flaga = false;
                    }
                    licznik += 1;
                    ui->comboBoxBranch->addItem(brancz);
                }
                else
                {
                    int l = k;
                    if(zwrot[l] == '*')
                    {
                        //jesli byla * to szukamy tego elementu , zmieniam flage zeby tam wyzej wszedl
                        flaga = true;
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
    proces->close();
    flagaBranchRemot=1;
    proces->start(remote(sciezkaGita));
}

void Push::zczytajRepo(QByteArray zwrot)
{
    int i;
    QString repo = "";       //nazwa zdalnego repozytorium

    for(i = 0; i < zwrot.length()-1; i++)
    {
        while(zwrot[i] != '\n' && zwrot[i] != ' ')
        {
            //dopoki nie ma ani tego ani tego, to wyrazenie jest prawda i robi
            repo+=zwrot[i];                   //jak tylko ktores sie zmieni to przestaje  bo 1*0 = 0
            ++i;
        }
        ui->comboBox_2Remote->addItem(repo);
        repo = "";
    }

    flagaBranchRemot = 2;
}

void Push::pull()
{
    ui->wyswietlacz->clear();

    if(ui->comboBox_2Remote->currentText() != "")
    {
        proces->close();
        proces->start(git::pull(sciezkaGita,ui->comboBox_2Remote->currentText()));
    }
    else
    {
        ui->wyswietlacz->append("No remotes found.");
    }
}
