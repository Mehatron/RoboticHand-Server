#include "socket.h"

#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>

#include "exception.h"
#include "socketserver.h"

Socket::Socket(int id, SocketServer *server)
    : m_server(server),
      m_socket(id)
{
    m_receivingThread = std::thread([this]() {
        receiving();
    });
}

Socket::~Socket(void)
{
    disconnect();
}

void Socket::disconnect(void)
{
    close(m_socket);
}

void Socket::sendString(const std::string &message)
{
    if(message.size() > BUFFER_SIZE)
        throw Exception("Meesage too big");

    if(send(m_socket, message.c_str(), BUFFER_SIZE, 0) < 0)
        throw Exception("Can't send message");
}

void Socket::receiving(void)
{
    while(true)
    {
        char buffer[BUFFER_SIZE];
        if(recv(m_socket, buffer, BUFFER_SIZE, 0) <= 0)
            break;

        m_server->messageReceived(this, std::string(buffer));
        if(strcmp(buffer, "close") == 0)
            break;
    }
}
