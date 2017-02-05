#include "server.h"

#include <iostream>
#include <boost/bind.hpp>
#include "json.hpp"

#include "exception.h"

using nlohmann::json;

Server::Server(void)
{
}

Server::~Server(void)
{
}

void Server::start(void)
{
    try {
        setupHandlers();
        m_server.init_asio();
        m_server.listen(Server::PORT);
        m_server.start_accept();

        m_server.run();
    } catch(websocketpp::exception &ex) {
        throw Exception(ex.what());
    }
}

void Server::onClientConnected(const websocketpp::connection_hdl &hdl)
{
    m_clients.insert(hdl);
}

void Server::onClientDisconnected(const websocketpp::connection_hdl &hdl)
{
    m_clients.erase(hdl);
}

void Server::onMessageRecived(const websocketpp::connection_hdl &hdl,
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

void Server::setupHandlers(void)
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
