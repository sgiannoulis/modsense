/*
 * hal_interface.c
 *
 * Created: 13/6/2013 2:26:24 μμ
 *  Author: spilios
 */ 

#include "hal_interface.h"

struct hw_registry_struct global_hw_registry;


bool hal_system_init(){
	
	global_hw_registry.COM0.hw_pointer=&USARTC0;
    global_hw_registry.COM0.number=0;
    global_hw_registry.COM0.type=usart;
    global_hw_registry.COM0.used=0;
	global_hw_registry.COM0.started=0;
   
    global_hw_registry.COM1.hw_pointer=&USARTC1;
    global_hw_registry.COM1.number=1;
    global_hw_registry.COM1.type=usart;
    global_hw_registry.COM1.used=0;
	global_hw_registry.COM1.started=0;
			
    global_hw_registry.COM2.hw_pointer=&USARTD0;
    global_hw_registry.COM2.number=2;
    global_hw_registry.COM2.type=usart;
    global_hw_registry.COM2.used=0;
	global_hw_registry.COM2.started=0;
	
    global_hw_registry.COM3.hw_pointer=&USARTD1;
    global_hw_registry.COM3.number=3;
    global_hw_registry.COM3.type=usart;
    global_hw_registry.COM3.used=0;
	global_hw_registry.COM3.started=0;
	
    global_hw_registry.COM4.hw_pointer=&USARTE0;
    global_hw_registry.COM4.number=4;
    global_hw_registry.COM4.type=usart;
    global_hw_registry.COM4.used=0;
	global_hw_registry.COM4.started=0;
	
	global_hw_registry.COM5.hw_pointer=&USARTF0;
	global_hw_registry.COM5.number=5;
	global_hw_registry.COM5.type=usart;
	global_hw_registry.COM5.used=0;
	global_hw_registry.COM5.started=0;
	
	
	//Ethernet HW is located under USARTF0 serial 
	global_hw_registry.ETHERNET0.hw_pointer=&USARTF0;
	global_hw_registry.ETHERNET0.number=0;
	global_hw_registry.ETHERNET0.type=ethernet;
	global_hw_registry.ETHERNET0.used=0;
	global_hw_registry.ETHERNET0.started=0;
	
	for(uint8_t i=0;i<TIMERS_COUNT;i++){
	global_hw_registry.TIMERS[i].number=i;
	global_hw_registry.TIMERS[i].started=false;
	global_hw_registry.TIMERS[i].used=false;
	global_hw_registry.TIMERS[i].type=timer;		
	}
	
	for(uint8_t i=0;i<GPIO_COUNT;i++){
		global_hw_registry.GPIO[i].number=0;
		global_hw_registry.GPIO[i].started=false;
		global_hw_registry.GPIO[i].used=false;
		global_hw_registry.GPIO[i].type=gpio;
	}
	
	global_hw_registry.TWI_C.hw_pointer=&TWIC;	
	global_hw_registry.TWI_C.number=0;
	global_hw_registry.TWI_C.type=twi;
	global_hw_registry.TWI_C.used=0;
	global_hw_registry.TWI_C.started=0;
	
	global_hw_registry.TWI_E.hw_pointer=&TWIE;
	global_hw_registry.TWI_E.number=1;
	global_hw_registry.TWI_E.type=twi;
	global_hw_registry.TWI_E.used=0;
	global_hw_registry.TWI_E.started=0;
	
	global_hw_registry.RTC_INT.hw_pointer=&RTC32;
	global_hw_registry.RTC_INT.number=0;
	global_hw_registry.RTC_INT.type=rtc_int;
	global_hw_registry.RTC_INT.used=0;
	global_hw_registry.RTC_INT.started=0;
	
	global_hw_registry.RTC_EXT.hw_pointer=&TWIC;
	global_hw_registry.RTC_EXT.number=0;
	global_hw_registry.RTC_EXT.type=rtc_ext;
	global_hw_registry.RTC_EXT.used=0;
	global_hw_registry.RTC_EXT.started=0;
	
	global_hw_registry.SPI_C.hw_pointer=&SPIC;
	global_hw_registry.SPI_C.number=0;
	global_hw_registry.SPI_C.type=spi;
	global_hw_registry.SPI_C.used=0;
	global_hw_registry.SPI_C.started=0;
		
	global_hw_registry.SPI_D.hw_pointer=&SPID;
	global_hw_registry.SPI_D.number=1;
	global_hw_registry.SPI_D.type=spi;
	global_hw_registry.SPI_D.used=0;
	global_hw_registry.SPI_D.started=0;
	
	global_hw_registry.SD_MMC.hw_pointer=SD_MMC_SPI;
	global_hw_registry.SD_MMC.number=0;
	global_hw_registry.SD_MMC.type=sd_mmc;
	global_hw_registry.SD_MMC.used=0;
	global_hw_registry.SD_MMC.started=0;
	
	global_hw_registry.ADC_INT0.hw_pointer=&ADCA;
	global_hw_registry.ADC_INT0.number=0;
	global_hw_registry.ADC_INT0.type=adc;
	global_hw_registry.ADC_INT0.used=0;
	global_hw_registry.ADC_INT0.started=0;
	
	global_hw_registry.ADC_INT1.hw_pointer=&ADCB;
	global_hw_registry.ADC_INT1.number=1;
	global_hw_registry.ADC_INT1.type=adc;
	global_hw_registry.ADC_INT1.used=0;
	global_hw_registry.ADC_INT1.started=0;	
	
	//global_hw_registry.ADC_EXT.hw_pointer=ADS1254;
	global_hw_registry.ADC_EXT.number=2;
	global_hw_registry.ADC_EXT.type=adc;
	global_hw_registry.ADC_EXT.used=0;
	global_hw_registry.ADC_EXT.started=0;	
	
	//global_hw_registry.GPS.hw_pointer=;
	global_hw_registry.GPS.number=0;
	global_hw_registry.GPS.type=gps;
	global_hw_registry.GPS.used=0;
	global_hw_registry.GPS.started=0;
	
	//global_hw_registry.WIFI.hw_pointer=WIFI;
	global_hw_registry.WIFI.number=0;
	global_hw_registry.WIFI.type=wifi;
	global_hw_registry.WIFI.used=0;
	global_hw_registry.WIFI.started=0;
	
	return true;
	}

