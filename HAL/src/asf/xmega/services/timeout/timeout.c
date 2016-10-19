/**
 * \file  timeout.c
 *
 * \brief Timeout service for XMEGA
 *
 * Copyright (C) 2011-2012 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */
#include <asf.h>
#include <conf_timeout.h>
#include "etimer.h"

/* Check if RTC32 is defined, otherwise use RTC as default */
#if defined(CLOCK_SOURCE_RTC32)
  #include <rtc32.h>
#else
  #include <rtc.h>
#endif
#include "definitions.h"

#define DEBUG_TIMEOUT

#define TIMER_INTERRUPT_PRIORITY_BM PMIC_LVL_LOW  //if changed must change accordingly the below define
#define TIMER_INTERRUPT_PRIORITY TC_INT_LVL_LO

#define MAX_TICKS (~((clock_time_t)0) / 2)


/** \brief Timeout timekeeping data */
struct timeout_struct {
	
	//When the timer was started
	uint16_t start_time;
		
	/**
	 * Current count-down value. Counts down for every tick.
	 * Will be considered as expired when it reaches 0, and
	 * may then be reloaded with period.
	 */
	uint16_t count;

	/**
	 * Period between expires. Used to reload count.
	 * If 0, the count won't be reloaded.
	 */
	uint16_t period;
	
	/**
	*Array of callback functions  for each timer
	*
	*/
	callback_fun callback;
};

static bool timers_initialized = false;
static bool run_callbacks = true;

/** Array of configurable timeout timekeeping data */
static struct timeout_struct timeout_array[TIMERS_COUNT];

/** Bitmask of active timeouts */
static uint8_t timeout_active;

/** Bitmask of expired timeouts */
static uint8_t timeout_expired;

static uint32_t	system_msec_time=0;
static uint32_t ticks = 0;
/**
 * \brief Callback function for RTC compare interrupt handler
 *
 * The function executes when the RTC compare interrupt occurs and loop
 * through all timeout channels. The timeout_array[channel_index] which
 * contains the remaining ticks before timeout is decremented and the timeout
 * active/expired masks are updated.
 */
static void tick_handler()
//static void tick_handler()
{
	irqflags_t flags;
	
	flags = cpu_irq_save();
	if(system_msec_time>=1000){
		return;
	}
	
	system_msec_time=(system_msec_time+1000/TIMEOUT_TICK_HZ);
	ticks++;
	
	cpu_irq_restore(flags);
	
	//PRINTF("\n\r%u",msec_time());	
/*	

	

	if(msec_offset<0){
		msec_offset++;
		tick_handler(1);
	}
	*/
	uint8_t i;
	
	//tc_restart(&TCD0);
	
	/* Loop through all timeout channels */
	for (i = 0; i < TIMERS_COUNT; i++) {
		/* Skip processing on current channel if not active */
		if (!(timeout_active & (1 << i))) {
			continue;
		}

		/* Decrement current channel with one tick */
		timeout_array[i].count--;

		/* Skip further processing on current channel if not expired */
		if (timeout_array[i].count) {
			continue;
		} else {
			/* Update expired bit mask with current channel if callback is not set*/
			if(timeout_array[i].callback){
				
				if(run_callbacks)
					timeout_array[i].callback();
			}
			else
				timeout_expired |= (1 << i);

			/* If Periodic timer, reset timeout counter to period
			 * time */
		//	flags = cpu_irq_save();
			if (timeout_array[i].period) {
				timeout_array[i].count
					= timeout_array[i].period;
			}
			/* If not periodic timeout, set current channel to
			 * in-active */
			else {
				timeout_active &= ~(1 << i);
			}
			//cpu_irq_restore(flags);
		}
	}

	
	 if( etimer_pending() && (etimer_next_expiration_time() - ticks - 1) > MAX_TICKS)
		 etimer_request_poll();
		 

	
//	PRINTF("%u\r\n",system_msec_time);
	/* Reset RTC before next tick */
	//rtc_set_time(0);
	//rtc_set_alarm(TIMEOUT_COMP);
	
}

/**
 * \brief Initialize timeout
 *
 * Initializes timeout counter for desired tick rate and starts it. The device
 * interrupt controller should be initialized prior to calling this function,
 * and global interrupts must be enabled.
 *
 * \note If the service is configured to use the asynchronous RTC32 module,
 *       there are restrictions on the timeout period that can be used - see
 *       to \ref rtc32_min_alarm_time for details.
 */
