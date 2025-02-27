#include "stt.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    STT w;
    Q_UNUSED(w);
    return a.exec();
}
