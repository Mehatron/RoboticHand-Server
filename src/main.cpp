#include "exception.h"
#include "robotichand.h"
#include "server.h"

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

    Server connection;
    try {
        connection.start();
    } catch(Exception &ex) {
        std::cout << ex << std::endl;
    }

    return 0;
}
