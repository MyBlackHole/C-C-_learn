#include <QFile>
#include <QTextStream>

#include "mainwindow.h"
#include "usersettings.h"
#include "ui_usersettings.h"

UserSettings::UserSettings(QString sciezkaGita,QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::UserSettings)
    , sciezkaGita(sciezkaGita)
    , flagaConfigu(false) //na poczatku wyswietlaj zwrot, gdy true - zczytaj info ale nie wyswietlaj
{

    ui->setupUi(this);
    ui->lineEdit_3DefaultRepo->setText(sciezkaGita);    //poczatkowo wpisuje do lineedita sciezkeGita, nie mozna jej zmieniac, ja moge zapisac do pliku
    ui->lineEdit_3DefaultRepo->setReadOnly(true);
    proces = new QProcess(this);

    if(sciezkaGita == "")
    {
        ui->lineEditChangeEditor->hide();
        ui->lineEditChangeName->hide();
        ui->lineEdit_2ChangeEmail->hide();
        ui->label_2DeveloperINfo->hide();
        ui->labelGit->hide();
        ui->pushButtonChangeEditor->hide();
        ui->pushButtonChangeName->hide();
        ui->pushButton_2ChangeEmail->hide();
        ui->labelGitSettings->hide();
    }
    else
    {
        ui->labelGit->setText("Your repository in "+sciezkaGita);
        checkConfig();
    }

    checkDefaultRepository();
    connect(ui->pushButtonBack,SIGNAL(clicked()),this,SLOT(close()));
    connect(ui->pushButton_3SetRepo,SIGNAL(clicked()),this,SLOT(changeDefaultRepository()));
    connect(ui->pushButtonSetNull,SIGNAL(clicked()),this,SLOT(setNoDefaultRepository()));
    connect(ui->pushButtonChangeName,SIGNAL(clicked()),this,SLOT(changeUserName()));
    connect(ui->pushButton_2ChangeEmail,SIGNAL(clicked()),this,SLOT(changeUserEmail()));
    connect(ui->pushButtonChangeEditor,SIGNAL(clicked()),this,SLOT(changeEditor()));
}

UserSettings::~UserSettings()
{
    delete ui;

    if(proces->isOpen())
    {
        proces->close();
    }

    delete proces;
}

void UserSettings::changeDefaultRepository()
{
    try
    {
        if(ui->lineEdit_3DefaultRepo->text() != NULL)
        {
            QFile plik("repozytorium.txt");     //tworze plik repozytorium.txt w katalgu domyslnym
            plik.open(QIODevice::WriteOnly);
            QTextStream out(&plik);
            out<<ui->lineEdit_3DefaultRepo->text();
            ui->labelRepositoryInfo->setText("The default repository has changed!");
            ((MainWindow*)this->parentWidget())->sciezkaGita=ui->lineEdit_3DefaultRepo->text(); //sciezkaGita to ta linijka zczytana z pliku
            plik.close();
        }
        else
        {
            ui->labelInfo->setText("Open the repository which you want to make default and come back :)");
        }
    }
    catch(std::exception &x)
    {
        ui->labelInfo->setText(x.what());
    }

}

void UserSettings::checkDefaultRepository()
{
    try
    {
        QFile plik("repozytorium.txt");
        plik.open(QIODevice::ReadOnly);
        QTextStream in(&plik);
        QString linia;
        linia = in.readLine();

        if(linia == "")
        {
            ui->labelRepositoryInfo->setText("Currently no default repository set!");
        }
        else
        {
            ui->labelRepositoryInfo->setText("Current default repository is "+linia);
            ((MainWindow*)this->parentWidget())->sciezkaGita=linia; //sciezkaGita to ta linijka zczytana z pliku
        }
        plik.close();
    }
    catch(std::exception &x)
    {
        //jesli pojawi sie wyjatek, np. niepomyslne otwarcie pliku
        ui->labelInfo->setText(x.what());
    }
}

void UserSettings::setNoDefaultRepository()
{
    try
    {
        QFile plik("repozytorium.txt");
        plik.open(QIODevice::WriteOnly);
        QTextStream out(&plik);
        out << "";        //wpisuje pusta linie do pliku , czyli nie ma sciezki :D
        plik.close();
        ui->lineEdit_3DefaultRepo->clear();
        ui->labelRepositoryInfo->setText("No default repository set!");
    }
    catch(std::exception &x)
    {
        ui->labelInfo->setText(x.what());
    }
}

void UserSettings::checkConfig()
{
    proces->start(config(sciezkaGita));
    connect(proces, SIGNAL(readyRead()), this, SLOT(zczytajZwrot()));
    flagaConfigu = true;
}

void UserSettings::zczytajZwrot()
{
    QByteArray zwrot = proces->readAllStandardOutput();

    if(flagaConfigu)
    {
        readConfig(zwrot);
    }
}

void UserSettings::readConfig(QByteArray zwrot)
{
    QString szukane;    //szukana przez nas fraza, czyli imie, email itp.
    for(int i=0;i<zwrot.length();i++)
    {
        while(zwrot[i]!='\n')
        {
            szukane+=zwrot[i];
            i++;
        }

        if(szukane.contains("user.name="))
        {
            szukane.remove(0,strlen("user.same="));
            ui->lineEditChangeName->setText(szukane);
            szukane = "";
        }
        else if(szukane.contains("user.email="))
        {
            szukane.remove(0,strlen("user.email="));
            ui->lineEdit_2ChangeEmail->setText(szukane);
            szukane = "";
        }
        else if(szukane.contains("core.editor="))
        {
            szukane.remove(0,strlen("user.editor="));
            ui->lineEditChangeEditor->setText(szukane);
            szukane = "";
        }
        else
        {
            szukane = "";
        }
    }
}

void UserSettings::changeUserName()
{
    if(ui->lineEditChangeName->text() != NULL)
    {
        proces->close();
        proces->start(configUserName(sciezkaGita,ui->lineEditChangeName->text()));
        ui->labelInfo->setText("User name changed to: "+ui->lineEditChangeName->text());
    }
}

void UserSettings::changeUserEmail()
{
    if(ui->lineEdit_2ChangeEmail->text() != NULL)
    {
        if(ui->lineEdit_2ChangeEmail->text().contains(" "))
        {
            ui->labelInfo->setText("E-mail should be one-word. Change it.");
        }
        else
        {
            proces->close();
            proces->start(configUserEmail(sciezkaGita,ui->lineEdit_2ChangeEmail->text()));
            ui->labelInfo->setText("User e-mail changed to: "+ui->lineEdit_2ChangeEmail->text());
        }
    }
}

void UserSettings::changeEditor()
{
    if(ui->lineEditChangeEditor->text() != NULL)
    {
        if(ui->lineEditChangeEditor->text().contains(" "))
        {
            ui->labelInfo->setText("Editor's name should be one-word. Change it.");
        }
        else
        {
            proces->close();
            proces->start(configChangeEditor(sciezkaGita,ui->lineEditChangeEditor->text()));
            ui->labelInfo->setText("Default editor changed to: "+ui->lineEditChangeEditor->text()+". Hope it really exists :D");
        }
    }
}
