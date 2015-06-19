#ifndef SERVERTHREAD_H
#define SERVERTHREAD_H

#include <QThread>

/**
 * Thread that executes the stubserver function.
 */
class ServerThread : public QThread
{
    Q_OBJECT

    const QString configName;
    const int     port;

public:
    ServerThread(const QString &configName, int port = 4225);

    /**
     * Stop the server, and then end this thread.
     */
    void doStop();

signals:
    /**
     * This signal is triggered when the server stops with an error.
     * If the server exists normally the "finish" signal from the
     * QThread can be used.
     *
     * @param s - the error message / exception
     */
    void error(const QString &s);

protected:
    /**
     * Thread method: start the server.
     */
    void run() Q_DECL_OVERRIDE;
};

#endif // SERVERTHREAD_H
