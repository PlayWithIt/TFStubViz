#include <string.h>

#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    const char *filename = NULL;

    for (int i = 1; i < argc; ++i)
    {
        if (strcmp(argv[i], "-d") == 0 && i+1 < argc)
            filename = argv[++i];
        else {
            printf("Usage: TFStubViz {-d device.properties}\n\n");
            return 1;
        }
    }

    MainWindow w(filename);
    w.show();

    return a.exec();
}