struct HW_Object* hal_get_hw_obj(hw_type type, uint8_t hw_number){

	
//int size = sizeof(struct usart_hal_config);
uint8_t id = 0;

	switch(type) {
		
		case usart:
		switch(hw_number){
			case 0:
				if(global_hw_registry.COM0.config_struct==NULL)
					global_hw_registry.COM0.config_struct = malloc(sizeof(struct usart_hal_config));
				if(global_hw_registry.COM0.config_struct!=NULL ){
					global_hw_registry.COM0.used=true;
					return &global_hw_registry.COM0;
				}	
			break;
			
			case 1:
				if(global_hw_registry.COM1.config_struct==NULL )
					global_hw_registry.COM1.config_struct= malloc(sizeof(struct usart_hal_config));
				if(global_hw_registry.COM1.config_struct!=NULL){
					global_hw_registry.COM1.used=true;
					return &global_hw_registry.COM1;
				}
			break;
			
			case 2:
				if(global_hw_registry.COM2.config_struct==NULL )
					global_hw_registry.COM2.config_struct= malloc(sizeof(struct usart_hal_config));
				if(global_hw_registry.COM2.config_struct!=NULL ){
					global_hw_registry.COM2.used=true;
					return &global_hw_registry.COM2;
				}
			break;
			
			case 3:
				if(global_hw_registry.COM3.config_struct==NULL )
					global_hw_registry.COM3.config_struct= malloc(sizeof(struct usart_hal_config));
				if(global_hw_registry.COM3.config_struct!=NULL){
					global_hw_registry.COM3.used=true;
					return &global_hw_registry.COM3;
				}
			break;
			
			case 4:
				if(global_hw_registry.COM4.config_struct==NULL )
					global_hw_registry.COM4.config_struct= malloc(sizeof(struct usart_hal_config));
				if(global_hw_registry.COM4.config_struct!=NULL ){
					global_hw_registry.COM4.used=true;
					return &global_hw_registry.COM4;
				}
			break;
			
			case 5:
				if(global_hw_registry.COM5.config_struct==NULL )
					global_hw_registry.COM5.config_struct= malloc(sizeof(struct usart_hal_config));
				if(global_hw_registry.COM5.config_struct!=NULL ){
					global_hw_registry.COM5.used=true;
					return &global_hw_registry.COM5;
				}
				
				/*
				global_hw_registry.COM5.config_struct = &config_com5;
				if(global_hw_registry.COM5.config_struct!=NULL && global_hw_registry.COM5.used!=true ){
					global_hw_registry.COM5.used=true;
					return &global_hw_registry.COM5;
				}*/
			break;
			
			default:
			break;
		}  
		

		case ethernet:
			if( global_hw_registry.ETHERNET0.config_struct==NULL )
				global_hw_registry.ETHERNET0.config_struct= malloc(sizeof(struct hal_ethernet_config));
			if(global_hw_registry.ETHERNET0.config_struct!=NULL){
				global_hw_registry.ETHERNET0.used=true;
				return &global_hw_registry.ETHERNET0;
			}
			/*
			global_hw_registry.ETHERNET0.config_struct= &config_eth;
			if(global_hw_registry.ETHERNET0.config_struct!=NULL && global_hw_registry.ETHERNET0.used!=true ){
				global_hw_registry.ETHERNET0.used=true;
				return &global_hw_registry.ETHERNET0;
			}*/
		break;
		
		case timer:
			for(id=0;id<TIMERS_COUNT;id++){
				if(global_hw_registry.TIMERS[id].used==false)
					break;
			}
		
			if(id==TIMERS_COUNT)
				break;
			else{
				//global_hw_registry.TIMERS[id].config_struct= malloc(sizeof(struct hal_timer_config));
				//global_hw_registry.TIMERS[id].config_struct= &config_timer[id];
				global_hw_registry.TIMERS[id].used=true;
				return &global_hw_registry.TIMERS[id];
			}
		break;
		
		case gpio:
			//check if the desired pin is already allocated and return that pointer if it is
			for(id=0;id<GPIO_COUNT;id++){
				if(global_hw_registry.GPIO[id].number==hw_number)
					return &global_hw_registry.GPIO[id]; 
			}
			
			//return an empty pointer if available
			for(id=0;id<GPIO_COUNT;id++){
				if(global_hw_registry.GPIO[id].used==false)
					break;
			}
			
			if(id==GPIO_COUNT)
				break;
			else{
				//global_hw_registry.GPIO[id].config_struct= malloc(sizeof(struct gpio_hal_config));
				global_hw_registry.GPIO[id].number=hw_number;
				global_hw_registry.GPIO[id].used=true;
				return &global_hw_registry.GPIO[id];
			}	
		
		break;
		
		case twi:
			switch(hw_number){
			case 0:
				if(global_hw_registry.TWI_C.used!=true )
					global_hw_registry.TWI_C.used=true;
				return &global_hw_registry.TWI_C;
			break;
			
			case 1:
				if(global_hw_registry.TWI_E.used!=true )
					global_hw_registry.TWI_E.used=true;
				return &global_hw_registry.TWI_E;
			break;
			
			default:
			break;				
			}
		
		break;
		
		case rtc_int:
			if(global_hw_registry.RTC_INT.used!=true )
				global_hw_registry.RTC_INT.used=true;
			return &global_hw_registry.RTC_INT;
		break;
			
	
		case rtc_ext:
			if(global_hw_registry.RTC_EXT.used!=true )
				global_hw_registry.RTC_EXT.used=true;
			return &global_hw_registry.RTC_EXT;
		break;
	
		case spi:
		switch(hw_number){
			case 0:
			if(global_hw_registry.SPI_C.used!=true )
				global_hw_registry.SPI_C.used=true;
			return &global_hw_registry.SPI_C;
			
			break;
			
			case 1:
			if(global_hw_registry.SPI_D.used!=true )
				global_hw_registry.SPI_D.used=true;
			return &global_hw_registry.SPI_D;
			
			break;
			
			default:
			break;
		}
		break;
		
		case sd_mmc:
			if(global_hw_registry.SD_MMC.used!=true ){
				global_hw_registry.SD_MMC.used=true;
				return &global_hw_registry.SD_MMC;
			}
		break;	
		
		case adc:
		switch(hw_number){
			case 0:
			if(global_hw_registry.ADC_INT0.used!=true ){
				global_hw_registry.ADC_INT0.used=true;
				return &global_hw_registry.ADC_INT0;
			}
			break;
			
			case 1:
			if(global_hw_registry.ADC_INT1.used!=true ){
				global_hw_registry.ADC_INT1.used=true;
				return &global_hw_registry.ADC_INT1;
			}
			break;
			
			case 2:
			if(global_hw_registry.ADC_EXT.used!=true ){
				global_hw_registry.ADC_EXT.used=true;
				return &global_hw_registry.ADC_EXT;
			}
			break;
			
			default:
			break;
		}
		break;
		
		case gps:
		if(global_hw_registry.GPS.used!=true ){
			global_hw_registry.GPS.used=true;
			return &global_hw_registry.GPS;
		}
		break;
		
		case wifi:
		if(global_hw_registry.WIFI.used!=true )
			global_hw_registry.WIFI.used=true;
		return &global_hw_registry.WIFI;
		
		break;
			
		default:
		return NULL;
		break;
	}
	
