/*
 * wifi.c
 *
 * Created: 19/10/2012 11:30:53 πμ
 *  Author: spilios
 */ 
#include "wifi.h"
#include "gpio.h"
#include "../Boards/Modsense/user_board.h"

bool enable_wifi(){	
	
	if(gpio_pin_is_low(WIFI_ENABLER))
		gpio_set_pin_high(WIFI_ENABLER);
	
	return true;
} 

bool disable_wifi(){
	
	if(gpio_pin_is_high(WIFI_ENABLER))
		gpio_set_pin_low(WIFI_ENABLER);
	
	return true;
}

