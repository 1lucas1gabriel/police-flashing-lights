#include "led.h"


void LEDInit(LED *led){
	/* Enable clock on GPIO PORT */
	rcc_periph_clock_enable(led->GPIO_Clken);
	
	/* Set GPIO Pin to 'output push-pull' */
	gpio_set_mode(led->GPIOPort, GPIO_MODE_OUTPUT_50_MHZ,
				GPIO_CNF_OUTPUT_PUSHPULL, led->GPIOPin);
	
	/* Setup LED initial state */
	LEDActuate(led, led->ledState);
}


void LEDActuate(LED *led, led_actuation_t action){

	if(action == led_actuate_off){
		gpio_clear(led->GPIOPort, led->GPIOPin);
		led->ledState = led_status_off;
		led->lastDeactivationTime = millis();
	}
	else if(action == led_actuate_on){
		gpio_set(led->GPIOPort, led->GPIOPin);
		led->ledState = led_status_on;
	}
	else if(action == led_actuate_flashing){

		/* Evaluates Off duration in a given period */
		if((millis() + led->offset - led->lastDeactivationTime) >= led->flashingPeriod){
			gpio_clear(led->GPIOPort, led->GPIOPin);
			led->lastDeactivationTime = millis() + led->offset;
		}
		
		/* Evaluates On duration in a given period */
		if((millis() + led->offset + led->OnPeriod - led->lastDeactivationTime) >= led->flashingPeriod){
			gpio_set(led->GPIOPort, led->GPIOPin);
		}
		led->ledState = led_status_flashing;
	}
	else{ // Not allowed action input status may deactivate LED
		gpio_clear(led->GPIOPort, led->GPIOPin);
		led->ledState = led_status_off;
	}	
}
