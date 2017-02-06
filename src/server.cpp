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
    sendState(hdl, m_roboticHand.getState());
}

void Server::onClientDisconnected(const websocketpp::connection_hdl &hdl)
{
    m_clients.erase(hdl);
}

void Server::onMessageRecived(const websocketpp::connection_hdl &hdl,
                              const WSServer::message_ptr &msg)
{
    try {
        std::string command = msg->get_payload();
        if(command == "mode_automatic")
            m_roboticHand.setMode(RoboticHand::ModeAutomatic);
        else if(command == "mode_manual")
            m_roboticHand.setMode(RoboticHand::ModeManual);
        else if(command == "move_up")
            m_roboticHand.moveUp();
        else if(command == "move_down")
            m_roboticHand.moveDown();
        else if(command == "move_right")
            m_roboticHand.moveRight();
        else if(command == "move_left")
            m_roboticHand.moveLeft();
        else if(command == "rotate_up")
            m_roboticHand.rotateUp();
        else if(command == "rotate_down")
            m_roboticHand.rotateDown();
        else if(command == "extend")
            m_roboticHand.extend();
        else if(command == "unextend")
            m_roboticHand.unextend();
        else if(command == "pick")
            m_roboticHand.pick();
        else if(command == "place")
            m_roboticHand.place();
    } catch(Exception &ex) {
        std::cout << ex << std::endl;
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

void Server::sendState(const websocketpp::connection_hdl &client,
                       const RoboticHand::State &state)
{
    json data = {
        { "mode", state.mode == RoboticHand::ModeAutomatic ? "automatic" : "manual" },
        { "construction_down", state.constructionDown },
        { "construction_up", state.constructionUp },
        { "left", state.left },
        { "right", state.right },
        { "rotation_down", state.rotationDown },
        { "rotation_up", state.rotationUp },
        { "extends_unextended", state.extendsUnextended },
        { "extends_extended", state.extendsExtended },
        { "picked", state.picked }
    };
    std::cout << data.dump(4) << std::endl;
    m_server.send(client, data.dump(4), websocketpp::frame::opcode::text);
}
