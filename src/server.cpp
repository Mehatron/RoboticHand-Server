#include "server.h"

#include <iostream>
#include <boost/bind.hpp>

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

        m_WSServer.init_asio();
        m_WSServer.listen(Server::WSPORT);
        m_WSServer.start_accept();

        m_roboticHand.open("/dev/ttyUSB1", "/dev/ttyUSB0");
        m_roboticHand.unlock();
        m_roboticHand.setMode(RoboticHand::ModeManual);
        m_roboticHand.moveUp();
        m_roboticHand.moveLeft();
        m_roboticHand.unextend();
        m_roboticHand.setMode(RoboticHand::ModeAutomatic);
        m_roboticHand.lock();
        m_roboticHand.updateState();

        m_roboticHand.start();
        m_socketServer.start(Server::PORT);
        m_WSServer.run();
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
    json state = stateToJSON(m_roboticHand.getState());
    m_WSServer.send(hdl, state.dump(), websocketpp::frame::opcode::text);
}

void Server::onClientConnected(Socket *client)
{
    json state = stateToJSON(m_roboticHand.getState());
    client->sendString(state.dump());
}

void Server::onClientDisconnected(const websocketpp::connection_hdl &hdl)
{
    m_clients.erase(hdl);
}

void Server::onMessageReceived(const std::string &message)
{
    try {
        std::string command = message;
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
                else if(state.rotationDown && state.extendsUnextended)
                    m_roboticHand.extend();
            } else if(command == "motor1_start")
            {
                m_roboticHand.motor1Start();
            } else if(command == "motor1_stop")
            {
                m_roboticHand.motor1Stop();
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
    json data = stateToJSON(state);
    m_socketServer.sendToAll(data.dump());
    for(auto client : m_clients)
        m_WSServer.send(client, data.dump(), websocketpp::frame::opcode::text);
}

void Server::setupHandlers(void)
{
    m_WSServer.set_open_handler([this](const websocketpp::connection_hdl &hdl)
        {
            onClientConnected(hdl);
        });
    m_socketServer.setOnClientConnectHandler([this](Socket *client)
        {
            onClientConnected(client);
        });
    m_WSServer.set_close_handler([this](const websocketpp::connection_hdl &hdl)
        {
            onClientDisconnected(hdl);
        });
    m_WSServer.set_message_handler([this](const websocketpp::connection_hdl &hdl,
                                        const WSServer::message_ptr &msg)
        {
            onMessageReceived(msg->get_payload());
        });
    m_socketServer.setOnMessageHandler([this](const Socket *socket, const std::string &message)
        {
            onMessageReceived(message);
        });
    m_roboticHand.setOnStateChangedHandler([this](const RoboticHand::State &state)
        {
            onRoboticHandStateChanged(state);
        });
}

json Server::stateToJSON(const RoboticHand::State &state) const
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

    return data;
}
