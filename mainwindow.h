#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "serverthread.h"


namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    void loadConfig();

public:
    explicit MainWindow(const char *configFile, int port);
    ~MainWindow();

public slots:
    void serverError(const QString &str);
    void serverStopped();

private slots:
    void on_action_Exit_triggered();

    void on_action_Load_stub_config_triggered();

    void on_action_Run_triggered();

    void on_action_Stop_triggered();

private:
    Ui::MainWindow *ui;

    QString      configFileName;
    ServerThread *serverThread;
    bool          serverOK;
    int           port;
};

#endif // MAINWINDOW_H
