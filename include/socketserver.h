#ifndef _SOCKET_SERVER_H_
#define _SOCKET_SERVER_H_

#include <thread>
#include <functional>
#include <list>
#include <string>

#include "socket.h"

class SocketServer
{
public:
    explicit SocketServer(void);
    virtual ~SocketServer(void);

    void start(int port);
    void messageReceived(Socket *socket, const std::string &message);
    void sendToAll(const std::string &message);

    inline void setOnMessageHandler(std::function<void(Socket *socket, const std::string message)> handler)
        { m_onMessageHandler = handler; };
    inline void setOnClientConnectHandler(std::function<void(Socket *socket)> handler)
        { m_onClientConnectHandler = handler; };

private:
    void run(void);

    std::function<void(Socket *socket, const std::string &message)> m_onMessageHandler;
    std::function<void(Socket *socket)> m_onClientConnectHandler;

    int m_port;
    int m_socket;
    std::list<std::shared_ptr<Socket>> m_clients;

    std::thread m_connectionThread;
};

#endif // _SOCKET_SERVER_H_
