#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFrame>
#include <QSplitter>
#include <QPushButton>
#include <QMouseEvent>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void setBtnPos();
    void setBtnIcon();

private:
    Ui::MainWindow *ui;
    QFrame  *m_Lframe,*m_Rframe;
    QSplitter *m_sp;
    QPushButton *m_pButton;
    bool m_bInitShow;
    QList<qint32> base;

protected:
    void showEvent(QShowEvent *event);
    void resizeEvent(QResizeEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private slots:
    void sl_splitterMoved(int pos, int index);
    void sl_btnClicked();
};

#endif // MAINWINDOW_H
