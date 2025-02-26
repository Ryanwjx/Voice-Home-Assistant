#include "llm.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LLM w;
    Q_UNUSED(w);
    return a.exec();
}
