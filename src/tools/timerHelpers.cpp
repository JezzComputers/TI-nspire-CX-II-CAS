#include "timerHelpers.hpp"
#include <nspireio/nspireio.h>

TimerStatus TIMER_SETUP(TimerSelect timerX, bool enable, bool mode, bool intEnable, TimerPrescale prescale, bool size, bool oneShot)
{
    if (timerX == TIMER_A_SEL)
    {
        if (prescale >= 0b11) {
            return TIMER_ERR_INVALID_PRESCALE;
        }
        TIMER_A_CONTROL = (enable & 0x1) << 7 |
        (mode & 0x1) << 6 |
        (intEnable & 0x1) << 5 |
        (prescale & 0x3) << 2 |
        (size & 0x1) << 1 |
        (oneShot & 0x1);
        return TIMER_OK;
    }
    else if (timerX == TIMER_B_SEL)
    {
        if (prescale >= 0b11) {
            return TIMER_ERR_INVALID_PRESCALE;
        }
        TIMER_B_CONTROL = (enable & 0x1) << 7 |
        (mode & 0x1) << 6 |
        (intEnable & 0x1) << 5 |
        (prescale & 0x3) << 2 |
        (size & 0x1) << 1 |
        (oneShot & 0x1);
        return TIMER_OK;
    }
    return ERR_INVALID_TIMER;
}