	return NULL;
}

bool hal_release_hw_obj(struct HW_Object* hw_pointer){
	
	if(hw_pointer==NULL){
		printf("ERROR null pointer passed to release");
		return false;
	}
	
	switch(hw_pointer->type) {
		
		case usart:
			hw_pointer->used=0;
			if(hw_pointer->started)
				hal_stop_hw_obj(hw_pointer);
			hw_pointer->started=0;
			free(hw_pointer->config_struct);
		break;
		
		//TODO add more hw components as we go on
		case ethernet:
			if(hw_pointer->started)
				hal_stop_hw_obj(hw_pointer);
			
			hal_release_hw_obj(&global_hw_registry.COM5);
			hw_pointer->used=0;
			hw_pointer->started=0;
			free(hw_pointer->config_struct);
		break;
		
		case timer:
			if(hw_pointer->started)
				hal_stop_hw_obj(hw_pointer);
			hw_pointer->used=0;
			hw_pointer->started=0;
			hw_pointer->config_struct=NULL;
			//free(hw_pointer->config_struct);
		break;
		
		case gpio:
			if(hw_pointer->started)
				hal_stop_hw_obj(hw_pointer);
			
			hw_pointer->used=0;
			hw_pointer->number=0;
			hw_pointer->started=0;
			hw_pointer->config_struct=NULL;
			//free(hw_pointer->config_struct);
		break;
		
		case twi:
			if(hw_pointer->started)
				hal_stop_hw_obj(hw_pointer);
			
			hw_pointer->used=0;
			hw_pointer->started=0;
			hw_pointer->config_struct=NULL;
		break;
		
		case rtc_int:
			if(hw_pointer->started)
				hal_stop_hw_obj(hw_pointer);
			
			hw_pointer->used=0;
			hw_pointer->started=0;
			hw_pointer->config_struct=NULL;
		break;
		
		case rtc_ext:
			if(hw_pointer->started)
				hal_stop_hw_obj(hw_pointer);
		
			hal_release_hw_obj(hw_pointer->hw_pointer);
			hw_pointer->used=0;
			hw_pointer->started=0;
			hw_pointer->config_struct=NULL;
		break;
		
		case spi:
			if(hw_pointer->started)
				hal_stop_hw_obj(hw_pointer);
			
			hw_pointer->used=0;
			hw_pointer->started=0;
			hw_pointer->config_struct=NULL;
		break;
		
		case sd_mmc:
		//Releasing com2 since sd_mmc uses com2 as software spi
			if(hw_pointer->started)
				hal_stop_hw_obj(hw_pointer);
			
			hal_release_hw_obj(&global_hw_registry.COM2);
			hw_pointer->used=0;
			hw_pointer->started=0;
			hw_pointer->config_struct=NULL;
		break;
		
		case adc:
			if(hw_pointer->started)
				hal_stop_hw_obj(hw_pointer);
			hw_pointer->used=0;
			hw_pointer->started=0;
			hw_pointer->config_struct=NULL;
		break;
		
		case gps:
			if(hw_pointer->started)
				hal_stop_hw_obj(hw_pointer);
			
			hal_release_hw_obj(&global_hw_registry.COM0);
			hw_pointer->used=0;
			hw_pointer->started=0;
			hw_pointer->config_struct=NULL;
		break;
		
		case wifi:
		if(hw_pointer->started)
		hal_stop_hw_obj(hw_pointer);
		
		hal_release_hw_obj(&global_hw_registry.COM4);
		hw_pointer->used=0;
		hw_pointer->started=0;
		hw_pointer->config_struct=NULL;

		break;
		
		
		default:
		return false;
		break;
	}

	return true;
}

