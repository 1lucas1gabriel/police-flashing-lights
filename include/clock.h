/**************************************************************************
 * General CLOCK library
 * Lucas Gabriel Cosmo Morais
 * 12/2024
 **************************************************************************/
#ifndef CLOCK_H
#define CLOCK_H

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/systick.h>


// Storage for our monotonic system clock.
// Note that it needs to be volatile since we're modifying it from an interrupt.
static volatile uint32_t _millis = 0;


void systick_setup(void);
void sys_tick_handler(void);
uint64_t millis(void);
void delay(uint32_t duration);



#endif // CLOCK_H
