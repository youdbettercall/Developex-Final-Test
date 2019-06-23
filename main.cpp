#include "graphicinterface.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GraphicInterface w;
    w.show();

    return a.exec();
}
