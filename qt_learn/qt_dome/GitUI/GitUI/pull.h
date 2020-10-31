#ifndef PULL_H
#define PULL_H
#include "remoting.h"
#include <QDialog>

namespace Ui {
class Pull;
}

class Pull : public Remoting
{
    Q_OBJECT
    
public:
    explicit Pull(QString sciezkaGita,QWidget *parent = 0);
    ~Pull();
    
private:
    void otworz();
    void stworz();
    void wyswietlZwrot(QByteArray zwrot);
    Ui::Pull *ui;
};

#endif // PULL_H
