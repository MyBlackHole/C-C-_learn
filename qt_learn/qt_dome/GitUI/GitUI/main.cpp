#include <QApplication>
#include "mainwindow.h"

using namespace std;
class file;
//  f4 - switch z headera na cpp
// shift + f2 - switch z headera na cpp
// ctrl +shift + r - rename tego co jest na nim
// ctrl + tab - przeskakuje po otwartych
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    return a.exec();
}
