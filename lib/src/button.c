#include "button.h"


void ButtonInit(Button *button){
	/* Enable clock on GPIO PORT. */
	rcc_periph_clock_enable(button->GPIO_Clken);

	/* Set GPIO Pin to 'input open-drain'. */
	gpio_set_mode(button->GPIOPort, GPIO_MODE_INPUT, 
				GPIO_CNF_INPUT_FLOAT, button->GPIOPin);
}


button_states_t ButtonRead(Button *button){

	// output format is 2bytes with full PORT reading e.g 0000000000001000
	uint16_t reading = gpio_get(button->GPIOPort, button->GPIOPin);

	// reading should be 0 or 1 for a specific GPIO pin
	button_states_t btn_reading = (bool)((reading) & (button->GPIOPin)); 
	
	// marks time when any button transition is detected
	if(btn_reading != button->lastButtonReading){
	
    	button->lastDebounceTime = millis();
  	}
	
	// applies debounce time filter from last button transition  
  	if((millis() - button->lastDebounceTime) > button->debounceDelay){
  	
  		// updates button status to new debounced button state				
  	    if(btn_reading != button->debouncedButtonState){
    		button->debouncedButtonState = btn_reading;
 
    		if(button->debouncedButtonState == (button_pressed)){
    			// set flag to inform rising edge of debounced button status. Flag consumers must set down (acknowledge)
    	 		button->risingEdgeFound = true;
    	 	}
			else{ // debouncedButtonState == (button_released)
				// set flag to inform falling edge of debounced button status. Flag consumers must set down (acknowledge)
    	 		button->fallingEdgeFound = true;
			}
    	}
  	}
  	
	button->lastButtonReading = btn_reading;
	
	return button->debouncedButtonState;
}
