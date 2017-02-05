#ifndef _ROBOTIC_HAND_H_
#define _ROBOTIC_HAND_H_

#include <string>
#include <ostream>
#include <functional>

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

    inline void moveUp(void) { sendAction(ActionMoveUp); };
    inline void moveDown(void) { sendAction(ActionMoveDown); };
    inline void moveRight(void) { sendAction(ActionMoveRight); };
    inline void moveLeft(void) { sendAction(ActionMoveLeft); };
    inline void rotateUp(void) { sendAction(ActionRotateUp); };
    inline void rotateDown(void) { sendAction(ActionRotateDown); };
    inline void extend(void) { sendAction(ActionExtend); };
    inline void unextend(void) { sendAction(ActionUnExtend); };
    inline void pick(void) { sendAction(ActionPick); };
    inline void place(void) { sendAction(ActionPlace); };

    void setMode(Mode mode);

    void setOnStateChangedHandler(std::function<void(const State &state)> &handler)
        { m_onStateChangedHandler = handler; };

    void updateState(void);
    void update(void);

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

    std::function <void(const State &state)> m_onStateChangedHandler;
};

bool operator==(const RoboticHand::State &lhs, const RoboticHand::State &rhs);
inline bool operator!=(const RoboticHand::State &lhs, const RoboticHand::State &rhs)
    { return !operator==(lhs, rhs); };

#endif // _ROBOTIC_HAND_H_
