#include "kdreportswindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    KDReportsWindow w;
    w.show();
    return a.exec();
}
