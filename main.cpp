#include "xg_vision.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    XG_Vision w;
    w.show();

    return a.exec();
}
