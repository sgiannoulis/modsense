/**
 * \addtogroup hal_GPS
 * @{
 */



/**
 * \file
 * Source file for the GPS HAL
 * \author
 * Spilios Giannoulis <sgiannoulis@isi.gr>
 *
 */ 

#include "hal_gps.h"
#include "led.h"

#define ASCII_DOLLAR 0x24
#define ASCII_COMMA 0x2c
#define ASCII_A		0x41


volatile static gps_data_t gps_data;
static struct HW_Object* gps_pulse;
static struct HW_Object* GPS_COM0;

typedef enum {
	IDLE,
	FOUND_PACKET,
	READING_TIME,
	READING_STATUS,
	READING_LATID,
	READING_LONG,
	READING_SPEED,
	READING_COURSE,
	READING_DATE,
	READING_VARIATION,
	READING_CHECKSUM
}gps_state;


bool hal_gps_init( struct HW_Object * hw_device){
		 
	if(hw_device==NULL)
		return false;
	
	//Enable GPS hw module
	if(gpio_pin_is_low(GPS_ENABLER))
		gpio_set_pin_high(GPS_ENABLER);
		
	/************************************************************************/
	/* Com0/GPS  usart init                                                 */
	/************************************************************************/
	usart_config SERIAL_OPTIONS = {
		.baudrate = 9600,
		.charlength = USART_CHSIZE_8BIT_gc,
		.paritytype = USART_PMODE_DISABLED_gc,
		.stopbits = false
	};
		 
	GPS_COM0 = hal_get_hw_obj(usart,0);  //0 is UsartC0 connected to GPS
	hal_init_hw_obj( GPS_COM0);
	hal_set_config_hw_obj( GPS_COM0, &SERIAL_OPTIONS);
	//hal_start_hw_obj(GPS_COM0);
		 
	gps_pulse =  hal_get_hw_obj(gpio,GPS_SECOND);
		 
	static struct gpio_hal_config gpio_conf;
	gpio_conf.flags=IOPORT_DIR_INPUT | IOPORT_RISING;
	gpio_conf.interrupt_level=PORT_INTLVL_MED_gc;
	gpio_conf.use_int_team0=true;
		 
		 
	hal_init_hw_obj(gps_pulse);
	hal_set_config_hw_obj(gps_pulse,&gpio_conf);
	//hal_start_hw_obj(gps_pulse);
		 
		 return;
		 
}


bool gps_set_config(struct  HW_Object* hw_device, struct hal_gps_config* config){

	return true;
	
}


bool gps_get_config(struct HW_Object* hw_device, struct hal_gps_config* config){

	return true;	
}


bool gps_start(struct HW_Object* hw_device){
	
	if(hw_device==NULL)
		return false;
	
	hw_device->started=true;
	
	//open the power to the gps module if still off
	if(gpio_pin_is_low(GPS_ENABLER))
		gpio_set_pin_high(GPS_ENABLER);
	
	hal_start_hw_obj(GPS_COM0);
	hal_start_hw_obj(gps_pulse);
	
	//Use led to signal operation
	LED_On(LED_GREEN);
}


bool gps_stop(struct HW_Object* hw_device){
	
	if(hw_device==NULL)
		return false;
	
	hw_device->started=false;
	
	hal_stop_hw_obj(gps_pulse);
	hal_stop_hw_obj(GPS_COM0);

	
	//close the power to the gps module if still on
	if(gpio_pin_is_high(GPS_ENABLER))
		gpio_set_pin_low(GPS_ENABLER);
		
	LED_Off(LED_GREEN);
}



gps_data_t* get_gps_data(){
	
	if(gps_data.gps_locked)
		return &gps_data;
	else
		return NULL;
}


