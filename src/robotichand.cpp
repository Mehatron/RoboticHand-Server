#include "robotichand.h"

#include <iostream>
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

void RoboticHand::updateState(void)
{
    unsigned char sensors[9];
    unsigned char automatic[1];

    int err = FACOM_getDiscretes(DISCRETE_X, 0, 9, sensors);
    if(err < 0)
        throw Exception("FACOM error: " + std::to_string(err), err);
    err = FACOM_getDiscretes(DISCRETE_M, 0, 1, automatic);
    if(err < 0)
        throw Exception("FACOM error: " + std::to_string(err), err);

    State currentState = m_state;
    m_state.constructionDown = sensors[0];
    m_state.constructionUp = sensors[1];
    m_state.left = sensors[2];
    m_state.right = sensors[3];
    m_state.rotationDown = sensors[4];
    m_state.rotationUp = sensors[5];
    m_state.extendsUnextended = sensors[6];
    m_state.extendsExtended = sensors[7];
    m_state.picked = !sensors[8];

    if(automatic[0])
        m_state.mode = ModeAutomatic;
    else
        m_state.mode = ModeManual;

    if(m_state == currentState)
        return;
}

bool operator==(const RoboticHand::State &lhs, const RoboticHand::State &rhs)
{
    if(lhs.mode != rhs.mode)
        return false;
    if(lhs.constructionDown != rhs.constructionDown)
        return false;
    if(lhs.constructionUp != rhs.constructionUp)
        return false;
    if(lhs.left != rhs.left)
        return false;
    if(lhs.right != rhs.right)
        return false;
    if(lhs.rotationDown != rhs.rotationDown)
        return false;
    if(lhs.rotationUp != rhs.rotationUp)
        return false;
    if(lhs.extendsUnextended != rhs.extendsUnextended)
        return false;
    if(lhs.extendsExtended != rhs.extendsExtended)
        return false;
    if(lhs.picked != rhs.picked)
        return false;

    return true;
}

std::ostream &operator<<(std::ostream &lhs, const RoboticHand &rhs)
{
    RoboticHand::State state = rhs.m_state;

    lhs << "{" << std::endl
        << "\t" << "Down:          " << state.constructionDown << std::endl
        << "\t" << "Up:            " << state.constructionUp << std::endl
        << "\t" << "Left:          " << state.left << std::endl
        << "\t" << "Right:         " << state.right << std::endl
        << "\t" << "Rotation Up :  " << state.rotationUp << std::endl
        << "\t" << "Rotation Down: " << state.rotationDown << std::endl
        << "\t" << "Extended:      " << state.extendsExtended << std::endl
        << "\t" << "UnExteded:     " << state.extendsUnextended << std::endl
        << "\t" << "Picked:        " << state.picked << std::endl
        << "}" << std::endl;

    return lhs;
}
