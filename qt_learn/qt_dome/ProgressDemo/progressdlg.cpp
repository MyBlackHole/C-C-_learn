#include "progressdlg.h"
#include "ui_dialog.h"
#include <QProgressDialog>
#include <QFont>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    QFont font("ZYSong18030",12);
    setFont(font);
    setWindowTitle(QStringLiteral("Progress"));

    FileNum = new QLabel;
    FileNum->setText(QStringLiteral("文件数目："));
    FileNumLineEdit = new QLineEdit;
    FileNumLineEdit->setText(QStringLiteral("100000"));

    ProgressType = new QLabel;
    ProgressType->setText(QStringLiteral("显示类型："));
    comboBox = new QComboBox;
    comboBox->addItem(QStringLiteral("progressBar"));
    comboBox->addItem(QStringLiteral("progressDialog"));

    progressBar = new QProgressBar;

    starBtn = new QPushButton();
    starBtn->setText(QStringLiteral("开始"));

    mainLayout = new QGridLayout(this);
    mainLayout->addWidget(FileNum,0,0);
    mainLayout->addWidget(FileNumLineEdit,0,1);
    mainLayout->addWidget(ProgressType,1,0);
    mainLayout->addWidget(comboBox,1,1);
    mainLayout->addWidget(progressBar,2,0,1,2);
    mainLayout->addWidget(starBtn,3,1);
    mainLayout->setMargin(15);
    mainLayout->setSpacing(10);

    connect(starBtn, SIGNAL(clicked()), this, SLOT(startProgress()));
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::startProgress()
{
    bool ok;
    int num = FileNumLineEdit->text().toInt(&ok);

    if(comboBox->currentIndex() == 0)                   //ProgressBar
    {
        progressBar->setRange(0,num);
        for(int i = 1; i < num + 1; i++)
        {
            progressBar->setValue(i);
        }
    }
    else if(comboBox->currentIndex() == 1)             //ProgressDialog
    {
        QProgressDialog *progressDialog = new QProgressDialog(this);
        QFont font("ZYSong18030",12);
        progressDialog->setFont(font);
        progressDialog->setWindowModality(Qt::WindowModal);
        progressDialog->setMinimumDuration(5);
        progressDialog->setWindowTitle(QStringLiteral("Please Wait"));
        progressDialog->setLabelText(QStringLiteral("Copying..."));
        progressDialog->setCancelButtonText(QStringLiteral("Cancel"));
        progressDialog->setRange(0,num);
        for(int i = 1; i < num + 1; i++)
        {
            progressDialog->setValue(i);
            if(progressDialog->wasCanceled())
                return;
        }
    }
}

