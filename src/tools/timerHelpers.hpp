#ifndef TIMER_HPP
#define TIMER_HPP

#include <stdint.h>

#define TIMER_A 0x900C0000
#define TIMER_A_LOAD (*(volatile uint32_t*)TIMER_A)
#define TIMER_A_VALUE (*(volatile uint32_t*)(TIMER_A + 0x04))
#define TIMER_A_CONTROL (*(volatile uint8_t*)(TIMER_A + 0x08))
#define TIMER_A_INT_CLEAR (*(volatile uint32_t*)(TIMER_A + 0x0C))
#define TIMER_A_INT_STATUS (*(volatile uint32_t*)(TIMER_A + 0x10))
#define TIMER_A_BGLOAD (*(volatile uint32_t*)(TIMER_A + 0x18))
#define TIMER_B 0x900D0000
#define TIMER_B_LOAD (*(volatile uint32_t*)TIMER_B)
#define TIMER_B_VALUE (*(volatile uint32_t*)(TIMER_B + 0x04))
#define TIMER_B_CONTROL (*(volatile uint8_t*)(TIMER_B + 0x08))
#define TIMER_B_INT_CLEAR (*(volatile uint32_t*)(TIMER_B + 0x0C))
#define TIMER_B_INT_STATUS (*(volatile uint32_t*)(TIMER_B + 0x10))
#define TIMER_B_BGLOAD (*(volatile uint32_t*)(TIMER_B + 0x18))

enum TimerSelect {
    TIMER_A_SEL = 0,
    TIMER_B_SEL = 1,
};

enum TimerStatus {
    TIMER_OK = 0,
    TIMER_ERR_INVALID_PRESCALE = 1,
    ERR_INVALID_TIMER = 2
};

enum TimerPrescale {
    PRESCALE_DIV1   = 0b00, // divide by 1
    PRESCALE_DIV16  = 0b01, // divide by 16
    PRESCALE_DIV256 = 0b10 // divide by 256
    // 0b11 is invalid
};

/**
 * Initialise timer x
 *
 * @param timerX 0 = Timer 1 selected, 1 = Timer 2 selected.
 * @param enable 0 = Timer module disabled, 1 = Timer module enabled.
 * @param mode 0 = Timer module is in free-running mode, 1 = Timer module is in periodic mode.
 * @param intEnable 0 = Timer module Interrupt disabled, 1 = Timer module Interrupt enabled.
 * @param prescale 00 = 0 stages of prescale, clock is divided by 1, 01 = 4 stages of prescale, clock is divided by 16, 10 = 8 stages of prescale, clock is divided by 256, 11 = Undefined, do not use.
 * @param size 0 = 16-bit counter, 1 = 32-bit counter.
 * @param oneShot 0 = wrapping mode, 1 = one-shot mode.
 * @return TimerStatus
 */
TimerStatus TIMER_SETUP(TimerSelect timerX, bool enable, bool mode, bool intEnable, TimerPrescale prescale, bool size, bool oneShot);

#endif