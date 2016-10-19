/*
 * gps_parser.c
 *
 * Created: 26/11/2013 4:15:00 μμ
 *  Author: spilios
 */ 

//variables to hold the data from gps
#include "definitions.h"
#include "hal_interface.h"
#include "gps_parser.h"


#define ASCII_DOLLAR 0x24
#define ASCII_COMMA 0x2c


	
static gps_data_t gps_data;
	

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
	

static struct HW_Object* GPS_COM0;

//usart_receive_byte(GPS_COM0, &temp_char);

bool init_gps(void){

	 //Enable GPS
	 gpio_set_pin_high(GPS_ENABLER); 
	 
	/************************************************************************/
	/* Com0/GPS  usart init                                                  */
	/************************************************************************/
	 usart_config USART_SERIAL_OPTIONS = {
		 .baudrate = 9600,
		 .charlength = USART_CHSIZE_8BIT_gc,
		 .paritytype = USART_PMODE_DISABLED_gc,
		 .stopbits = false
	 };
	 	 
	 GPS_COM0 = hal_get_hw_object(usart,0);  //0 is UsartC0 connected to GPS
	 hal_init_hw_object( GPS_COM0);
	 hal_set_config( GPS_COM0, &USART_SERIAL_OPTIONS);
	 hal_hw_start(GPS_COM0);
	 	 
	return; 
}


uint8_t parse_gps_data(void){
	
	//bool res=true;
	uint8_t temp_char=0;
	static bool found_comma=false;
	static gps_state receive_state=IDLE;
	static uint8_t i=0;
	
	//PRINTF("%u",GPS_COM0);
	
	while(usart_receive_byte(GPS_COM0, &temp_char)){
		
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
					break;
				}
				gps_data.time[i++]=temp_char;	
			break;
			
			case READING_STATUS:
				if(temp_char==ASCII_COMMA){
					receive_state=READING_LATID;
					//PRINTF("Go to Latitude\r\n");
				}
				else if (temp_char=="A")
					gps_data.gps_locked=true;
				else {
					gps_data.gps_locked=false; //the GPS hasn't locked so no valid data available
					//return 0;
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
					return 1;
				}
			break;
			
			default:
			return 0;
		}//end switch
		
	}//end while
	
	return 0;		
}


gps_data_t* get_gps_data(void){
	return &gps_data;
}