bool hal_init_hw_obj(struct HW_Object* hw_pointer){
	
	if(hw_pointer==NULL){
		printf("ERROR null pointer passed to init");	
		return false;
	}
	if(hw_pointer->started)
		return true;
		
	
	switch(hw_pointer->type) {
	
		case usart:
			return hal_usart_init(hw_pointer);
		break;
	
		case ethernet:
			return ethernet_init(hw_pointer);
		break;
		
		case timer:
			return hal_timer_init(hw_pointer);
		break;
		
		case gpio:
			return hal_gpio_init(hw_pointer);
		break;
		
		case twi:
			return hal_twi_init(hw_pointer);
		break;
		
		case rtc_ext:
			return hal_system_time_init(hw_pointer);
		break;
		
		case spi:
			return hal_spi_init(hw_pointer);
		break;
		
		case sd_mmc:
			return hal_sd_mmc_init(hw_pointer);
		break;
	
		case adc:
			return hal_adc_init(hw_pointer);
		break;
		
		case gps:
			return hal_gps_init(hw_pointer);
		break;
		
		case wifi:
			return hal_wifi_init(hw_pointer);
		break;
		
		default:
		return false;
		break;
	}


return false;
}

bool hal_set_config_hw_obj(struct HW_Object* hw_pointer,void* config){
	
	if(hw_pointer==NULL){
		printf("ERROR null pointer passed to set config");
		return false;
	}
	switch(hw_pointer->type) {
		
		case usart:
			return hal_usart_set_config(hw_pointer, (usart_config*) config);
		break;
		
		//TODO add more hw components as we go on
		case ethernet:
			return ethernet_set_config(hw_pointer, (struct hal_ethernet_config*) config);
		break;
		
		case timer:
			return hal_timer_set_config(hw_pointer, (struct hal_timer_config*) config);
		break;
		
		case gpio:
			return hal_gpio_set_config(hw_pointer, (struct gpio_hal_config*) config);
		break;
		
		case twi:
			return hal_twi_set_config(hw_pointer, (struct hal_twi_config*) config);
		break;
		
		case rtc_ext:
			return true;
		break;
				
		case spi:
			return hal_spi_set_config(hw_pointer, (struct hal_spi_config*) config);
		break;
		
		case sd_mmc:
			return hal_sd_mmc_set_config(hw_pointer, (struct hal_sd_mmc_config*) config);
		break;
		
		case adc:
			return hal_adc_set_config(hw_pointer, (struct hal_adc_config*) config);
		break;
		
		case gps:
			return gps_set_config(hw_pointer, (struct hal_gps_config*) config);
		break;
		
		case wifi:
			return hal_wifi_set_config(hw_pointer, (struct hal_wifi_config*) config);
		break;
		
		default:
		return false;
		break;
	}


	return false;
}

