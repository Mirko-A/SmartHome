#include "hal.h"

// #include <wiringPi.h>

static int wiringPiSetup() {
    // Stub: Assume success for now.
    return 0;
}

HAL::HAL() {
    m_Initialized = (wiringPiSetup() >= 0);
};
