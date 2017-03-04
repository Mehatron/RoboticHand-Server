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

        m_roboticHand.open("/dev/ttyUSB1", "/dev/ttyUSB0");
        m_roboticHand.lock();
        m_roboticHand.setMode(RoboticHand::ModeAutomatic);
        m_roboticHand.moveUp();
        m_roboticHand.moveLeft();
        m_roboticHand.unextend();
        m_roboticHand.rotateDown();
        m_roboticHand.updateState();

        m_roboticHand.start();
        m_server.run();
    } catch(websocketpp::exception &ex) {
        throw Exception(ex.what());
    }
}

void Server::toggleLock(void)
{
    RoboticHand::Mode mode = m_roboticHand.getMode();
    if(mode == RoboticHand::ModeLock)
        m_roboticHand.unlock();
    else
        m_roboticHand.lock();
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
        else
        {
            RoboticHand::State state = m_roboticHand.getState();

            if(command == "grab_toggle")
            {
                if(state.picked)
                    m_roboticHand.place();
                else
                    m_roboticHand.pick();
            } else if(command == "right_right")
            {
                if(state.rotationUp && state.extendsUnextended)
                    m_roboticHand.extend();
                else if(state.rotationDown)
                    m_roboticHand.rotateUp();
            } else if(command == "right_up")
            {
                if(state.rotationDown && state.extendsExtended)
                    m_roboticHand.unextend();
                else if(state.rotationDown && state.extendsUnextended)
                    m_roboticHand.rotateUp();
            } else if(command == "right_left")
            {
                if(state.rotationUp && state.extendsExtended)
                    m_roboticHand.unextend();
                else if(state.rotationUp && state.extendsUnextended)
                    m_roboticHand.rotateDown();
            } else if(command == "right_down")
            {
                if(state.rotationUp && state.extendsExtended)
                    m_roboticHand.unextend();
                else if(state.rotationUp && state.extendsUnextended)
                    m_roboticHand.rotateDown();
                else if(state.rotationDown && state.extendsExtended)
                    m_roboticHand.extend();
            } else if(command == "motor2_start")
            {
                m_roboticHand.motor2Start();
            } else if(command == "motor2_stop")
            {
                m_roboticHand.motor2Stop();
            } else if(command == "motor3_start_right")
            {
                m_roboticHand.motor3StartRight();
            } else if(command == "motor3_start_left")
            {
                m_roboticHand.motor3StartLeft();
            } else if(command == "motor3_stop")
            {
                m_roboticHand.motor3Stop();
            }
        }
    } catch(Exception &ex) {
        std::cout << ex << std::endl;
    }
}

void Server::onRoboticHandStateChanged(const RoboticHand::State &state)
{
    for(auto client : m_clients)
        sendState(client, state);
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
    m_roboticHand.setOnStateChangedHandler([this](const RoboticHand::State &state)
        {
            onRoboticHandStateChanged(state);
        });
}

void Server::sendState(const websocketpp::connection_hdl &client,
                       const RoboticHand::State &state)
{
    json data = {
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

    switch(state.mode)
    {
        case RoboticHand::ModeAutomatic:
            data["mode"] = "automatic";
            break;
        case RoboticHand::ModeManual:
            data["mode"] = "manual";
            break;
        default:
            data["mode"] = "lock";
            break;
    }

    m_server.send(client, data.dump(), websocketpp::frame::opcode::text);
}
