// Flashing lights with non-blocking function
// task1: Implement compact read_button function
// task 2: Implement compact flashing lights function
// task 3: refactor code (create libs, macros, decoupled architecture)

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/systick.h>
//#include "button.h"

typedef enum{
	button_released,//off
	button_pressed	//on

} button_states_t;


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
	uint32_t 		lastDebounceTime;		// last time input pin status has transitioned
	uint16_t 		debounceDelay;			// configurable debounce time (msecs)
	button_states_t lastButtonReading;		// previous reading from the input pin
	button_states_t debouncedButtonState;	// current debounced button state
	bool risingEdgeFound;

} Button;


typedef struct{
	const uint32_t 	GPIOPort;
	const uint16_t 	GPIOPin;
	enum rcc_periph_clken GPIO_Clken;
	led_states_t 	ledState;
	uint32_t 		lastActivationTime; 	// last time led was active
	uint32_t 		offPeriod; 				// control (duty cycle) time period led stays off (msecs)
	uint32_t 		flashingPeriod;		  	// light flashing period (msecs)
	
} LED;

//=======================================================================================
// Storage for our monotonic system clock.
// Note that it needs to be volatile since we're modifying it from an interrupt.
static volatile uint32_t _millis = 0;


void systick_setup(void) {
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



uint64_t millis(void) {
    return _millis;
}

// This is our interrupt handler for the systick reload interrupt.
// The full list of interrupt services routines that can be implemented is
// listed in libopencm3/include/libopencm3/stm32/f0/nvic.h
void sys_tick_handler(void) {
    // Increment our monotonic clock
    _millis++;
}

/**
 * Delay for a real number of milliseconds
 */
void delay(uint32_t duration) {
    const uint32_t until = millis() + duration;
    while (millis() < until);
}
//=======================================================================================



/* Set STM32 to 48 MHz. */
static void clock_setup(void){
	rcc_clock_setup_pll(&rcc_hsi_configs[RCC_CLOCK_HSI_48MHZ]);
	// or rcc_clock_setup_in_hsi_out_48mhz()

}


static void LEDInit(LED *led){
	/* Enable clock on GPIO PORT */
	rcc_periph_clock_enable(led->GPIO_Clken);
	
	/* Set GPIO Pin to 'output push-pull' */
	gpio_set_mode(led->GPIOPort, GPIO_MODE_OUTPUT_50_MHZ,
				GPIO_CNF_OUTPUT_PUSHPULL, led->GPIOPin);
}


static void LEDActuate(LED *led, led_actuation_t action){

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




static void ButtonInit(Button *button){
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
    	}
  	}
  	
	button->lastButtonReading = btn_reading;
	
	return button->debouncedButtonState;
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
	
	
