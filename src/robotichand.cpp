#include "robotichand.h"

#include <iostream>
#include <string>
#include <chrono>
#include <facom/facom.h>
#include <facom/error.h>
#include <uniprot/uniprot.h>
#include <uniprot/error.h>

#include <iostream>

#include "exception.h"

RoboticHand::RoboticHand(void)
    : m_open(false),
      m_onStateChangedHandler(nullptr),
      m_updateThreadRunning(false)
{
}

RoboticHand::RoboticHand(const std::string &portFatek, const std::string &portUnitronics)
    : m_open(false),
      m_onStateChangedHandler(nullptr),
      m_updateThreadRunning(false)
{
    open(portFatek, portUnitronics);
}

RoboticHand::~RoboticHand(void)
{
    try {
        if(m_updateThread.joinable())
            m_updateThread.join();
        close();
    } catch(Exception &ex) {}
}

void RoboticHand::open(const std::string &portFatek, const std::string &portUnitronics)
{
    if(m_open)
        return;

    int err = FACOM_open(portFatek.c_str(), 0x01);
    if(err < 0)
        throw Exception("FACOM error: " + std::to_string(err), err);
    err = FACOM_start();
    if(err < 0)
        throw Exception("FACOM error: " + std::to_string(err), err);
    err = UNIPROT_open(portUnitronics.c_str());
    if(err < 0)
        throw Exception("UNIPROT error: " + std::to_string(err), err);

    m_open = true;
}

void RoboticHand::close()
{
    if(!m_open)
        return;

    int err = FACOM_close();
    m_open = false;
    if(err < 0)
        throw Exception("FACOM error: " + std::to_string(err), err);
}

void RoboticHand::sendAction(RoboticHand::Action action)
{
    if(!m_open)
        throw Exception("Connection is not opened");

    std::lock_guard<std::mutex> lockSatate(m_stateMutex);
    int err = FACOM_setDiscrete(DISCRETE_M, action, ACTION_SET);
    if(err < 0)
        throw Exception("FACOM error: " + std::to_string(err), err);
}

void RoboticHand::setMode(Mode mode)
{
    if(!m_open)
        throw Exception("Connection is not opened");

    int err = 0;
    std::lock_guard<std::mutex> lockState(m_stateMutex);
    if(mode == ModeAutomatic)
        err = FACOM_setDiscrete(DISCRETE_M, 1, ACTION_SET);
    else
        err = FACOM_setDiscrete(DISCRETE_M, 1, ACTION_RESET);

    if(err < 0)
        throw Exception("FACOM error: " + std::to_string(err), err);
}

void RoboticHand::updateState(void)
{
    //std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
    unsigned char sensors[9];
    unsigned char status[11];


    std::lock_guard<std::mutex> lkState(m_stateMutex);
    int err = FACOM_getDiscretes(DISCRETE_X, 0, 9, sensors);
    if(err < 0)
        throw Exception("FACOM error: " + std::to_string(err), err);
    err = FACOM_getDiscretes(DISCRETE_M, 0, 11, status);
    if(err < 0)
        throw Exception("FACOM error: " + std::to_string(err), err);

    State currentState;
    currentState.constructionDown = sensors[0];
    currentState.constructionUp = sensors[1];
    currentState.left = sensors[2];
    currentState.right = sensors[3];
    currentState.rotationDown = sensors[4];
    currentState.rotationUp = sensors[5];
    currentState.extendsUnextended = sensors[6];
    currentState.extendsExtended = sensors[7];
    currentState.picked = !sensors[8];

    if(status[1])
    {
        currentState.mode = ModeAutomatic;

        if(currentState.constructionUp &&
           currentState.right &&
           currentState.rotationUp &&
           currentState.extendsUnextended &&
           !currentState.picked)
        {
            int err = 0;
            if(status[8])
                err = UNIPROT_write("11");
            if(status[9])
                err = UNIPROT_write("21");
            if(status[10])
                err = UNIPROT_write("31");

            if(err < 0)
                throw Exception("UNIPROT error: " + std::to_string(err), err);
        }
    }
    else
    {
        currentState.mode = ModeManual;
    }

    if(currentState != m_state)
    {
        m_state = currentState;

        if(m_onStateChangedHandler)
            m_onStateChangedHandler(m_state);
    }
    //std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
    //auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    //std::cout << "Time: " << diff.count() << std::endl;
}

/*
 * Run on separate thread
 */
void RoboticHand::update(void)
{
    std::unique_lock<std::mutex> lkRunning(m_updateThreadRunningMutex);
    while(m_updateThreadRunning)
    {
        while(m_cvUpdateThreadRunning.wait_for(lkRunning, std::chrono::nanoseconds(1))
            == std::cv_status::timeout)
        {
            updateState();
        }
    }
}

void RoboticHand::start(void)
{
    if(m_updateThreadRunning)
        throw Exception("RoboticHand is already started");

    std::lock_guard<std::mutex> lkRunning(m_updateThreadRunningMutex);
    m_updateThreadRunning = true;
    m_updateThread = std::thread([this]() {
            update();
        });
}

void RoboticHand::stop(void)
{
    if(!m_updateThreadRunning)
        throw Exception("RoboticHand is already stoped");

    std::lock_guard<std::mutex> lkRunning(m_updateThreadRunningMutex);
    m_updateThreadRunning = false;
    m_cvUpdateThreadRunning.notify_all();
}

void RoboticHand::lock(void)
{
    std::lock_guard<std::mutex> lkRunning(m_updateThreadRunningMutex);
    FACOM_setDiscrete(DISCRETE_M, 0, ACTION_RESET);
}

void RoboticHand::unlock(void)
{
    std::lock_guard<std::mutex> lkRunning(m_updateThreadRunningMutex);
    FACOM_setDiscrete(DISCRETE_M, 0, ACTION_SET);
}

bool operator==(const RoboticHand::State &lhs, const RoboticHand::State &rhs)
{
    if(lhs.mode != rhs.mode ||
        lhs.constructionDown != rhs.constructionDown ||
        lhs.constructionUp != rhs.constructionUp ||
        lhs.left != rhs.left ||
        lhs.right != rhs.right ||
        lhs.rotationDown != rhs.rotationDown ||
        lhs.rotationUp != rhs.rotationUp ||
        lhs.extendsUnextended != rhs.extendsUnextended ||
        lhs.extendsExtended != rhs.extendsExtended ||
        lhs.picked != rhs.picked)
        return false;

    return true;
}

std::ostream &operator<<(std::ostream &lhs, const RoboticHand &rhs)
{
    RoboticHand::State state = rhs.m_state;

    lhs << "{" << std::endl
        << "\t" << "Mode:          " << (state.mode == RoboticHand::ModeAutomatic
                                            ? "Automatic" : "Manual") << std::endl
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
