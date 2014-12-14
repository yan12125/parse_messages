#include "MergerDialog.hpp"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MergerDialog lw;
    lw.resize(400, 200);
    lw.show();
    return a.exec();
}
