#include <QProcess>

#include "git.h"
#include "push.h"
#include "fetch.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

/* KONSTRUKTOR */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , proces(NULL)
    , flagaPierwszegoCommitu(false)
    , flagaSprawdzania(true)
    , sciezkaGita("")
    , flagaSprawdzeniaIstnieniaGita(true)
    , flagaWyswietlania(false)  //na poczatku nie wyswietlaj zwrotu bo nie trzeba
{
    ui->setupUi(this);
    proces = new QProcess(this);
    proces->setReadChannel(QProcess::StandardOutput);
    connect(proces, SIGNAL(readyRead()), this, SLOT(zczytajZwrot()));

    ui->wyswietlacz->hide();        // wszystko najpierw schowane jak jest git to sie odsloni nizej
    ui->pushButtonCommit->hide();
    ui->pushButtonLog->hide();
    ui->pushButtonStatus->hide();
    ui->menuBar->hide();

    try
    {
        sprawdzCzyIstniejeGit();
    }
    catch(QString wyjatek)
    {
        ui->labelGit->setText(wyjatek);    //na poczatku ustawiam gdyby nie bylo gita
    }

}

/* **************** */
MainWindow::~MainWindow()
{
    delete ui;
    delete repozytorium;
    delete brancz;
    delete plik;
    delete remot;
    delete user_settings;
    delete help_indeks;
    delete about;

    if(proces->isOpen())
    {
        proces->close();
    }

    delete proces;
}

/* R E P O S I T O R Y*/
void MainWindow::otworzRepozytorium()
{
    /*otworz okno Repozytorium */
    repozytorium = new Repository("otworz",this);
    repozytorium->show();
}

void MainWindow::stworzRepozytorium()
{
    /*otworz okno Repozytorium */
    repozytorium = new Repository("stworz",this);
    repozytorium->show();
}

void MainWindow::usunRepozytorium()
{
    /*otworz okno Repozytorium */
    repozytorium = new Repository("usun",this);
    repozytorium->show();
}

/* ************************ */
/* F I L E */
void MainWindow:: dodajPlik()
{
    if(sciezkaGita != "")
    {
        plik = new File(sciezkaGita, this);
        ((Repository*)plik)->show();            //jako ze nie dziedzicze bezposrednio z QWidget, nie mam dostepu do jej metody show , ni wia camu
    }
    else
    {
        ui->wyswietlacz->append("First open repository...");
    }
}

/* ************************* */
/* B R A N C H */
void MainWindow::branch()
{
    if((sciezkaGita != "") && flagaPierwszegoCommitu)
    {
        brancz = new Branch(sciezkaGita,this);
        brancz->show();
    }
    else if(sciezkaGita ==  "")
    {
        ui->wyswietlacz->append("First open repository...");
    }
    else
    {
        ui->wyswietlacz->append("No *master branch found... Make first commit! :)");
    }
}

/* R E M O T E */
void MainWindow::remote()
{
    if((sciezkaGita != "") && flagaPierwszegoCommitu)
    {
        remot = new Remote(sciezkaGita,this);
        remot->show();
    }
    else if(sciezkaGita == "")
    {
        ui->wyswietlacz->append("First open repository...");
    }
    else
    {
        ui->wyswietlacz->append("No *master branch found... Make first commit! :)");
    }
}

void MainWindow::clone()
{
    if((sciezkaGita != "") && flagaPierwszegoCommitu)
    {
        remot= new Clone(sciezkaGita,this);
        remot->show();
    }
    else if(sciezkaGita == "")
    {
        ui->wyswietlacz->append("First open repository...");
    }
    else
    {
        ui->wyswietlacz->append("No *master branch found... Make first commit! :)");
    }
}

void MainWindow:: fetch()
{
    if((sciezkaGita != "") && flagaPierwszegoCommitu)
    {
        remot= new Fetch(sciezkaGita,this);
        remot->show();
    }
    else if(sciezkaGita == "")
    {
        ui->wyswietlacz->append("First open repository...");
    }
    else
    {
        ui->wyswietlacz->append("No *master branch found... Make first commit! :)");
    }
}

void MainWindow::push()
{
    if((sciezkaGita != "") && flagaPierwszegoCommitu)
    {
        remot= new Push(sciezkaGita,this);
        remot->show();
    }
    else if(sciezkaGita == "")
    {
        ui->wyswietlacz->append("First open repository...");
    }
    else
    {
        ui->wyswietlacz->append("No *master branch found... Make first commit! :)");
    }
}

/* ***************************** */
/* U S E R */
void MainWindow::userSettings()
{
    user_settings = new UserSettings(sciezkaGita,this);
    user_settings->show();
}

/* ***************************** */
/* H E L P  I N D E K S */
void MainWindow::helpIndeks()
{
    help_indeks = new HelpIndeks(this);
    help_indeks->show();
}

/* MainWindow methods */
void MainWindow::sprawdzPierwszyCommit()
{
    flagaPierwszegoCommitu =false;
    flagaSprawdzeniaIstnieniaGita =false;

    if(proces->isOpen())
    {
        proces->close();
    }

    proces->start(git::branch(sciezkaGita));    //branch musi zwrocic galaz z gwiazdka *
}

