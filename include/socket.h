#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <string>
#include <thread>

class SocketServer;

class Socket
{
public:
    explicit Socket(int id, SocketServer *server);
    virtual ~Socket(void);

    static const int BUFFER_SIZE                    = 1024;

    void disconnect(void);
    void sendString(const std::string &message);

    inline int getId(void) const { return m_socket; };

private:
    void receiving(void);

    SocketServer *m_server;
    int m_socket;

    std::thread m_receivingThread;
};

#endif // _SOCKET_H_
