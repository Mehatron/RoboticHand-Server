#include "robotichand.h"

#include <string>
#include <facom/facom.h>
#include <facom/error.h>

#include "exception.h"

RoboticHand::RoboticHand(void)
    : m_open(false)
{
}


RoboticHand::RoboticHand(const std::string &port)
    : m_open(false)
{
    try {
        open(port);
    } catch(Exception &ex) {}
}

RoboticHand::~RoboticHand(void)
{
    try {
        close();
    } catch(Exception &ex) {}
}

void RoboticHand::open(const std::string &port)
{
    int err = FACOM_open(port.c_str(), 0x01);
    if(err < 0)
        throw Exception("FACOM error: " + std::to_string(err), err);
    m_open = true;
}

void RoboticHand::close()
{
    int err = FACOM_close();
    if(err < 0)
        throw Exception("FACOM error: " + std::to_string(err), err);
}

void RoboticHand::sendAction(RoboticHand::Action action)
{
    int err = FACOM_setDiscrete(DISCRETE_M, action, ACTION_SET);
    if(err < 0)
        throw Exception("FACOM error: " + std::to_string(err), err);
}

void RoboticHand::setMode(Mode mode)
{
    int err = 0;
    if(mode == ModeAutomatic)
        err = FACOM_setDiscrete(DISCRETE_M, 4, ACTION_SET);
    else
        err = FACOM_setDiscrete(DISCRETE_M, 4, ACTION_RESET);

    if(err < 0)
        throw Exception("FACOM error: " + std::to_string(err), err);
}

void RoboticHand::moveUp(void)
{
    sendAction(ActionMoveUp);
}

void RoboticHand::moveDown(void)
{
    sendAction(ActionMoveDown);
}

void RoboticHand::moveRight(void)
{
    sendAction(ActionMoveRight);
}

void RoboticHand::moveLeft(void)
{
    sendAction(ActionMoveLeft);
}

void RoboticHand::rotateUp(void)
{
    sendAction(ActionRotateUp);
}

void RoboticHand::rotateDown(void)
{
    sendAction(ActionRotateDown);
}

void RoboticHand::extend(void)
{
    sendAction(ActionExtend);
}

void RoboticHand::unextend(void)
{
    sendAction(ActionUnExtend);
}

void RoboticHand::pick(void)
{
    sendAction(ActionPick);
}

void RoboticHand::place(void)
{
    sendAction(ActionPlace);
}
