#include "GLDRichTextEditor.h"

#include <QtWidgets/QApplication>

//#include "vld.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    GLDRichTextEditor w;
    w.show();

    return a.exec();
}
