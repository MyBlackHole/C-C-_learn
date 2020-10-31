#include <qapplication.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include "PaintWidget.h"
#include "PaintWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    PaintWindow* p = new PaintWindow();
    p->resize(512, 512);
    p->show();
    
    return app.exec();
}
