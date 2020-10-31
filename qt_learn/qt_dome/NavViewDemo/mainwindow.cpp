#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "navmodel.h"
#include "navdelegate.h"

MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags flags) :
    QMainWindow(parent, flags),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    NavModel* model = new NavModel(this);
    model->readDataFromConfig(QCoreApplication::applicationDirPath() + "/config.xml");
    NavDelegate* pDelegate = new NavDelegate(this);
    ui->listView->setModel(model);
    ui->listView->setItemDelegate(pDelegate);
    connect(ui->listView, SIGNAL(doubleClicked(const QModelIndex &)), model, SLOT(Collapse(const QModelIndex&)));

}

MainWindow::~MainWindow()
{
    delete ui;
}
