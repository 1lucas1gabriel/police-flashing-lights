#include "led.h"


void LEDInit(LED *led){
	/* Enable clock on GPIO PORT */
	rcc_periph_clock_enable(led->GPIO_Clken);
	
	/* Set GPIO Pin to 'output push-pull' */
	gpio_set_mode(led->GPIOPort, GPIO_MODE_OUTPUT_50_MHZ,
				GPIO_CNF_OUTPUT_PUSHPULL, led->GPIOPin);
}


void LEDActuate(LED *led, led_actuation_t action){

	if(action == led_actuate_off){
		gpio_clear(led->GPIOPort, led->GPIOPin);
		led->ledState = led_status_off;
	}
	else if(action == led_actuate_on){
		gpio_set(led->GPIOPort, led->GPIOPin);
		led->ledState = led_status_on;
	}
	else if(action == led_actuate_flashing){

		if(millis() - led->lastActivationTime > led->flashingPeriod){
			gpio_set(led->GPIOPort, led->GPIOPin);
			led->lastActivationTime = millis();
		}
		if(millis() - (led->lastActivationTime - led->offPeriod) > led->flashingPeriod){
			gpio_clear(led->GPIOPort, led->GPIOPin);
		}
		led->ledState = led_status_flashing;
	}
	else{ // Not allowed actuation may deactivate LED
		gpio_clear(led->GPIOPort, led->GPIOPin);
		led->ledState = led_status_off;
	}	
}
