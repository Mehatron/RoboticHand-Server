#include "exception.h"
#include "robotichand.h"

int main(void)
{
    RoboticHand roboticHand;

    try {
        roboticHand.open("/dev/ttyUSB0");
        roboticHand.setMode(RoboticHand::ModeManual);
        roboticHand.moveDown();
        roboticHand.close();
    } catch(Exception &ex) {}

    return 0;
}
