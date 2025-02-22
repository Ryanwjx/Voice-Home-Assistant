#include "tts.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TTS w;
    Q_UNUSED(w);
    return a.exec();
}
