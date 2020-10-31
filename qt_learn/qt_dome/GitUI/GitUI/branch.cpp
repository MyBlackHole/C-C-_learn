#include "branch.h"
#include "ui_branch.h"

Branch::Branch(QString sciezka,QWidget *parent)
    : QDialog(parent)
    , sciezkaGita(sciezka)
    , flagaPoleceniaBranch(true)
    , hideOrShow(true) //domyslnie flase, jak klikne tak ze wywolam git branch, zmienie na true
    , ui(new Ui::Branch)
{
    ui->setupUi(this);
    ui->label->setText("Your branches in "+sciezkaGita);
    ui->labelError->clear();
    ui->labelInfo->clear();
    ui->labelError->hide();         //domyslnie elementy sa schowane
    ui->labelInfo->hide();          //odkrywaja sie w momencie wybrania opcji edycji
    ui->pushButtonAddBranch->hide();
    ui->pushButtonDeleteBranch->hide();
    ui->lineEditBranchName->hide();

    proces = new QProcess(this);
    proces->setReadChannel(QProcess::StandardOutput);
    proces->setProcessChannelMode(QProcess::MergedChannels);
    proces->start(branch(sciezkaGita));                 //od razu wyswietlam status repo jakie wczesniej otworzylem

    connect(proces, SIGNAL(readyRead()), this, SLOT(zczytajZwrot())); //wyswietlam je w wyswietlaczu
    connect(this->ui->pushButtonAddBranch,SIGNAL(clicked()),this,SLOT(dodajBranch()));
    connect(this->ui->radioButton,SIGNAL(toggled(bool)),this,SLOT(showHideElements()));
    connect(this->ui->pushButtonDeleteBranch,SIGNAL(clicked()),this,SLOT(usunBranch()));
    connect(this->ui->pushButtonBack,SIGNAL(clicked()),this,SLOT(close()));
    connect(this->ui->pushButtonMerge,SIGNAL(clicked()),this,SLOT(merge()));
    connect(this->ui->pushButtonCheckout,SIGNAL(clicked()),this,SLOT(checkoutToHash()));
}

Branch::~Branch()
{
    delete ui;

    if(proces->isOpen())
    {
        proces->close();
    }

    delete proces;
}

void Branch::zczytajBranche(QByteArray zwrot)
{
    int i;
    QString brancz="";       //nazwa galezi
    int index=0;             //bedzie wskazywac indeks galezi z *
    bool flaga=false;
    int licznik=0;      //liczy ile elementow dodalem
    for(i = 0; i < zwrot.length() - 1; i++)
    {
        while(zwrot[i] != ' '&& zwrot[i] != '\n')
        {
            //dopoki nie ma ani tego ani tego, to wyrazenie jest prawda i robi
            brancz += zwrot[i];                   //jak tylko ktores sie zmieni to przestaje  bo 1*0 = 0
            int k = i;
            if(k < (zwrot.length() - 1))
            {
                if(zwrot[k]!='*' && (zwrot[++k]==' '|| zwrot[k]=='\n'))
                {
                    //jesli nie jest gwiazdka i nastepne to spacja lub nowa linia
                    if(flaga == true)
                    {
                        index = licznik; //zapamietuje index elementu ktory jest * ...
                        flaga = false;
                    }
                    licznik += 1;
                    ui->comboBox->addItem(brancz);
                }
                else
                {
                    int l = k;

                    if(zwrot[l] == '*')
                    {
                        //jesli byla * to szukamy tego elementu , zmieniam flage zeby tam wyzej wszedl
                        flaga=true;
                    }
                }
            }
            ++i;
        }

        brancz = "";
    }

    QString nazwa = ui->comboBox->itemText(0);       //teraz zamiana, zapamietuje nazwe elementu pierwszego w liscie
    lokalnaGalaz = nazwa;
    ui->comboBox->setItemText(0,ui->comboBox->itemText(index)); //elementem pierwszym jest glowny branch, ten z *
    ui->comboBox->setItemText(index,nazwa);     //tam gdzie byla nazwa galezi z *, jest teraz ten spod 0 ;]
    aktualizujBrancheCombo2();
    flagaPoleceniaBranch = false;     //dopoki nastepne polecenie branch nie zostanie wywolane, nie wywoluj tej funkcji
    connect(this->ui->comboBox,SIGNAL(currentIndexChanged(QString)),this,SLOT(zmianaBrancha()));
    connect(this->ui->comboBox,SIGNAL(currentIndexChanged(QString)),this,SLOT(aktualizujBrancheCombo2()));
}

