/*
 * timer_10ns.c
 *
 * Created: 13/2/2013 2:03:37 μμ
 *  Author: Spilios Giannoulis
 */ 



#include "tc.h"
#include "timer_10ns.h"


bool init_10us_step_counter(void){
	
	tc_enable(&TCC0);
	tc_set_wgm(&TCC0, TC_WG_NORMAL);
	
	return true;
	}
	

inline void delay_1us_step(uint16_t step){
	
	tc_write_period(&TCC0, (11*step));
	tc_write_clock_source(&TCC0, TC_CLKSEL_DIV1_gc);
	//tc_set_resolution(&TCC0, 10800);
	tc_restart(&TCC0);
	
	while(!tc_is_overflow(&TCC0));
	
	tc_clear_overflow(&TCC0);
	
	return;
}

inline void delay_10us_step(uint16_t step){
	
		//irqflags_t flags = cpu_irq_save();
		tc_write_period(&TCC0, (111*step));
		tc_write_clock_source(&TCC0, TC_CLKSEL_DIV1_gc);
		//tc_set_resolution(&TCC0, 10800);
		tc_restart(&TCC0);
		//cpu_irq_restore(flags);
		
		while(!tc_is_overflow(&TCC0));
		
		tc_clear_overflow(&TCC0);
	
	return;
	}
	
void delay_1ms_step(uint16_t step){
	
	for(uint16_t i=0;i<step;i++)
		delay_10us_step(100);
	return;
}