bool hal_get_config_hw_obj(struct HW_Object* hw_pointer,void* config){
	
	if(hw_pointer==NULL){
		printf("ERROR null pointer passed to get config");
		return false;
	}
	switch(hw_pointer->type) {
		
		case usart:
			return hal_usart_get_config(hw_pointer, (usart_config*) config);
		break;
		
		case ethernet:
			return ethernet_get_config(hw_pointer, (struct hal_ethernet_config*) config);
		break;
		
		case timer:
			return hal_timer_get_config(hw_pointer, (struct hal_timer_config*) config);
		break;
		
		case gpio:
			return hal_gpio_get_config(hw_pointer, (struct gpio_hal_config*) config);
		break;
		
		case twi:
			return hal_twi_get_config(hw_pointer, (struct hal_twi_config*) config);
		break;
		
		case rtc_ext:
			return true;
		break;
		
		case spi:
			return hal_spi_get_config(hw_pointer, (struct hal_spi_config*) config);
		break;
		
		case sd_mmc:
			return hal_sd_mmc_set_config(hw_pointer, (struct hal_sd_mmc_config*) config);
		break;

		case adc:
			return hal_adc_get_config(hw_pointer, (struct hal_adc_config*) config);
		break;
			
		case gps:
			return gps_get_config(hw_pointer, (struct hal_gps_config*) config);
		break;
			
		case wifi:
			return hal_wifi_get_config(hw_pointer, (struct hal_wifi_config*) config);
		break;
			
		default:
			return false;
		break;
	}


	return false;
}

