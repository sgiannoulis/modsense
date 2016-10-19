

#include "hal_system_time.h"
#include "definitions.h"
#include "calendar.h"

//! Slave TWI address of the external RTC
#define TWI_SLAVE_ADDR       0x68
//! Register base address of the external RTC
#define RTC_REG_ADDR         0x00

#define RTC_TWI_SPEED            400000

static rtc_time_t rtc_time = {0} ;
static struct hal_twi_config config ;
static struct HW_Object* twi_bus;

calendar_date temp_calendar;
//static uint32_t system_seconds=0;
uint32_t system_global_time=0;


void enable_seconds_interrupt();

// Package to send
static twi_package_t packet = {
	.chip        = TWI_SLAVE_ADDR ,				//!< TWI slave bus address
	.addr[0]     = RTC_REG_ADDR,                //!< TWI slave memory register address
	.addr_length = sizeof(uint8_t),				//!< Size of each register of RTC
	.buffer      = (void *) &rtc_time,
	.length      = sizeof(rtc_time_t),
	.no_wait     = false
};

bool calendar_to_rtc32_form(calendar_date* input,rtc_time_t*  output){
	
	/*typedef struct calendar_date {
		uint8_t second;        //!< 0-59
		uint8_t minute;        //!< 0-59
		uint8_t hour;          //!< 0-23
		uint8_t date;          //!< 1-31
		uint8_t month;         //!< 1 January - 12 December
		uint16_t year;         //!< 1970-2105
		uint8_t dayofweek;     //!< 0 Sunday  - 6 Saturday
	}calendar_date;
*/	
	
	output->seconds = ((input->second/10)<<4) | (input->second%10);
	output->minutes = ((input->minute/10)<<4) | (input->minute%10);
	
	if(input->hour>20)
		output->hours =  ((uint8_t)(input->hour/20)<<5) | (input->hour%10);
 	else
 		output->hours =  ((uint8_t)(input->hour/10)<<4) | (input->hour%10);
	
	output->day = input->dayofweek +1;
	
	output->date =((input->date/10)<<4)| (input->date%10);

	output->month_century = ((uint8_t)(input->year - 1970)/100)<<7 | (input->month/10)<<4 | (input->month%10);
	
	if((input->year - 1970)/100)
		output->year = ((uint8_t)(input->year - (1970+100))/10)<<4 |  (input->year%10);
	else
		output->year = ((uint8_t)(input->year - 1970)/10)<<4 |  (input->year%10);

	return true;
}


bool rtc32_to_calendar_form(rtc_time_t* input,calendar_date* output){
	
	output->second = (input->seconds&0x0f) + 10 * (input->seconds>>4);
	output->minute = (input->minutes&0x0f) + 10 * (input->minutes>>4);
	
	
	 
	 output->date = (input->date&0x0f) + 10 * ((input->date&0x30)>>4);
	 output->month = (input->month_century&0x0f) + 10* ((input->month_century&0x10)>>4);
	 output->year = 1970 + (input->year&0x0f) + 10*((input->year&0xf0)>>4) + 100*((input->month_century&0x80)>>7);
	 
	 output->dayofweek = input->day;
	 	
	if(!(output->hour&0x40)) //then its 24h mode
		 output->hour = (input->hours&0x0f) + ((input->hours&0x10)>>4)*10 + ((input->hours&0x20)>>4)*20;
	 else //its 12h mode, dont use it
		 return false;
	 
	return true;
}


bool hal_system_time_init( struct HW_Object* hw_device){
	
	// We must initialize the TWI_C and setup the communication with the slave RTC
	// then set the time clock of our system according to RTC time
//	PRINTF("\r\nStart of Testing RTC32 ext via TWI \r\n");
		if (hw_device->started)
			return true;
		
		twi_bus = hal_get_hw_obj(twi,0);
		hw_device->hw_pointer = twi_bus;
		
		config.chip_address=50;
		config.master=true;
		config.speed=RTC_TWI_SPEED;
		
		hal_set_config_hw_obj(twi_bus,&config);
		
		if(hal_init_hw_obj(twi_bus))
//		PRINTF("\nSuccess init on i2c \r\n");
			hal_start_hw_obj(twi_bus);
		
		
		hal_twi_read_data(twi_bus, &packet);
		rtc32_to_calendar_form(&rtc_time, &temp_calendar);
		system_global_time =  calendar_date_to_timestamp(&temp_calendar);
		
		//TODO here we must call gps to get gps time 
		{
			
			//system_global_time=0;
			if(system_global_time<1382124960){  //checking if we have no time or very old time 
			//inizitalize RTC time  = 18/10/13 16:33:00
			rtc_time.minutes=			0b00110011;
			rtc_time.hours=				0b00010110;
			rtc_time.date =				0b00011000;
			rtc_time.month_century =	0b00010000;
			rtc_time.year=				0b01000011;

			hal_twi_write_data(twi_bus, &packet);
			}
		}
		
		//Get system time from ext RTC
		hal_twi_read_data(twi_bus, &packet);
		
		rtc32_to_calendar_form(&rtc_time, &temp_calendar);
		
		system_global_time =  calendar_date_to_timestamp(&temp_calendar);
		
//		PRINTF("System time in RTC form is %u/%u/%u , %u:%u:%u \r\n",rtc_time.date, rtc_time.month_century,rtc_time.year, rtc_time.hours, rtc_time.minutes, rtc_time.seconds );
//		PRINTF("System time in Calendar form is %u/%u/%u , %u:%u:%u \r\n",temp_calendar.date, temp_calendar.month, temp_calendar.year, temp_calendar.hour, temp_calendar.minute, temp_calendar.second );
//		PRINTF("System time in Linux timestamp form %lu \r\n", system_global_time);
		
	return true;		
}


