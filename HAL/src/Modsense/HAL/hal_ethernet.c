/*
 * hal_ethernet.c
 *
 * Created: 16/6/2013 10:07:15 μμ
 *  Author: spilios
 */ 
#include "hal_ethernet.h"

static  struct HW_Object* HW_COM5 ;

bool ethernet_init( struct HW_Object* hw_device){
	if(hw_device==NULL)
		return false;
	
	//IF the used usart is changed, all it is needed is simply to change this get function attributes
	 HW_COM5 = hal_get_hw_obj(usart,5);
	
	if(!HW_COM5)
		return false;
		
	/************************************************************************/
	/* Com5/Ethernet usart init                                             */
	/************************************************************************/
	usart_config SERIAL_OPTIONS = {
		.baudrate = 460800,
		.charlength = USART_CHSIZE_8BIT_gc,
		.paritytype = USART_PMODE_DISABLED_gc,
		.stopbits = false
	};
	
	if(!hal_init_hw_obj( HW_COM5))
		return false;
	
	//hal_start_hw_obj(GPS_COM0);
		
	return hal_set_config_hw_obj( HW_COM5, &SERIAL_OPTIONS);
}

bool ethernet_set_config( struct HW_Object* hw_device, struct hal_ethernet_config* config){

	//return hal_set_config_hw_obj(HW_COM5,config);
	return true;
}
	
bool ethernet_get_config( struct HW_Object* hw_device, struct hal_ethernet_config* config){

	return true;
}


bool ethernet_start( struct HW_Object* hw_device){
	if(hw_device==NULL)
		return false;
	if(!hw_device->started){
	hw_device->started=true;	
	return hal_start_hw_obj(HW_COM5);
	}
	return true;
}

bool ethernet_stop( struct HW_Object* hw_device){
	if(hw_device==NULL)
		return false;
	hw_device->started=false;	
	return hal_stop_hw_obj(HW_COM5);
}

inline bool ethernet_send_byte( struct HW_Object* hw_device, uint8_t byte_data){

	return hal_usart_send_byte(HW_COM5,byte_data);
}

inline bool  ethernet_receive_byte( struct HW_Object* hw_device,uint8_t* data){

	 
	return hal_usart_receive_byte(HW_COM5,data);
}


bool ethernet_send_string( struct HW_Object* hw_device, uint8_t* string, uint16_t size){

	return hal_usart_send_string(HW_COM5,string,size);
}

uint8_t ethernet_receive_string( struct HW_Object* hw_device, uint8_t* buffer, uint16_t size){

	return hal_usart_receive_string(HW_COM5,buffer,size);
}

