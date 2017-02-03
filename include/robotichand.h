#ifndef _ROBOTIC_HAND_H_
#define _ROBOTIC_HAND_H_

#include <string>
#include <ostream>

class RoboticHand
{
public:
    RoboticHand(void);
    RoboticHand(const std::string &port);
    ~RoboticHand(void);

    enum Mode {
        ModeAutomatic = 0,
        ModeManual
    };

    struct State
    {
        Mode mode;
        bool constructionDown       = false;
        bool constructionUp         = false;
        bool left                   = false;
        bool right                  = false;
        bool rotationDown           = false;
        bool rotationUp             = false;
        bool extendsUnextended      = false;
        bool extendsExtended        = false;
        bool picked                 = false;
    };

    void open(const std::string &port);
    void close(void);

    State getState(void) const { return m_state; };
    Mode getMode(void) const { return m_state.mode; };

    void moveUp(void);
    void moveDown(void);
    void moveRight(void);
    void moveLeft(void);
    void rotateUp(void);
    void rotateDown(void);
    void extend(void);
    void unextend(void);
    void pick(void);
    void place(void);

    void setMode(Mode mode);

    void updateState(void);

    friend std::ostream &operator<<(std::ostream &lhs, const RoboticHand &rhs);

private:
    enum Action {
        ActionMoveUp        = 40,
        ActionMoveDown      = 41,
        ActionMoveRight     = 42,
        ActionMoveLeft      = 43,
        ActionRotateUp      = 44,
        ActionRotateDown    = 45,
        ActionExtend        = 46,
        ActionUnExtend      = 47,
        ActionPick          = 48,
        ActionPlace         = 49
    };

    void sendAction(Action action);

    bool m_open;
    State m_state;
};

bool operator==(const RoboticHand::State &lhs, const RoboticHand::State &rhs);
/*
bool operator!=(const RoboticHand::State &lhs, const RoboticHand::State &rhs)
    { return !operator==(lhs, rhs); };
*/

#endif // _ROBOTIC_HAND_H_