/*
 * hal_timers.c
 *
 * Created: 20/6/2013 6:12:53 μμ
 *  Author: spilios
 */ 

#include "hal_timers.h"

bool hal_timer_init(struct HW_Object* hw_device){
	
	if(hw_device==NULL)
		return false;
	
	return timeout_init();	
	}
	
	
bool hal_timer_set_config(struct HW_Object* hw_device, struct hal_timer_config* config){
	
	if(hw_device==NULL || config==NULL)
		return false;
	
	hw_device->config_struct = config;
	
/*	struct hal_timer_config* hw_config = (struct hal_timer_config*) hw_device->config_struct;
	
	hw_config->callback = config->callback ;
	hw_config->period_timeout = config->period_timeout ;
	hw_config->type =config->type ;
*/	
	return true;
	}
	
	
bool hal_timer_get_config(struct HW_Object* hw_device, struct hal_timer_config* config){
	
	if(hw_device==NULL )
		return false;
		
	if(config==NULL) { //passed empty pointer, return pointer to config struct
		config = (struct hal_timer_config*) hw_device->config_struct;
	}
	else{ //passed pointer to a current allocated struct, copy variables

		struct hal_timer_config* hw_config = (struct hal_timer_config*) hw_device->config_struct;
		
		config->callback = hw_config->callback ;
		config->period_timeout = hw_config->period_timeout ;
		config->type = hw_config->type ;
	}
	return true;
}


bool hal_timer_start(struct HW_Object* hw_device){
	
	if(hw_device==NULL)
		return false;
	
	struct hal_timer_config* hw_config = (struct hal_timer_config*) hw_device->config_struct;
	if(hw_config==NULL)
		return false;
		
	timeout_set_callback((timeout_id_t)hw_device->number, hw_config->callback);
	
	if(hw_config->type==periodic)
		timeout_start_periodic((timeout_id_t)hw_device->number, hw_config->period_timeout);
	else if(hw_config->type==single)
		timeout_start_singleshot((timeout_id_t)hw_device->number, hw_config->period_timeout);
	
	return true;	
}

bool hal_timer_restart(struct HW_Object*  hw_device){
	
	if(hw_device==NULL)
		return false;
	
	struct hal_timer_config* hw_config = (struct hal_timer_config*) hw_device->config_struct;
	if(hw_config==NULL)
		return false;
		
	if(hw_config->type==periodic)
		timeout_start_periodic((timeout_id_t)hw_device->number, hw_config->period_timeout);
	else if(hw_config->type==single)
		timeout_start_singleshot((timeout_id_t)hw_device->number, hw_config->period_timeout);
	
	return true;
}

bool hal_timer_stop(struct HW_Object* hw_device){

	timeout_stop((timeout_id_t) hw_device->number);
	return true;
}


bool hal_timer_expired(struct HW_Object* hw_device){
	
	return timeout_test_and_clear_expired((timeout_id_t) hw_device->number);
}