bool hal_system_time_start(struct HW_Object* hw_device){
	
	if (hw_device->started!=true)
	{
	hw_device->started=true;
	enable_seconds_interrupt();
	}
	
	return true;
}


	
bool hal_system_time_stop(struct HW_Object* hw_device){
	hw_device->started=false;
	return hal_stop_hw_obj(twi_bus);
}	

uint32_t hal_system_time_read_sec_time(struct HW_Object*  hw_device){
/*	if(hw_device->started){
		hal_twi_read_data(twi_bus, &packet);
		rtc32_to_calendar_form(&rtc_time, &temp_calendar);
		system_global_time = calendar_date_to_timestamp(&temp_calendar);
	return system_global_time;
	}
	return 13;*/
	return system_global_time;
}

uint32_t hal_system_time_read_msec_time(struct HW_Object*  hw_device){
	
	//Making space in the system global time for the 1000 msecs in uint32. 2^32 /1000 = 4294967,296
	return (system_global_time%4294967)*1000+msec_time();
}


bool hal_system_time_write_time(struct HW_Object*  hw_device, uint32_t timestamp){
	
	system_global_time=timestamp;
	
	//Write also to external RTC if it is started
	if(hw_device->started){ 
		//Create the calendar struct type
		timestamp_to_calendar(timestamp,&temp_calendar);
		//Transform to RTC32 type 
		calendar_to_rtc32_form( &temp_calendar, &rtc_time);
		//Write to external RTC32
		
		if( STATUS_OK == hal_twi_write_data(twi_bus, &packet))
			reset_msec_count(false);
			return true;
	}
	
	return false;
	
}

void enable_seconds_interrupt(){
	
	static uint8_t control_bits = 0x40;
	
	twi_package_t control_packet = {
		.chip        = TWI_SLAVE_ADDR ,				//!< TWI slave bus address
		.addr[0]     = 0x0E,							//!< TWI slave control register address
		.addr_length = sizeof(uint8_t),				//!< Size of each register of RTC
		.buffer      = (void *) &control_bits,
		.length      = sizeof(control_bits),
		.no_wait     = false
	};
	
	hal_twi_write_data(twi_bus, &control_packet);
	
	
	struct HW_Object* sec_gpio =  hal_get_hw_obj(gpio,RTC_SECONDS);
	
	static struct gpio_hal_config gpio_conf;
	gpio_conf.flags=IOPORT_DIR_INPUT | IOPORT_FALLING;
	gpio_conf.interrupt_level=PORT_INTLVL_HI_gc;
	gpio_conf.use_int_team0=true;
	
	
	hal_init_hw_obj(sec_gpio);
	hal_set_config_hw_obj(sec_gpio,&gpio_conf);
	hal_start_hw_obj(sec_gpio);
		
/*
	PORT_t * port_name = arch_ioport_pin_to_base(RTC_SECONDS);
	//port_name->PIN2CTRL |=IOPORT_SENSE_FALLING;
	port_name->INT0MASK |= arch_ioport_pin_to_mask(RTC_SECONDS);
	//PORTB.INT0MASK = 0x04;
	
	port_name->INTCTRL=PORT_INT0LVL_HI_gc;
	*/
}

/*
Task: Set up PORTC interrupt 0 as a medium level interrupt, triggered by the rising edge of PC0. Use the interrupt service routine to toggle the output on PORTD.
1. Configure input/sense on pin0 to rising edge.
2. Write 0x01 to PORTC.INT0MASK to select PC0 as source for interrupt 0.
3. Set IN0LVL part of PORTC.INTCTRL to PORT_INT0LVL_MED_gc to enable interrupt 0 at medium level.
4. Enable medium level interrupts in the PMIC.
5. Enable the global interrupt flag.
*/

ISR(PORTB_INT0_vect)
{
	reset_msec_count(true);
	system_global_time++;
	//PRINTF("%u",system_global_time);
	
}



