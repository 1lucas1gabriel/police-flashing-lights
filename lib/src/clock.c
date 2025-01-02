#include "clock.h"


void systick_setup(void){
    // Set the systick clock source to our main clock
    systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
    // Clear the Current Value Register so that we start at 0
    STK_CVR = 0;
    // In order to trigger an interrupt every millisecond, we can set the reload
    // value to be the speed of the processor / 1000 -1
    systick_set_reload(rcc_ahb_frequency / 1000 - 1);
    // Enable interrupts from the system tick clock
    systick_interrupt_enable();
    // Enable the system tick counter
    systick_counter_enable();
}


/* This is our interrupt handler for the systick reload interrupt. 
 * Full list of interrupt services routines in /libopencm3/stm32/f0/nvic.h */
void sys_tick_handler(void){
    // Increment our monotonic clock
    _millis++;
}


/* Return timer in msecs for execution time since boot up */
uint64_t millis(void){
    return _millis;
}


/* Delay for a real number of milliseconds. */
void delay(uint32_t duration){
    const uint32_t until = millis() + duration;
    while (millis() < until);
}