void MainWindow::weryfikujPierwszyCommit(QString zwrot)
{
    if(zwrot.contains("*"))
    {
        flagaPierwszegoCommitu = true;        //byl pierwszy commit
        ui->wyswietlacz->append("Ready to go...!");
    }
}

void MainWindow:: status()
{
    if(proces->isOpen())
    {
        proces->close();
    }

    if(sciezkaGita!="")
    {
        proces->start(git::status(sciezkaGita));
    }
    else
    {
        ui->wyswietlacz->append("First open git repository.");
    }
}

void MainWindow:: wyswietl(QString tekst)
{
    ui->wyswietlacz->append(tekst);
}

void MainWindow::log()
{
    if(proces->isOpen())
    {
        proces->close();
    }

    if(sciezkaGita != "")
    {
        proces->start(git::log(sciezkaGita));
    }
    else
    {
        ui->wyswietlacz->append("First open git repository.");
    }
}

void MainWindow:: zczytajZwrot()
{
    QByteArray bytes = proces->readAllStandardOutput();
    wyswietlZwrot(bytes);
}

void MainWindow::wyswietlZwrot(QString zwrot)
{
    if(flagaWyswietlania)
    {
        ui->wyswietlacz->append(zwrot);
    }

    if(flagaSprawdzeniaIstnieniaGita)
    {
        weryfikujIstnienieGita(zwrot);
    }
    else if(flagaSprawdzania)
    {
        weryfikujPierwszyCommit(zwrot);
    }
}

void MainWindow::commit()
{
    if(proces->isOpen())
    {
        proces->close();
    }

    if(sciezkaGita!="")
    {
        proces->start(git::commit(sciezkaGita));
    }
    else
    {
        ui->wyswietlacz->append("First open git repository.");
    }
}

void MainWindow::aboutProgram()
{
    about = new About(this);
    about->show();
}

void MainWindow::ustawLabelGit(bool flaga)
{
    if(flaga && (sciezkaGita != ""))
    {
        ui->labelGit->setText("Your repository in "+sciezkaGita);
    }
    else if(!flaga)
    {
        ui->labelGit->clear();
    }
}

void MainWindow::sprawdzCzyIstniejeGit()
{
    if(proces->isOpen())
    {
        proces->close();
    }

    proces->start(checkGit()); //urchamiam po prostu "git " i sprawdzam jaki zwrot czy istnieje w ogole, jest zainstalowany

    if(proces->state()!=2)
    {
        //sprawdzam czy proces jest uruchomiony i gotowy do zapisu- odczytu
        QString wyjatek="No git program found! Install it very quickly and come back :)";   //jesli nie rzucam wyjatek
        throw wyjatek;
    }
}

void MainWindow::weryfikujIstnienieGita(QString zwrot)
{
    if(zwrot.contains("usage: git"))
    {
        ui->labelGit->clear();  //czyscze bo to zawiera info ze nie ma gita a jest :D
        ui->wyswietlacz->show();            //odkrywam elementy bo jest git to moze dzialac
        ui->pushButtonCommit->show();
        ui->pushButtonLog->show();
        ui->pushButtonStatus->show();
        ui->menuBar->show();
        flagaWyswietlania=true;
        user_settings = new UserSettings(sciezkaGita,this);
        user_settings->checkDefaultRepository();    //jesli zczyta cos pliku ustawi to jako domyslna sciezke
        delete user_settings;
        ustawLabelGit(true);    //ustawiam labelGit zeby pokazywal sciezke jesli jest domyslna, to tez jest w Repository przy otwieraniu usuwaniu itp.
        sprawdzPierwszyCommit();

        connect(ui->actionRepositoryOpen,SIGNAL(triggered()),this,SLOT(otworzRepozytorium()));
        connect(ui->actionRepositoryCreate,SIGNAL(triggered()),this,SLOT(stworzRepozytorium()));
        connect(ui->actionRepositoryDelete,SIGNAL(triggered()),this,SLOT(usunRepozytorium()));
        connect(ui->actionFileAdd,SIGNAL(triggered()),this,SLOT(dodajPlik()));
        connect(ui->pushButtonStatus,SIGNAL(clicked()),this,SLOT(status()));
        connect(ui->pushButtonLog,SIGNAL(clicked()),this,SLOT(log()));
        connect(ui->actionBranchBranch,SIGNAL(triggered()),this,SLOT(branch()));
        connect(ui->actionRemoteFetch_2,SIGNAL(triggered()),this,SLOT(fetch()));
        connect(ui->actionModify_Repositories,SIGNAL(triggered()),this,SLOT(remote()));
        connect(ui->actionRemoteClone,SIGNAL(triggered()),this,SLOT(clone()));
        connect(ui->actionRemotePush,SIGNAL(triggered()),this,SLOT(push()));
        connect(ui->actionUserSettings,SIGNAL(triggered()),this,SLOT(userSettings()));
        connect(ui->actionIndex,SIGNAL(triggered()),this,SLOT(helpIndeks()));
        connect(ui->actionAbout,SIGNAL(triggered()),this,SLOT(aboutProgram()));
        connect(ui->pushButtonCommit,SIGNAL(clicked()),this,SLOT(commit()));
    }
}
