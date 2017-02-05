#include "exception.h"
#include "robotichand.h"
#include "websocket.h"

#include <iostream>

int main(void)
{
    /*
    RoboticHand roboticHand;

    try {
        roboticHand.open("/dev/ttyUSB0");
        roboticHand.setMode(RoboticHand::ModeManual);
        roboticHand.moveUp();
        roboticHand.updateState();
        std::cout << roboticHand;
        roboticHand.close();
    } catch(Exception &ex) {
        std::cout << ex << std::endl;
    }
    */

    WebSocket connection;
    try {
        connection.start(8272);
    } catch(Exception &ex) {
        std::cout << ex << std::endl;
    }

    return 0;
}
