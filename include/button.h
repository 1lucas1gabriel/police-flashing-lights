/**************************************************************************
 * General Push Button library
 * Lucas Gabriel Cosmo Morais
 * 12/2024
 **************************************************************************/
#ifndef BUTTON_H
#define BUTTON_H

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include "clock.h"



typedef enum{
	button_released,	//off
	button_pressed		//on

} button_states_t;


typedef struct{
	const uint32_t 	GPIOPort;
	const uint16_t 	GPIOPin;
	enum rcc_periph_clken GPIO_Clken;
	uint32_t 		lastDebounceTime;		// last time input pin status has transitioned
	uint16_t 		debounceDelay;			// configurable debounce time (msecs)
	button_states_t lastButtonReading;		// previous reading from the input pin
	button_states_t debouncedButtonState;	// current debounced button state
	bool risingEdgeFound;					// rising edge flag. >> Consumers must set false to acknowledge <<
	bool fallingEdgeFound;					// falling edge flag. >> Consumers must set false to acknowledge <<
	uint32_t 		lastRisingEdgeTime;
	uint32_t 		lastFallingEdgeTime;
} Button;


void ButtonInit(Button *button);
button_states_t ButtonRead(Button *button);
void ButtonDetectEdges(Button *button);


#endif	// BUTTON_H
