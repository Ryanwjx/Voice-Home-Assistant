#include <QApplication>
#include "llm.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    LLM window;
    Q_UNUSED(window);
    return app.exec();
}
