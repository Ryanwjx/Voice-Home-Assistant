#include "MQTT.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MQTT w;
    Q_UNUSED(w);
    return a.exec();
}
