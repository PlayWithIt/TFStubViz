#include <string.h>

#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    const char *filename = NULL;
    int port = 4225;

    for (int i = 1; i < argc; ++i)
    {
        if (strcmp(argv[i], "-d") == 0 && i+1 < argc)
            filename = argv[++i];
        else if (strcmp(argv[i], "-p") == 0 && i+1 < argc)
            port = atoi(argv[++i]);
        else {
            printf("\nUsage: TFStubViz {-d device.properties} {-p port}\n"
                   "\n Starts the GUI with the specified configuration on the given port."
                   "\n The default port is 4225."
                   "\n");
            return 1;
        }
    }

    MainWindow w(filename, port > 1024 && port < 64000 ? port : 4225);
    w.show();

    return a.exec();
}
