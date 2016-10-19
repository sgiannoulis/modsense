/*
 * hal_spi.c
 *
 * Created: 2/8/2013 2:19:36 μμ
 *  Author: spilios
 */ 

#include "hal_spi.h"
#include "spi_master.h"

//#include "hal_interface.h"




bool hal_spi_init( struct HW_Object* hw_device){
	
	spi_master_init(hw_device->hw_pointer);
	return true;
}



bool hal_spi_set_config(struct HW_Object* hw_device, struct hal_spi_config* config){
	

	struct spi_device temp_spi;
	
	hw_device->config_struct = config;
	spi_master_setup_device(hw_device->hw_pointer,&temp_spi,config->spi_mode,config->baudrate,0);
	
	return true;
}




bool hal_spi_get_config(struct HW_Object* hw_device, struct hal_spi_config* config){
	
	struct hal_spi_config* hw_config = (struct hal_spi_config*)hw_device->config_struct;
	
	config->baudrate = hw_config->baudrate;
	config->spi_mode = hw_config->spi_mode;
	
	return true;
}


bool hal_spi_start(struct HW_Object* hw_device){
	
	if(!hw_device->started)
		spi_enable(hw_device->hw_pointer);
	
	hw_device->started=true;
	
	return true;
}


bool hal_spi_device_select(struct HW_Object* hw_device,uint8_t spi_slave_id){
		
	struct spi_device temp_spi;
	temp_spi.id= spi_slave_id;
	
	spi_select_device(hw_device->hw_pointer,&temp_spi);
	
	return true;
}

bool hal_spi_device_deselect(struct HW_Object* hw_device, uint8_t spi_slave_id){
	
	struct spi_device temp_spi;
	temp_spi.id=spi_slave_id;
	
	spi_deselect_device(hw_device->hw_pointer,&temp_spi);
	
	return true;
}
	
	
bool hal_spi_stop(struct HW_Object*  hw_device){
	
	if(hw_device->started)
		spi_disable(hw_device->hw_pointer);
	hw_device->started=false;
	
	return true;
}



bool hal_spi_send_byte(struct HW_Object*  hw_device, uint8_t byte_data){
	
	spi_write_single(hw_device->hw_pointer, byte_data);		
	return true;
}



bool hal_spi_receive_byte(struct HW_Object*  hw_device, uint8_t* byte_data){
		
	spi_read_single(hw_device->hw_pointer, byte_data);
	return true;
}



bool hal_spi_send_string(struct HW_Object*  hw_device, uint8_t* string, uint8_t size){
	
	if(spi_write_packet(hw_device->hw_pointer, string,size)==STATUS_OK)
		return true;
	else 
		return false;
}


uint8_t hal_spi_receive_string(struct HW_Object*  hw_device, uint8_t* buffer, uint8_t size)	{
	
	if(spi_read_packet(hw_device->hw_pointer, buffer,size)==STATUS_OK)
		return true;
	else
		return false;
}