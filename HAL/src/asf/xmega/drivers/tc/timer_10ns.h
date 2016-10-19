/*
 * timer_10ns.h
 *
 * Created: 13/2/2013 2:29:49 μμ
 *  Author: spilios
 */ 


#ifndef TIMER_10NS_H_
#define TIMER_10NS_H_

#include "compiler.h"

bool init_10us_step_counter(void);
void delay_1us_step(uint16_t step);
void delay_10us_step(uint16_t step);
void delay_1ms_step(uint16_t step);



#endif /* TIMER_10NS_H_ */