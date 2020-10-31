#include "navbarwidget.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Q_INIT_RESOURCE(navbar);

    NavbarWidget w;
    w.show();
    
    return app.exec();
}
