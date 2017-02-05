#ifndef _WEBSOCKET_H_
#define _WEBSOCKET_H_

#include <functional>
#include <set>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

class WebSocket
{
public:
    WebSocket(void);
    ~WebSocket(void);

    void start(int port = 8272);

private:
    typedef websocketpp::server<websocketpp::config::asio> WSServer;
    typedef std::set<websocketpp::connection_hdl,
                     std::owner_less<websocketpp::connection_hdl>> WSClientList;

    void onClientConnected(const websocketpp::connection_hdl &hdl);
    void onClientDisconnected(const websocketpp::connection_hdl &hdl);
    void onMessageRecived(const websocketpp::connection_hdl &hdl,
                          const WSServer::message_ptr &msg);
    void setupHandlers(void);

    WSServer m_server;
    WSClientList m_clients;
};

#endif // _WEBSOCKET_H_
