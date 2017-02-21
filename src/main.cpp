#include "exception.h"
#include "robotichand.h"
#include "server.h"

#include <iostream>
#include <csignal>
#include <unistd.h>

Server server;

void signalHandler(int signum)
{
    std::cout << signum << std::endl;
    switch(signum)
    {
        case SIGINT:
            std::exit(0);
            break;
        case SIGUSR1:
            server.toggleLock();
            break;
    }
}

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

    std::signal(SIGINT, signalHandler);
    std::signal(SIGUSR1, signalHandler);

    try {
        server.start();
    } catch(Exception &ex) {
        std::cout << ex << std::endl;
    }

    return 0;
}
