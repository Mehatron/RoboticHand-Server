#ifndef _WEBSOCKET_H_
#define _WEBSOCKET_H_

#include <functional>
#include <set>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include "robotichand.h"

class Server
{
public:
    Server(void);
    ~Server(void);

    const int PORT          = 8272;

    void start(void);
    void toggleLock(void);

private:
    typedef websocketpp::server<websocketpp::config::asio> WSServer;
    typedef std::set<websocketpp::connection_hdl,
                     std::owner_less<websocketpp::connection_hdl>> WSClientList;

    void onClientConnected(const websocketpp::connection_hdl &hdl);
    void onClientDisconnected(const websocketpp::connection_hdl &hdl);
    void onMessageRecived(const websocketpp::connection_hdl &hdl,
                          const WSServer::message_ptr &msg);
    void onRoboticHandStateChanged(const RoboticHand::State &state);
    void setupHandlers(void);

    void sendState(const websocketpp::connection_hdl &client,
                   const RoboticHand::State &state);

    WSServer m_server;
    WSClientList m_clients;

    RoboticHand m_roboticHand;
};

#endif // _WEBSOCKET_H_
