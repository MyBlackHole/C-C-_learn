#ifndef NAVBARWIDGET_H
#define NAVBARWIDGET_H

#include <QWidget>
#include <QComboBox>
#include <QCheckBox>
#include <QListWidget>
#include <QPushButton>
#include <QCloseEvent>
#include "navbar.h"

class NavbarWidget : public QWidget
{
    Q_OBJECT
    
public:
    NavbarWidget(QWidget *parent = 0);
    ~NavbarWidget();

private slots:
    void addPage();
    void insertPage();
    void removePage();
    void changeStylesheet(int index);
    void navBarCurrentChanged(int index);
    void navBarVisibleRowsChanged(int rows);
    void navBarStateChanged(bool collapsed);

protected:
    void closeEvent(QCloseEvent *e);

    NavBar *navBar;
    QListWidget *signalWidget;
    QComboBox *styleBox;
    QCheckBox *showHeaderBox;
    QCheckBox *showColBtnBox;
    QCheckBox *showOptMenuBox;
    QCheckBox *autoPopupBox;
    QPushButton *addPageButton;
    QPushButton *removePageButton;
    QPushButton *insertPageButton;
};

#endif // NAVBARWIDGET_H
