#include "websocket.h"

#include <iostream>
#include <boost/bind.hpp>
#include "json.hpp"

#include "exception.h"

using nlohmann::json;

WebSocket::WebSocket(void)
{
}

WebSocket::~WebSocket(void)
{
}

void WebSocket::start(int port)
{
    //m_server.set_message_handler(websocketpp::lib::bind(&onMessageRecived, this));
    //m_server.set_message_handler(boost::bind(&WebSocket::onMessageRecived, this));
    try {
        setupHandlers();
        m_server.init_asio();
        m_server.listen(port);
        m_server.start_accept();

        m_server.run();
    } catch(websocketpp::exception &ex) {
        throw Exception(ex.what());
    }
}

void WebSocket::onClientConnected(const websocketpp::connection_hdl &hdl)
{
    m_clients.insert(hdl);
}

void WebSocket::onClientDisconnected(const websocketpp::connection_hdl &hdl)
{
    m_clients.erase(hdl);
}

void WebSocket::onMessageRecived(const websocketpp::connection_hdl &hdl,
                                 const WSServer::message_ptr &msg)
{
    std::cout << msg->get_payload() << std::endl;
    for(auto client : m_clients)
    {
        try {
            m_server.send(client, msg);
            m_server.send(client, "Test!", websocketpp::frame::opcode::text);
        } catch(websocketpp::exception &ex) {
            throw Exception(ex.what());
        }
    }
}

void WebSocket::setupHandlers(void)
{
    m_server.set_open_handler([this](const websocketpp::connection_hdl &hdl)
        {
            onClientConnected(hdl);
        });
    m_server.set_close_handler([this](const websocketpp::connection_hdl &hdl)
        {
            onClientDisconnected(hdl);
        });
    m_server.set_message_handler([this](const websocketpp::connection_hdl &hdl,
                                        const WSServer::message_ptr &msg)
        {
            onMessageRecived(hdl, msg);
        });
}
