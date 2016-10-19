/*
 * ads1254.h
 *
 * Created: 13/2/2013 3:59:47 μμ
 *  Author: spilios
 */ 


#ifndef ADS1254_H_
#define ADS1254_H_

#include "timer_10ns.h"


typedef struct ext_adc_result{
	int8_t data[9];
	} ext_adc_result;

	
#define ADC_FULLSCALE 0x00FFFFFF
#define ADC_FULLSCALE_BITS 24

void init_ads1254(void);	
void read_ads1254_result(ext_adc_result* result);

void read_multiple_ads1254_result(ext_adc_result* result,uint16_t count,uint16_t period);


bool read_done_ads1254(void);

uint16_t return_count(void);
uint16_t reset_count(void);






#endif /* ADS1254_H_ */