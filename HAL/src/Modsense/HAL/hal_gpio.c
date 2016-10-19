/*
 * hal_usart.c
 *
 * Created: 13/6/2013 4:43:04 μμ
 *  Author: spilios
 */ 
/**
 * \file
 * Source file for the GPIO HAL
 * \author
 * Spilios Giannoulis <sgiannoulis@isi.gr>
 *
 */ 

#include "hal_gpio.h"
#include "hal_interface.h"

bool hal_gpio_init( struct HW_Object * hw_device){
	return true;
}
	

bool hal_gpio_set_config(struct HW_Object* hw_device, struct gpio_hal_config* config){
	
	if(hw_device==NULL || config==NULL)
		return false;
	
	hw_device->config_struct = config;
	struct gpio_hal_config* hw_config = (struct gpio_hal_config*) hw_device->config_struct;
/*	struct gpio_hal_config* hw_config = (struct gpio_hal_config*) hw_device->config_struct;
	
	hw_config->flags = config->flags;
	hw_config->interrupt_level=config->interrupt_level;
	hw_config->use_int_team0=config->use_int_team0;
*/	
	ioport_configure_pin((port_pin_t)hw_device->number, hw_config->flags);
	
	if(hw_config->interrupt_level){
		PORT_t * port_name = arch_ioport_pin_to_base((port_pin_t)hw_device->number);
		//port_name->PIN2CTRL |=IOPORT_SENSE_FALLING;
		if(hw_config->use_int_team0){
		//	port_name->INT0MASK |= arch_ioport_pin_to_mask((port_pin_t)hw_device->number);
			port_name->INTCTRL |= hw_config->interrupt_level;
		}
		else{ 
		//	port_name->INT1MASK |= arch_ioport_pin_to_mask((port_pin_t)hw_device->number);
			port_name->INTCTRL |= (hw_config->interrupt_level)<<2;
		}
		//PORTB.INT0MASK = 0x04;
	}
	
	return true;
	
}


bool hal_gpio_get_config(struct HW_Object* hw_device, struct gpio_hal_config* config){
	
	if(hw_device==NULL || config==NULL)
		return false;
	
	struct gpio_hal_config* hw_config = (struct gpio_hal_config*) hw_device->config_struct;
	
	 config->flags = hw_config->flags ;
	
	return true;
	
}

bool hal_gpio_start(struct HW_Object* hw_device){
	
	if(hw_device==NULL)
		return false;
		
	hw_device->started=true;
	
	struct gpio_hal_config* hw_config = (struct gpio_hal_config*) hw_device->config_struct;
		
	if(hw_config->interrupt_level){
		PORT_t * port_name = arch_ioport_pin_to_base((port_pin_t)hw_device->number);
		
		if(hw_config->use_int_team0){
			port_name->INT0MASK |= arch_ioport_pin_to_mask((port_pin_t)hw_device->number);
		}
		else{
			port_name->INT1MASK |= arch_ioport_pin_to_mask((port_pin_t)hw_device->number);
		}
		
	}
	
	return true;
}


bool hal_gpio_stop(struct HW_Object* hw_device){
	
	if(hw_device==NULL)
		return false;
		
	hw_device->started=false;
	
	struct gpio_hal_config* hw_config = (struct gpio_hal_config*) hw_device->config_struct;
	
	
	if(hw_config->interrupt_level){
		PORT_t * port_name = arch_ioport_pin_to_base((port_pin_t)hw_device->number);
		
		if(hw_config->use_int_team0){
			port_name->INT0MASK &= ~arch_ioport_pin_to_mask((port_pin_t)hw_device->number);
		//	port_name->INTCTRL &=~hw_config->interrupt_level;
		}
		else{
			port_name->INT1MASK &= ~arch_ioport_pin_to_mask((port_pin_t)hw_device->number);
		//	port_name->INTCTRL &= ~(hw_config->interrupt_level)<<2;
		}
		
	}
	
	return true;
}

bool hal_gpio_toggle(struct HW_Object* hw_device){
	
	if(hw_device==NULL)
		return false;
		
	if(hw_device->started){
		ioport_toggle_pin(hw_device->number);
		return true;
	}
	
	return false;
}

bool hal_gpio_high(struct HW_Object* hw_device){
	if(hw_device==NULL)
		return false;
		
	if(hw_device->started){
		ioport_set_value(hw_device->number,1);
		return true;
	}
	return false;
}

bool hal_gpio_low(struct HW_Object* hw_device){
	
	if(hw_device==NULL)
		return false;
		
	if(hw_device->started){
		ioport_set_value(hw_device->number,0);
		return true;
	}
	return false;
}

bool hal_gpio_is_low(struct HW_Object* hw_device){
	
	if(hw_device==NULL)
		return false;
		
	return ioport_pin_is_low(hw_device->number);

}

bool hal_gpio_is_high(struct HW_Object* hw_device){
	if(hw_device==NULL)
		return false;
	
	return ioport_pin_is_high(hw_device->number);
}

/** @} */