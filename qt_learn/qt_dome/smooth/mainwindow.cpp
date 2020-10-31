#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPalette>
#include <QDebug>
#include <QList>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_bInitShow(false)
{
    ui->setupUi(this);

    m_Lframe  = new QFrame;
    m_Rframe  = new QFrame;
    m_sp      = new QSplitter(this);
    m_pButton = new QPushButton(this); // after Splitter

    m_sp->addWidget(m_Lframe);
    m_sp->addWidget(m_Rframe);

    connect(m_sp, SIGNAL(splitterMoved(int, int)), this, SLOT(sl_splitterMoved(int,int)));
    connect(m_pButton, SIGNAL(clicked()), this, SLOT(sl_btnClicked()));

    setCentralWidget(m_sp);
    setWindowFlags(Qt::FramelessWindowHint);

    m_Lframe->setMouseTracking(true);
    m_Rframe->setMouseTracking(true);
    m_sp->setMouseTracking(true);
    m_pButton->setMouseTracking(true);
    setMouseTracking(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setBtnPos()
{
    int width = m_Lframe->width() - m_pButton->width();
    int heigh = (m_Lframe->height() - m_pButton->height())/2;
    m_pButton->move(width, heigh);
}

void MainWindow::setBtnIcon()
{
    if(m_Rframe->width() != 0)
    {
        m_pButton->setIcon(QIcon(":/images/quit.png"));
    }
    else
    {
        m_pButton->setIcon(QIcon(":/images/update.png"));
    }
}

void MainWindow::showEvent(QShowEvent *event)
{
    if(!m_bInitShow)
    {
        setBtnPos();
        setBtnIcon();
        m_bInitShow = true;

        QPalette p2 = m_Rframe->palette(); // initial the frame back round
        p2.setColor(QPalette::Window,Qt::gray);
        p2.setColor(QPalette::WindowText,Qt::red);
        m_Rframe->setPalette(p2);
        m_Rframe->setAutoFillBackground(true);
        m_Rframe->setMaximumWidth(300);

        base= m_sp->sizes();

        m_pButton->setFixedSize(32,32);
        m_pButton->setIconSize(QSize(16,16));
        m_pButton->setVisible(false);
        m_pButton->setStyleSheet("border:none;");

    }
    QMainWindow::showEvent(event);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    setBtnPos();
    QMainWindow::resizeEvent(event);
}

void MainWindow::sl_splitterMoved(int pos, int index)
{
    setBtnPos();

    if(m_Rframe->width() == 0)
    {
        setBtnIcon();
    }
}

void MainWindow::sl_btnClicked()
{
    // static  QList<qint32> base= m_sp->sizes(); //keep the initial pos
    QList<qint32>  newSize;

    if(m_Rframe->width()!=0)
    {
        newSize<<base.at(0)+base.at(1)<<0;
    }
    else
    {
        newSize<<base.at(0)<<base.at(1);
    }
    m_sp->setSizes(newSize);  //move the SPLITTER

    setBtnPos();
    setBtnIcon();
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
   int isWidth  = m_pButton->pos().x() + m_pButton->width();
   int isHeight = m_pButton->pos().y() + m_pButton->height();

   int x = event->pos().x();
   int y = event->pos().y();

   if(x > m_pButton->pos().x() && x < isWidth && y > m_pButton->y() && y < isHeight)
   {
       m_pButton->setVisible(true);
   }
   else
   {
       m_pButton->setVisible(false);
   }
}