uint8_t parse_gps_data(void){
	
	//bool res=true;
	uint8_t temp_char=0;
	static bool found_comma=false;
	static gps_state receive_state=IDLE;
	static uint8_t i=0;
	uint8_t result=0;
	
	//PRINTF("%u",GPS_COM0);
	
	while(hal_usart_receive_byte(GPS_COM0, &temp_char)){
		
		//Enable this to print the incoming NMEA sentences
		//PRINTF("%c",temp_char);
		
		switch (receive_state){
			
			case IDLE:
			if(temp_char==ASCII_DOLLAR){
				receive_state=FOUND_PACKET;
				//PRINTF("Found packet\r\n");
			}
			break;
			
			case FOUND_PACKET:
			if(temp_char==ASCII_COMMA){
				receive_state=READING_TIME;
				//PRINTF("Go to time\r\n");
			}
			break;
			
			case READING_TIME:
			if(temp_char==ASCII_COMMA){
				receive_state=READING_STATUS;
				i=0;
				//PRINTF("Go to status\r\n");
				//Calculate the bin values of time
				uint32_t temp=atol(gps_data.time);
				gps_data.bin_time[2]= temp%100;
				gps_data.bin_time[1]= (temp%10000)/100 ;
				gps_data.bin_time[0]= temp/10000;
				
				break;
			}
			gps_data.time[i++]=temp_char;
			break;
			
			case READING_STATUS:
			if(temp_char==ASCII_COMMA&&gps_data.gps_locked){
				receive_state=READING_LATID;
				//PRINTF("Go to Latitude\r\n");
			}
			else if (temp_char==ASCII_A)
			gps_data.gps_locked=true;
			else {
				gps_data.gps_locked=false; //the GPS hasn't locked so no valid data available
				receive_state=IDLE;
			}
			break;
			
			case READING_LATID:
			//Exit state if found second comma
			if(temp_char==ASCII_COMMA){
				if(found_comma){
					receive_state=READING_LONG;
					found_comma=false;
					i=0;
					//PRINTF("Go to longtitude\r\n");
					break;
				}
				else //found the first comma, set the flag
				found_comma=true;
			}
			//PRINTF("%c",temp_char);
			gps_data.latitude[i++]=temp_char;
			
			break;
			
			case READING_LONG:
			//Exit state if found second comma
			if(temp_char==ASCII_COMMA){
				if(found_comma){
					receive_state=READING_SPEED;
					found_comma=false;
					i=0;
					//PRINTF("Go to speed\r\n");
					break;
				}
				else
				found_comma=true;
			}
			gps_data.longitude[i++]=temp_char;
			break;
			
			case READING_SPEED:
			if(temp_char==ASCII_COMMA)
			receive_state=READING_COURSE;
			break;
			
			case READING_COURSE:
			if(temp_char==ASCII_COMMA)
			receive_state=READING_DATE;
			break;
			
			case READING_DATE:
			//Exit state if found comma
			if(temp_char==ASCII_COMMA){
				receive_state=READING_VARIATION;
				i=0;
				//Calculate the bin value of date
				uint32_t temp=atol(gps_data.date);
				gps_data.bin_date[2]= temp%100;
				gps_data.bin_date[1]= (temp%10000)/100 ;
				gps_data.bin_date[0]= temp/10000;
				
				break;
			}
			gps_data.date[i++]=temp_char;
			break;
			
			case READING_VARIATION:
			if(temp_char==ASCII_COMMA)
			receive_state=READING_CHECKSUM;
			break;
			
			case READING_CHECKSUM:
			i++;
			if(i==4){
				receive_state=IDLE;
				//	PRINTF("Time %s - ",&gps_data.time);
				//	PRINTF("Date %s - ",&gps_data.date);
				//	PRINTF("LAt %s - ",&gps_data.latitude);
				//	PRINTF("Long %s\r\n",&gps_data.longitude);
				
				i=0;
				result=1;
			}
			break;
			
			default:
				result=0;
			break;
			
		}//end switch
		
	}//end while
	
	return result;
}

ISR(PORTF_INT0_vect)
{
	uint8_t temp_char;
	
	PRINTF("\r\nGPS pulse\r\n");
	//read the latest gps data
	
/*	while(hal_usart_receive_byte(GPS_COM0, &temp_char)){
		
		//Enable this to print the incoming NMEA sentences
		PRINTF("%c",temp_char);
	}
	*/	
		
	if(parse_gps_data()){
	
		gps_data_t* tempor= get_gps_data();
	
				
		if(tempor !=NULL && tempor->gps_locked){
		
			//gps_data_t* tempor= get_gps_data();
		
	/*		PRINTF("\r\nTime %s ",tempor->time);
			PRINTF("Date %s ",tempor->date);
			PRINTF("LAt %s ",tempor->latitude);
			PRINTF("Long %s\r\n",tempor->longitude);
			PRINTF("H%u ",tempor->bin_time[0]);
			PRINTF("M%u ",tempor->bin_time[1]);
			PRINTF("S%u\r\n",tempor->bin_time[2]);
		
			PRINTF("D%u ",tempor->bin_date[0]);
			PRINTF("M%u ",tempor->bin_date[1]);
			PRINTF("Y%u\r\n",tempor->bin_date[2]);
		
			*/
			static struct HW_Object* system_time ;
			if (system_time==NULL){

				system_time = hal_get_hw_obj(rtc_ext,0);
				if(!system_time->started){
					hal_init_hw_obj(system_time);
					hal_start_hw_obj(system_time);
				}
			}
		
			calendar_date calend_date;
		
			calend_date.second=tempor->bin_time[2];
			calend_date.minute=tempor->bin_time[1];
			calend_date.hour=tempor->bin_time[0];
			calend_date.date=tempor->bin_date[0];
			calend_date.month=tempor->bin_date[1];
			calend_date.year=tempor->bin_date[2] +2000;
		
			if(hal_system_time_write_time(system_time,calendar_date_to_timestamp(&calend_date))){
				
				//reset the msec time to adjust full system time to start of second without running any callbacks
				//reset_msec_count(false);
				
				//got time and fix, turn of the gps
				hal_stop_hw_obj(&global_hw_registry.GPS);	
		
				PRINTF("M%u ",tempor->bin_time[1]);
				PRINTF("S%u\r\n",tempor->bin_time[2]);
			}else
				PRINTF("Error writing to ext RTC");
		
		}
	}
}


/** @} */