bool hal_start_hw_obj(struct HW_Object* hw_pointer){
	
	if(hw_pointer==NULL){
		printf("ERROR null pointer passed to start");
		return false;
	}
	if(hw_pointer->started)
		return true;
	
	switch(hw_pointer->type) {
		
		case usart:
			return hal_usart_start(hw_pointer);
		break;
		
		case ethernet:
			return ethernet_start(hw_pointer);
		break;
		
		case timer:
			return hal_timer_start(hw_pointer);
		break;
		
		case gpio:
			return hal_gpio_start(hw_pointer);
		break;
		
		case twi:
			return hal_twi_start(hw_pointer);
		break;
		
		case rtc_ext:
			return hal_system_time_start(hw_pointer);
		break;
		
		case spi:
			return hal_spi_start(hw_pointer);
		break;
		
		case adc:
			return hal_adc_start(hw_pointer);
		break;
		
		case sd_mmc:
			return hal_sd_mmc_start(hw_pointer);
		break;
		
		case gps:
			return gps_start(hw_pointer);
		break;
		
		case wifi:
			return hal_wifi_start(hw_pointer);
		break;
		
		default:
		return false;
		break;
	}


	return false;
}

bool hal_stop_hw_obj(struct HW_Object* hw_pointer){
	
	if(hw_pointer==NULL){
		printf("ERROR null pointer passed to stop");
		return false;
	}
	
	if(!hw_pointer->started)
		return true;
	
	switch(hw_pointer->type) {
		
		case usart:
			return hal_usart_stop(hw_pointer);
		break;
		
		case ethernet:
			return ethernet_stop(hw_pointer);
		break;
		
		case timer:
			return hal_timer_stop(hw_pointer);
		break;
		
		case gpio:
			return hal_gpio_stop(hw_pointer);
		break;
		
		case twi:
			return hal_twi_stop(hw_pointer);
		break;
		
		case rtc_ext:
			return hal_system_time_stop(hw_pointer);
		break;
		
		case spi:
			return hal_spi_stop(hw_pointer);
		break;
		
		case sd_mmc:
			return hal_sd_mmc_stop(hw_pointer);
		break;
		
		case adc:
		return hal_adc_stop(hw_pointer);
		break;
		
		case gps:
		return gps_stop(hw_pointer);
		break;
		
		case wifi:
		return hal_wifi_stop(hw_pointer);
		break;
		
		default:
		return false;
		break;
	}

}

