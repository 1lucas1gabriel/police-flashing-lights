// Flashing lights with non-blocking functions
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/systick.h>
#include "button.h"
#include "led.h"
#include "clock.h"


/* Set STM32 to 48 MHz. */
static void clock_setup(void){
	rcc_clock_setup_pll(&rcc_hsi_configs[RCC_CLOCK_HSI_48MHZ]);
	// or rcc_clock_setup_in_hsi_out_48mhz()

}


int main(void){

	Button pushbutton = {
		GPIOB,
		GPIO11,
		RCC_GPIOB,
		0,				// last time button was debounced
		70, 			// debounce time (msecs)
		button_released,// last button reading
		button_released,// debounced button state
		false,			// rising edge state found
		false,			// falling edge state found
		0,				// last time rising edge found
		0};				// last time falling edge found

	LED redLED = {
		GPIOB,
		GPIO13,
		RCC_GPIOB,
		led_status_off, // initial led state
		0,				// last time led was set Inactive
		250,			// on period
		500, 			// flashing period
		0};	 			// offset

	LED blueLED = {
		GPIOA,
		GPIO12,
		RCC_GPIOA,
		led_status_off,
		0,
		250,
		500,
		250};

	clock_setup();
	systick_setup();
	
	ButtonInit(&pushbutton);
	LEDInit(&blueLED);	
	LEDInit(&redLED);
	
	while(1) {
		
		/* control flashing lights upon short/long button press */
		ButtonDetectEdges(&pushbutton);

		//long long press
		if(pushbutton.lastFallingEdgeTime >= (pushbutton.lastRisingEdgeTime + 1000)){
			blueLED.ledState = led_status_flashing;
			LEDUpdateFlashingRate(&redLED, 200, 100, 0);
			LEDUpdateFlashingRate(&blueLED,200, 100, 100);
		}
		// long press
		else if((pushbutton.lastFallingEdgeTime < (pushbutton.lastRisingEdgeTime + 1000)) &&
				(pushbutton.lastFallingEdgeTime >=(pushbutton.lastRisingEdgeTime + 250))){
			blueLED.ledState = led_status_flashing;
			LEDUpdateFlashingRate(&redLED, 1000, 500, 0);
			LEDUpdateFlashingRate(&blueLED,1000, 500, 500);
		}
		//short press
		else{
			blueLED.ledState = led_status_off;
		} 
		

		if(blueLED.ledState == led_status_flashing){
			LEDActuate(&redLED, led_actuate_flashing);
			LEDActuate(&blueLED, led_actuate_flashing);
		}
		else{
			LEDActuate(&redLED, led_actuate_off);
			LEDActuate(&blueLED, led_actuate_off);
		}
	}
	return 0;
}
	
	
