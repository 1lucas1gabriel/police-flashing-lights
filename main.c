// Flashing lights with non-blocking function
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
						70, 
						button_released, 
						button_released,
						false};
						
	LED greenLED	  = {GPIOC, 
						GPIO13,
						RCC_GPIOC,
						led_status_off,
						0,
						250,
						500};
	
	clock_setup();
	systick_setup();
	
	ButtonInit(&pushbutton);
	LEDInit(&greenLED);
	
	LEDActuate(&greenLED, led_actuate_off);
	delay(1000);
	LEDActuate(&greenLED, led_actuate_on);
	delay(1000);
	
		
	while(1) {
		
		ButtonRead(&pushbutton);
		
		if(pushbutton.risingEdgeFound == true){
			pushbutton.risingEdgeFound = false; // ACK to rising edge
			greenLED.ledState = (greenLED.ledState == !led_status_flashing) ? led_status_flashing : led_status_off;
		}
		
		if(greenLED.ledState == led_status_flashing){
			LEDActuate(&greenLED, led_actuate_flashing);
		}
		else{
			LEDActuate(&greenLED, led_actuate_off);
		}
	}
	return 0;
}
	
	