bool timeout_init(void)
{
	if(!timers_initialized){
	
	
/*	rtc_init();
	rtc_set_callback(tick_handler);
	rtc_set_time(0);
	rtc_set_alarm(TIMEOUT_COMP);
*/
	
	tc_enable(&TCD0);
//	tc_enable(&TCD1);
	 
	
	/*
	* Configure interrupts callback functions for TIMER_EXAMPLE
	* overflow interrupt, CCA interrupt and CCB interrupt
	*/
	tc_set_overflow_interrupt_callback(&TCD0,
			tick_handler);
	//tc_set_cca_interrupt_callback(&TCD0,	example_cca_interrupt_callback);
	//tc_set_ccb_interrupt_callback(&TCD0,	example_ccb_interrupt_callback);

	/*
	* Configure TC in normal mode, configure period, CCA and CCB
	* Enable both CCA and CCB channels
	*/

	tc_set_wgm(&TCD0, TC_WG_NORMAL);
	tc_write_period(&TCD0, TIMEOUT_COMP);
	
//	tc_set_wgm(&TCD1, TC_WG_NORMAL);
//	tc_write_period(&TCD1, 0xffff);
	//tc_write_cc(&TCD0, TC_CCA, TIMER_EXAMPLE_PERIOD / 2);
	//tc_write_cc(&TCD0, TC_CCB, TIMER_EXAMPLE_PERIOD / 4);
	//tc_enable_cc_channels(&TCD0,(enum tc_cc_channel_mask_enable_t)(TC_CCAEN | TC_CCBEN));

	/*
	* Enable TC interrupts (overflow, CCA and CCB)
	*/
	tc_set_overflow_interrupt_level(&TCD0, TIMER_INTERRUPT_PRIORITY);
	//tc_set_cca_interrupt_level(&TCD0, TC_INT_LVL_LO);
	//tc_set_ccb_interrupt_level(&TCD0, TC_INT_LVL_LO);

	/*
	* Run TIMER_EXAMPLE at TIMER_EXAMPLE_PERIOD(31250Hz) resolution
	*/
	//tc_set_resolution(&TCD0, TIMER_EXAMPLE_PERIOD);
	tc_write_clock_source(&TCD0, TC_CLKSEL_DIV2_gc);
//	tc_write_clock_source(&TCD1, TC_CLKSEL_DIV1024_gc);
	
	timers_initialized = true;	
	}
	
	if(!pmic_level_is_enabled(TIMER_INTERRUPT_PRIORITY_BM))
		pmic_enable_level(TIMER_INTERRUPT_PRIORITY_BM);
	
	if(!cpu_irq_is_enabled())
		cpu_irq_enable();
	
	run_callbacks = true;
	
	return timers_initialized;
}

/**
 * \brief Start periodic timeout with a specific start timeout
 *
 * \param id            \ref timeout_id_t
 * \param period        Time period in number of ticks
 * \param offset        Time to first timeout in number of ticks
 */
void timeout_start_offset(timeout_id_t id, uint16_t period, uint16_t offset)
{
	/* Check that ID within the TIMERS_COUNT range */
	if (id < TIMERS_COUNT) {
		/* Disable interrupts before tweaking the bitmasks */
		irqflags_t flags;
		flags = cpu_irq_save();

		/* Update timeout struct with offset and period */
		timeout_array[id].count = (uint16_t)(((uint32_t)offset*TIMEOUT_TICK_HZ)/1000);
		timeout_array[id].period = (uint16_t)(((uint32_t)period*TIMEOUT_TICK_HZ)/1000);
		//timeout_array[id].period = period;


		/* Set current timeout channel bitmasks to active and not
		 * expired */
		timeout_active |= 1 << id;
		timeout_expired &= ~(1 << id);

		/* Restore interrupts */
		cpu_irq_restore(flags);
	}
}

/**
 * \brief Start singleshot timeout
 * * \param id       \ref timeout_id_t
 * \param timeout  Timeout in number of ticks
 */
void timeout_start_singleshot(timeout_id_t id, uint16_t timeout)
{
	timeout_start_offset(id, 0, timeout);
}

/**
 * \brief Start periodic timeout
 *
 * \param id      \ref timeout_id_t
 * \param period  Time period in number of ticks
 */
void timeout_start_periodic(timeout_id_t id, uint16_t period)
{
	timeout_start_offset(id, period, period);
}

/**
 * \brief Test and clear expired flag for running timeout
 *
 * \param id      \ref timeout_id_t
 * \retval true   Timer have expired; clearing expired flag
 * \retval false  Timer still running
 */
bool timeout_test_and_clear_expired(timeout_id_t id)
{
	/* Check that ID within the TIMERS_COUNT range */
	if (id < TIMERS_COUNT) {
		irqflags_t flags;

		/* Check if timeout has expired */
		if (timeout_expired & (1 << id)) {
			flags = cpu_irq_save();
			timeout_expired &= ~(1 << id);
			cpu_irq_restore(flags);
			return true;
		}
	}

	return false;
}

/**
 * \brief Stop running timeout
 *
 * \param id \ref timeout_id_t
 */
void timeout_stop(timeout_id_t id)
{
	irqflags_t flags;
	flags = cpu_irq_save();
	timeout_active &= ~(1 << id);
	cpu_irq_restore(flags);
}

void timeout_set_callback(timeout_id_t id, callback_fun callback){
	
	timeout_array[id].callback = callback;	
}

uint32_t msec_time(void){

	return system_msec_time%1000;   //0-999
}

uint32_t system_ticks(void){

	//return 10*(uint32_t)tc_read_count(&TCD1)/109;
//	PRINTF("ticks %lu\r\n",ticks);
	return ticks;
}

/*
uint32_t clock_seconds(void){
	return system_msec_time/1000;
}
*/

/*bool set_clock_time(uint32_t time){
	
	system_msec_time=time*1000;
	
	return true;
}
*/


void enable_callbacks(){
	run_callbacks=true;
}

void disable_callbacks(){
	run_callbacks=false;
}


Bool reset_msec_count(bool run_callback){

/*	static uint32_t last_sec=0;
	PRINTF("%u msec to ",msec_time()-last_sec);
	last_sec= msec_time();
*/
	
	/*
	msec_offset = system_msec_time - 1000;	
	if(abs(msec_offset)>500)
		msec_offset=0;

	while(msec_offset<0){
		msec_offset++;
		tick_handler(1);
	}
	*/
	
	tc_write_count(&TCD0,0);
	tc_clear_overflow(&TCD0);


	if(!run_callback)
		disable_callbacks();
		
	while(system_msec_time<990){
		tick_handler();
	}
	
	if(!run_callback)
	 enable_callbacks();
	
	//run once more with enabled callbacks for the 0 tick
	tick_handler();
	
#ifdef DEBUG_TIMEOUT
	if(system_msec_time!=1000)
		PRINTF("ERROR system msec not 0, msec %u\r\n",system_msec_time);
#endif
	
	
	system_msec_time=0;
	//tick_handler();
	
	return true;
}

