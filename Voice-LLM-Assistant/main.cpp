#include "Voice-LLM-Assistant.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    VoiceLLMAssistant w;
    w.show();
    return a.exec();
}
