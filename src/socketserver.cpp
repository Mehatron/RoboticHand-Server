#include "socketserver.h"

#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>

#include "exception.h"

SocketServer::SocketServer(void)
    : m_onMessageHandler(nullptr),
      m_onClientConnectHandler(nullptr),
      m_port(0),
      m_socket(0)
{
}

SocketServer::~SocketServer(void)
{
}

void SocketServer::start(int port)
{
    m_port = port;

    m_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(m_socket < 0)
        throw Exception("Can't create socket");

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htons(INADDR_ANY);
    serverAddr.sin_port = htons(port);

    if(bind(m_socket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
        throw Exception("Can't bind socket");

    listen(m_socket, 1);

    m_connectionThread = std::thread([this]() {
        run();
    });
}

void SocketServer::run(void)
{
    while(true)
    {
        int client = accept(m_socket, NULL, NULL);
        if(client < 0)
            continue;

        Socket *clientSocket = new Socket(client, this);
        m_clients.push_back(std::shared_ptr<Socket>(clientSocket));
        if(m_onClientConnectHandler)
            m_onClientConnectHandler(clientSocket);
    }
}

void SocketServer::sendToAll(const std::string &message)
{
    for(auto client : m_clients)
        client->sendString(message);
}

void SocketServer::messageReceived(Socket *socket, const std::string &message)
{
    if(message == "close")
    {
        for(std::list<std::shared_ptr<Socket> >::iterator i = m_clients.begin();
            i != m_clients.end(); i++)
        {
            if((*i)->getId() == socket->getId())
            {
                m_clients.erase(i);
                return;
            }
        }
    }

    if(m_onMessageHandler)
        m_onMessageHandler(socket, message);
}
