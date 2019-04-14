/*
 * Battery Level Bar Graph.c
 *
 * Created: 4/7/2019 8:36:00 PM
 * Author : peter
 */ 

#define F_CPU 1200000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <avr/sfr_defs.h>
#include <adc.h>

#define ADC_CH_LED_MODE 0x03	// ADC3
#define ADC_CH_BATT_V	0x02	// ADC2

/* States represent battery voltage levels */
#define EMPTY	0
#define LOW		1
#define MED		2
#define HIGH	3
#define FULL	4

/* Actual battery pack voltage is double */
#define THRESHOLD_LOW	676		// 3.30 V
#define THRESHOLD_MED	707		// 3.45 V
#define THRESHOLD_HIGH	738		// 3.60 V
#define THRESHOLD_FULL	769		// 3.75 V

#define LED_MASK	0x0F

/* Function Prototypes */
void select_led_mode(void);
void update_led_bar_graph(uint8_t level);
void sleep(void);

/* Global Variables */
/* In Dot mode only single LED is lit representing battery charge level */
uint8_t dot_mode[5] = {0x01, 0x01, 0x02, 0x04, 0x08};
/* In Bar mode LEDs up to current battery level are lit */
uint8_t bar_mode[5] = {0x01, 0x01, 0x03, 0x07, 0x0F};
uint8_t *led_mode;


/* Initialize watchdog timer. */
void wdt_init(void) {
	WDTCR |= (1 << WDTIE);      // watchdog timer interrupt enable
	WDTCR |= (1 << WDCE);       // start timed sequence
	//WDTCR |= (1 << WDP2);     // 0.25 sec timeout
	WDTCR |= (1 << WDP1) | (1 << WDP0); // 0.125 sec timeout
}

int main(void)
{
	wdt_init();
	adc_init();
	
	/* LEDs in Dot or Bar mode */
	select_led_mode();
	
	/* Sample battery voltage */
	adc_select_channel(ADC_CH_BATT_V);

	/* Output LEDs connected to PB0 - PB3 */
	DDRB |= LED_MASK;
	
	uint16_t battery_voltage = 0;

	/* Assume initial state */
	uint8_t state = FULL;
	
    while (1) 
    {
		battery_voltage = adc_read_single();
		update_led_bar_graph(state);
		
		switch(state)
		{
			case EMPTY:
				if (battery_voltage > THRESHOLD_LOW) {
					state = LOW;
				}
				break;

			case LOW:
				if (battery_voltage > THRESHOLD_MED) {
					state = MED;
				}
				if (battery_voltage < THRESHOLD_LOW) {
					state = EMPTY;
				}
				break;

			case MED:
				if (battery_voltage > THRESHOLD_HIGH) {
					state = HIGH;
				}
				if (battery_voltage < THRESHOLD_MED) {
					state = LOW;
				}
				break;

			case HIGH:
				if (battery_voltage > THRESHOLD_FULL) {
					state = FULL;
				}
				if (battery_voltage < THRESHOLD_HIGH) {
					state = MED;
				}
				break;

			case FULL:
				if (battery_voltage < THRESHOLD_FULL) {
					state = HIGH;
				}
				break;

			default:
				break;
		}

		sleep();    // power down uC
    }
}

/* Interrupt wakes uC from sleep */
EMPTY_INTERRUPT(WDT_vect);

/* Pull-down resistor is attached to the i/o pin through solder bridge. *
 * Pin is checked after MCU Reset to determine LED output mode.			*
 * If pin is pulled to ground then Dot mode is selected. Otherwise, Bar	*
 * mode is selected. Because output LED is also attached to this pin,	*
 * digital logic level can't be used to determine pin state.			*
 * ADC is used instead.													*/
void select_led_mode(void)
{
	adc_select_channel(ADC_CH_LED_MODE);
	// 0.1 volt is the the threshold
	if(adc_read_single() < 20) {
		led_mode = dot_mode;
	}
	else {
		led_mode = bar_mode;
	}
}

/* Output LEDs are updated each time this function runs.				*
 * At minimum level bottom LED is toggled on and Off.					*
 * At higher levels LEDs are lit depending on selected mode.			*/
void update_led_bar_graph(uint8_t level)
{
	if (level == 0) {
		// Toggle bottom LED
		PORTB ^= led_mode[level];
	}
	else {
		// Clear bottom 4 bits and update
		PORTB &= ~LED_MASK;
		PORTB |= led_mode[level];
	}
}

/* Put uC into sleep mode. */
void sleep(void) {
	set_sleep_mode(SLEEP_MODE_IDLE);
	sleep_enable();
	sei();
	sleep_cpu();
	sleep_disable();
	cli();
}