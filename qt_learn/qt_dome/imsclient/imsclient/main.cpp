#include "workstation.h"
#include <QtCore/QTextCodec>

#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    WorkStation w;
    int ret = w.createLoginScreen();

    if (ret == QDialog::Accepted)
    {
        w.createShowScreen();
        ret = a.exec();
    }

    return ret;
}
