// Flashing lights with non-blocking function
// task1: Implement compact read_button function
// task 2: Implement compact flashing lights function
// task 3: refactor code (create libs, macros, decoupled architecture)

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/systick.h>


typedef enum{
	led_off,
	led_on,
	led_flashing

} led_states_t;


typedef struct{
	const uint32_t 	GPIOPort;
	const uint16_t 	GPIOPin;
	led_states_t 	led_state;
	// inverted logic or open-drain etc type

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
	rcc_periph_clock_enable(RCC_GPIOC);

	/* Set GPIO Pin to 'output push-pull' */
	gpio_set_mode(led->GPIOPort, GPIO_MODE_OUTPUT_50_MHZ,
				GPIO_CNF_OUTPUT_PUSHPULL, led->GPIOPin);
}



int main(void)
{
	LED greenLED	  = {GPIOC, 
						GPIO13, 
						led_off};
	clock_setup();
	systick_setup();
	LEDInit(&greenLED);
	
	gpio_clear(GPIOC, GPIO13);	//on
	delay(1000);
	gpio_set(GPIOC, GPIO13);	// off
	delay(1000);
	
	uint32_t lastActivationTime   = 0;  
	uint32_t lightActuation_delay = 100; // (msecs) commands flashing duty cycle: 0 <  lightActuation_delay < lightActuation_T; 
	uint32_t lightActuation_T = 1000;  // (msecs) light actuation period

		
	while(1) {
			if(millis() - lastActivationTime > lightActuation_T){
				gpio_clear(GPIOC, GPIO13);	//on
				lastActivationTime = millis();
			}
			if((millis() - (lastActivationTime - lightActuation_delay)) > lightActuation_T){
				gpio_set(GPIOC, GPIO13);	//off
			}
	}
	return 0;
}