void Branch:: zczytajZwrot()
{
    QByteArray bytes = proces->readAllStandardOutput();

    if(flagaPoleceniaBranch)
    {
        zczytajBranche(bytes);
    }

    ui->wyswietlacz->clear();
    wyswietlZwrot(bytes);
}

void Branch::wyswietlZwrot(QString zwrot)
{
    ui->wyswietlacz->append(zwrot);
}

void Branch::zmianaBrancha()
{
    if(ui->comboBox->currentText()!=lokalnaGalaz)
    {
        proces->close();
        proces->start(checkout(sciezkaGita,ui->comboBox->currentText()));
        ui->wyswietlacz->clear();
    }
}

void Branch::dodajBranch()
{
    if(ui->lineEditBranchName->text() != "")
    {
        if(!ui->lineEditBranchName->text().contains(" "))
        {
            if(ui->comboBox->findText(ui->lineEditBranchName->text())==-1)
            {
                proces->close();
                proces->start(addBranch(sciezkaGita,ui->lineEditBranchName->text()));
                ui->comboBox->addItem(ui->lineEditBranchName->text());
                ui->lineEditBranchName->clear();
                ui->labelError->clear();
                ui->labelInfo->setText("The branch has been added!");
                aktualizujBrancheCombo2();
            }
            else
            {
                ui->labelError->setText("The branch with the given name already exists...");
                ui->labelInfo->setText("Change another name:");
            }
        }
        else
        {
            ui->labelError->setText("The name can't' contain space...");
            ui->labelInfo->setText("Try another name:");
        }
    }
    else
    {
        ui->labelInfo->clear();
        ui->labelError->setText("Insert branch name.");
    }
}

void Branch::showHideElements()
{
    if(hideOrShow)
    {
        ui->labelError->show();         //domyslnie elementy sa schowane
        ui->labelInfo->show();          //odkrywaja sie w momencie wybrania opcji edycji
        ui->pushButtonAddBranch->show();
        ui->pushButtonDeleteBranch->show();
        ui->lineEditBranchName->show();
        ui->comboBox_2->show();
        hideOrShow = false;
    }
    else
    {
        ui->labelError->hide();         //domyslnie elementy sa schowane
        ui->labelInfo->hide();          //odkrywaja sie w momencie wybrania opcji edycji
        ui->pushButtonAddBranch->hide();
        ui->pushButtonDeleteBranch->hide();
        ui->lineEditBranchName->hide();
        ui->comboBox_2->hide();
        hideOrShow = true;
    }
}

void Branch::aktualizujBrancheCombo2()
{
    lokalnaGalaz=ui->comboBox->currentText();

    for(int i=0;i<ui->comboBox->count();i++)
    {
        if(ui->comboBox->currentText()!=ui->comboBox->itemText(i))
        {
            if(ui->comboBox_2->findText(ui->comboBox->itemText(i))==-1)         //sprawdzam czy nie ma tego elementu juz w comboBoxie2
            {
                ui->comboBox_2->addItem(ui->comboBox->itemText(i));       //dodaje wszystkie procz tego z * ktorego bezkarnie usunac nie moge
            }
        }
        else
        {
            int indeks=ui->comboBox_2->findText(ui->comboBox->currentText());

            if(indeks!=-1)
            {
                ui->comboBox_2->removeItem(indeks);
            }
        }
    }
}

void Branch::usunBranch()
{
    int indeks2 = ui->comboBox_2->currentIndex();    //indeks usuwanego elementu z comboBox_2

    if(ui->comboBox_2->currentText() != "")
    {
        ui->label_2->setText("Deleting:" + ui->comboBox_2->currentText());
        proces->start(deleteBranch(sciezkaGita,ui->comboBox_2->currentText()));
        int indeks = ui->comboBox->findText(ui->comboBox_2->currentText());      //indeks usuwanego elementu z comboBox
        ui->comboBox_2->removeItem(indeks2);
        ui->comboBox->removeItem(indeks);
        ui->labelInfo->clear();
        ui->labelError->setText("The selected branch has been deleted!");
    }
}

void Branch::merge()
{
    proces->close();
    proces->start(git::merge(sciezkaGita,ui->comboBox_2->currentText()));
}

void Branch::checkoutToHash()
{
    if(ui->lineEditBranchName != NULL)
    {
        proces->close();
        proces->start(checkout(sciezkaGita,ui->lineEditBranchName->text()));
    }
}
