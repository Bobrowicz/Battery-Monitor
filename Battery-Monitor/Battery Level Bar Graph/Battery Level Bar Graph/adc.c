/*
 * adc.c
 *
 * Created: 4/11/2019 8:01:28 PM
 *  Author: peter
 */ 

 #include <adc.h>

 /* Initialize ADC */
 void adc_init()
 {	/* Voltage reference selection */
	 //ADMUX |= (1 << REFS0);	// Enabled: 1.1V internal voltage reference
	 // Disabled: Vcc as reference

	 /* Presentation of the ADC results in ADC Data Register */
	 //ADMUX |= (1 << ADLAR);	// Enabled: left adjusted result
	 // Disabled: right adjusted result

	 /* Input channel selection */
	 ADMUX |= 
	 //0x00;			// ADC0 (PB5)
	 //0x01;			// ADC1 (PB2)
	 //0x02;		    // ADC2 (PB4)
	 0x03;			 // ADC3 (PB3)

	 /* ADC Enable */
	 ADCSRA |= (1 << ADEN);

	 /* ADC Auto Trigger Enable */
	 //ADCSRA |= (1<< ADATE);

	 /* ADC Interrupt Enable */
	 //ADCSRA |= (1 << ADIE);

	 /* ADC prescaler selections */
	 ADCSRA |=   
	 /*0x00;*/		// div 2
	 /*0x01;*/		// div 2
	 /*0x02;*/		// div 4
	 /*0x03;*/		// div 8
	 0x04; 			// div 16
	 /*0x05;*/		// div 32
	 /*0x06;*/		// div 64
	 /*0x07;*/		// div 128
	 
	 /* ADC auto trigger sources */
	 //ADCSRB |=	/*0x00;*/		// Free Running Mode
	 /*0x01;*/		// Analog Comparator
	 /*0x02;*/		// External Interrupt Request 0
	 /*0x03;*/		// Timer/Counter Compare Match A
	 /*0x04;*/ 		// Timer/Counter Overflow
	 /*0x05;*/		// Timer/Counter Compare Match B
	 /*0x06;*/		// Pin Change Interrupt Request

	 /* Digital input disable */
	 //DIDR0 |= (1 << ADC0D);	// Disable Digital Input Buffer on ADC0
	 DIDR0 |= (1 << ADC2D);		// Disable Digital Input Buffer on ADC2
	 //DIDR0 |= (1 << ADC3D);	// Disable Digital Input Buffer on ADC3
	 //DIDR0 |= (1 << ADC1D);	// Disable Digital Input Buffer on ADC1
 }

 void adc_disable()
 {
	ADCSRA &= ~(1 << ADEN);
 }

 void adc_enable()
 {
	ADCSRA |= (1 << ADEN);
 }

 /* Perform single conversion and return result */
 uint16_t adc_read_single()
 {
	 ADCSRA |= (1 << ADSC);
	 while(bit_is_set(ADCSRA, ADSC));
	 return ADCW;
 }

 void adc_select_channel(uint8_t channel)
 {
	ADMUX &= 0b11111100;
	ADMUX |= (0b00000011 & channel);
 }