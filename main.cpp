#include "ImageRegistration.h"
#include "ImageSimple.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ImageRegistration w;
    w.show();
    ImageSimple test("test.jpg");
    test.write("new.jpg");
    return a.exec();
}
