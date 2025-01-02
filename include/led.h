/**************************************************************************
 * General LED library
 * Lucas Gabriel Cosmo Morais
 * 12/2024
 **************************************************************************/
#ifndef LED_H
#define LED_H

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include "clock.h"


typedef enum{
	led_status_off,
	led_status_on,
	led_status_flashing

} led_states_t;


typedef enum{
	led_actuate_off,
	led_actuate_on,
	led_actuate_flashing

} led_actuation_t;


typedef struct{
	const uint32_t 	GPIOPort;
	const uint16_t 	GPIOPin;
	enum rcc_periph_clken GPIO_Clken;
	led_states_t 	ledState;
	uint32_t 		lastActivationTime; 	// last time led was active
	uint32_t 		offPeriod; 				// control (duty cycle) time period led stays off (msecs)
	uint32_t 		flashingPeriod;		  	// light flashing period (msecs)
	
} LED;


void LEDInit(LED *led);
void LEDActuate(LED *led, led_actuation_t action);


#endif //LED_H
