#ifndef _WEBSOCKET_H_
#define _WEBSOCKET_H_

#include <functional>
#include <set>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include "json.hpp"

#include "robotichand.h"
#include "socketserver.h"

class Server
{
public:
    Server(void);
    ~Server(void);

    const int WSPORT        = 8272;
    const int PORT          = 8273;

    void start(void);
    void toggleLock(void);

private:
    typedef websocketpp::server<websocketpp::config::asio> WSServer;
    typedef std::set<websocketpp::connection_hdl,
                     std::owner_less<websocketpp::connection_hdl>> WSClientList;

    void onClientConnected(const websocketpp::connection_hdl &hdl);
    void onClientConnected( Socket *client);
    void onClientDisconnected(const websocketpp::connection_hdl &hdl);
    void onMessageReceived(const std::string &message);
    void onRoboticHandStateChanged(const RoboticHand::State &state);
    void setupHandlers(void);

    nlohmann::json stateToJSON(const RoboticHand::State &state) const;

    WSServer m_WSServer;
    WSClientList m_clients;
    SocketServer m_socketServer;

    RoboticHand m_roboticHand;
};

#endif // _WEBSOCKET_H_
