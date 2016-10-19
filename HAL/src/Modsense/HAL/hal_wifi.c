/*
 * hal_wifi.c
 *
 * Created: 16/6/2013 10:07:15 μμ
 *  Author: spilios
 */ 
#include "hal_wifi.h"

static  struct HW_Object* USART_WIFI_P ;
static uint8_t cmd_buf[20];


bool hal_wifi_init( struct HW_Object* hw_device){
	
	uint8_t rec_byte;
	
	if(hw_device==NULL)
		return false;
		
	//Enable the wifi power, do not reset the wifi module
	gpio_set_pin_high(WIFI_ENABLER); //enable WIFI/ethernet
	gpio_set_pin_high(WIFI_RESET); //do not reset WIFI/ethernet
		
	//IF the used usart is changed, all it is needed is simply to change this get function attributes
	 USART_WIFI_P = hal_get_hw_obj(usart,4);
	 
	 if(USART_WIFI_P==NULL)
		return false;
		
	 /************************************************************************/
	 /* Com4/Wifi usart init                                             */
	 /************************************************************************/
	 usart_config SERIAL_OPTIONS = {
		 .baudrate = 460800,
		 .charlength = USART_CHSIZE_8BIT_gc,
		 .paritytype = USART_PMODE_DISABLED_gc,
		 .stopbits = false
	 };
	 
	if(!hal_init_hw_obj( USART_WIFI_P))
		return false;
		
	hal_set_config_hw_obj( USART_WIFI_P, &SERIAL_OPTIONS);
	hal_start_hw_obj(USART_WIFI_P);
	
	//Initialize the WIFLY module with the appropriate ASCII COMMANDS
	delay_1ms_step(5000);
	/************************************************************************/
	/*The following must be send always depending on the desired AP to connect to
	$$$
	set wlan phrase netanet1
	set ip dhcp 0
	set ip address 192.168.1.104
	set ip netmask 255.255.255.0
	set ip gateway 192.168.1.1
	join ISI_B10
	
	and perhaps: set comm time 5 to speed up responses,  badly working, recommend set to 20
				 set uart baud 460800 //working
				 								                       */
	/************************************************************************/
	
	//hal_usart_send_string(USART_WIFI_P,(char*)"$$$",4);
	hal_usart_send_string(USART_WIFI_P,cmd_buf,sprintf((char*)cmd_buf,"$$$"));
	delay_1ms_step(1000);
	//Waiting for CMD response
//	while(rec_byte!="D")
//		hal_usart_receive_byte(USART_WIFI_P,&rec_byte);
	
	hal_usart_send_string(USART_WIFI_P,cmd_buf,sprintf((char*)cmd_buf,"\r\nscan\r\n"));
	//hal_usart_send_string(USART_WIFI_P,(char*)"scan\r\n",6);
	delay_1ms_step(4000);
	
	hal_usart_send_string(USART_WIFI_P,cmd_buf,sprintf((char*)cmd_buf,"scan\r\n"));
	//hal_usart_send_string(USART_WIFI_P,(char*)"scan\r\n",6);
	delay_1ms_step(4000);
	
		
//	hal_usart_send_string(USART_WIFI_P,cmd_buf,sprintf((char*)cmd_buf,"set comm time 20\r\n"));
//	delay_1ms_step(1000);
		
	hal_usart_send_string(USART_WIFI_P,cmd_buf,sprintf((char*)cmd_buf,"set wlan ssid ISI_B10\r\n"));
	delay_1ms_step(1000);
	
	hal_usart_send_string(USART_WIFI_P,cmd_buf,sprintf((char*)cmd_buf,"set wlan phrase netanet1\r\n"));
	delay_1ms_step(1000);
	
	hal_usart_send_string(USART_WIFI_P,cmd_buf,sprintf((char*)cmd_buf,"set ip dhcp 0\r\n"));
	delay_1ms_step(1000);	
	
	hal_usart_send_string(USART_WIFI_P,cmd_buf,sprintf((char*)cmd_buf,"set wlan join 1\r\n"));
	delay_1ms_step(1000);
	
	hal_usart_send_string(USART_WIFI_P,cmd_buf,sprintf((char*)cmd_buf,"save\r\n"));
	delay_1ms_step(1000);
	
//	hal_usart_send_string(USART_WIFI_P,cmd_buf,sprintf((char*)cmd_buf,"join ISI_B10\r\n"));
//	delay_1ms_step(1000);

	//hal_usart_send_string(USART_WIFI_P,(char*)"join ISI_B10\r\n",15);
	//Waiting for CMD response
//	while(rec_byte!="K")
//		hal_usart_receive_byte(USART_WIFI_P,&rec_byte);
	
	/*hal_usart_send_byte(USART_WIFI_P,68);
	hal_usart_send_byte(USART_WIFI_P,68);
	
	hal_wifi_start(hw_device);
	hal_wifi_send_byte(hw_device,69);*/
	
	hal_usart_send_string(USART_WIFI_P,cmd_buf,sprintf((char*)cmd_buf,"exit\r\n"));
	//delay_1ms_step(1000);
	
	return true;
}

bool hal_wifi_set_config( struct HW_Object* hw_device, struct hal_wifi_config* config){
	
	return true;
	//return hal_set_config_hw_obj(USART_WIFI_P,config);
}
	
bool hal_wifi_get_config( struct HW_Object* hw_device, struct hal_wifi_config* config){

	return true;
}


bool hal_wifi_start( struct HW_Object* hw_device){
	if(hw_device==NULL)
		return false;
	if(!hw_device->started)
		hw_device->started=true;	
	
	return hal_usart_start(USART_WIFI_P);
}

bool hal_wifi_stop( struct HW_Object* hw_device){
	if(hw_device==NULL)
		return false;
	hw_device->started=false;	
	return hal_usart_stop(USART_WIFI_P);
}

inline bool hal_wifi_send_byte( struct HW_Object* hw_device, uint8_t byte_data){

	if(hw_device->started)
		return hal_usart_send_byte(USART_WIFI_P,byte_data);
	else 
		return false;
}

inline bool  hal_wifi_receive_byte( struct HW_Object* hw_device,uint8_t* data){

	if(hw_device->started)
		return hal_usart_receive_byte(USART_WIFI_P,data);
	else
		return false;
	
}


bool hal_wifi_send_string( struct HW_Object* hw_device, uint8_t* string, uint16_t size){

	return hal_usart_send_string(USART_WIFI_P,string,size);
}

uint8_t hal_wifi_receive_string( struct HW_Object* hw_device, uint8_t* buffer, uint16_t size){

	return hal_usart_receive_string(USART_WIFI_P,buffer,size);
}

