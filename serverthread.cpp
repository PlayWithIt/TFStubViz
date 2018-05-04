
#include <QDebug>

#include <string>

#include <utils/utils.h>
#include <utils/Log.h>
#include <stubserver/stubserver.h>

#include "serverthread.h"


ServerThread::ServerThread(const QString &configName, int port)
   : configName(configName)
   , port(port)
{
}


/**
 * Stop the server, and then end this thread.
 */
void ServerThread::doStop()
{
    stubserver::stopServer();
}


/**
 * Thread method: start the server.
 */
void ServerThread::run()
{
    try {
        std::string s = configName.toLatin1().data();
        stubserver::runServer(s.c_str(), port);
    }
    catch (const std::exception &e) {
        utils::Log::error(e.what());
    }
}
