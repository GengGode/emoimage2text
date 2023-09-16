#include "form/EmoToText.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    EmoToText w;
    w.show();
    return a.exec();
}
