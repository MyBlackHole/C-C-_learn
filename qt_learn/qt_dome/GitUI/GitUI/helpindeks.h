#ifndef HELPINDEKS_H
#define HELPINDEKS_H

#include <QDialog>

namespace Ui {
class HelpIndeks;
}

class HelpIndeks : public QDialog
{
    Q_OBJECT

public:
    explicit HelpIndeks(QWidget *parent = 0);
    ~HelpIndeks();
    
private:
    Ui::HelpIndeks *ui;
    QString okreslAktualnaSciezke();    /* okresla aktualna sciezke programu, tutaj jest plik .html indeksu do wyswietlenia */
};

#endif // HELPINDEKS_H
