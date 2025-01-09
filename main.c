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

	Button pushbutton = {GPIOB,
						 GPIO11,
						 RCC_GPIOB,
						 0,
						 70, // debounce time (msecs)
						 button_released,
						 button_released,
						 false,
						 false,
						 0,
						 0};

	LED redLED = {GPIOB,
				  GPIO13,
				  RCC_GPIOB,
				  led_status_off, // initial led state
				  0,
				  250, // on time period
				  500, // flashing period
				  0};  // offset

	LED blueLED = {GPIOA,
				   GPIO12,
				   RCC_GPIOA,
				   led_status_off, // initial led state
				   0,
				   250,
				   500,
				   250}; // offset

	clock_setup();
	systick_setup();
	
	ButtonInit(&pushbutton);
	LEDInit(&blueLED);	
	LEDInit(&redLED);
	
		
	while(1) {
		
		/* control flashing lights upon toggle button
		ButtonRead(&pushbutton);

		if(pushbutton.risingEdgeFound == true){
			blueLED.ledState = (blueLED.ledState == !led_status_flashing) ? led_status_flashing : led_status_off;
			pushbutton.risingEdgeFound = false; // ACK to rising edge flag
		}
		*/
		
		/* control flashing lights upon short/long button press */
		ButtonDetectEdges(&pushbutton);

		//long press
		if(pushbutton.lastFallingEdgeTime >= (pushbutton.lastRisingEdgeTime + 2000)){
			blueLED.ledState = led_status_flashing;
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
			LEDActuate(&blueLED, led_actuate_off);
			LEDActuate(&redLED, led_actuate_off);
		}
	}
	return 0;
}
	
	
