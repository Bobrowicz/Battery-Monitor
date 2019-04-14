/*
 * adc.h
 *
 * Created: 4/11/2019 8:02:20 PM
 *  Author: peter
 */ 


#ifndef ADC_H_
#define ADC_H_


#include <avr/io.h>

void adc_init(void);
void adc_enable(void);
void adc_disable(void);
uint16_t adc_read_single(void);
void adc_select_channel(uint8_t channel);

#endif /* ADC_H_ */