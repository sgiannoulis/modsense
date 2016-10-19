/**
 * \addtogroup hal_twi
 * @{
 */

/**
 * \file
 * Source file for the TWI HAL
 * \author
 * Spilios Giannoulis <sgiannoulis@isi.gr>
 *
 */ 

//this &TWIC must be in hw_object->hw_pointer

//#define TWIC    (*(TWI_t *) 0x0480)  /* Two-Wire Interface */
//#define TWIE    (*(TWI_t *) 0x04A0)  /* Two-Wire Interface */

#include "hal_twi.h"

#define TWI_INTERRUPT_PRIORITY_BM PMIC_LVL_HIGH  //if changed must change accordingly the below define
//#define TWI_INTERRUPT_PRIORITY TWI_INT_LVL_MED


bool hal_twi_init( struct HW_Object* hw_device){
	
	 if(hw_device==NULL)
		return false;
	 
	 //irq_initialize_vectors();
	
	struct hal_twi_config* config = (struct hal_twi_config*) hw_device->config_struct;
	 
	 
	 if(config==NULL)  //TODO if no config has been set in twi what to do  ?
		return false;
	 
	  twi_options_t m_options = {
		    .speed     = config->speed,
		    .chip      = config->master,
		    .speed_reg = TWI_BAUD(sysclk_get_cpu_hz(), config->speed),
			.priority  = TWI_INTERRUPT_PRIORITY_BM
	   };
	  
	 sysclk_enable_peripheral_clock(hw_device->hw_pointer);
	 
	 if(config->master){ //chip wants to be master
		 if(twi_master_init(hw_device->hw_pointer, &m_options)==STATUS_OK)
			return true;
	 }
	 else {  //chip is configured as slave
		 TWI_SlaveInitializeDriver(config->twi , hw_device->hw_pointer, *config->processDataFunction);
		return true;
	 }
	 
	 if(!pmic_level_is_enabled(TWI_INTERRUPT_PRIORITY_BM))
	 pmic_enable_level(TWI_INTERRUPT_PRIORITY_BM);
	 
	 if(!cpu_irq_is_enabled())
	 cpu_irq_enable();
	 
	 return false;
}

bool hal_twi_set_config(struct HW_Object* hw_device, struct hal_twi_config* config){
	
	if(hw_device==NULL||config==NULL)
		return false;
		
	  hw_device->config_struct = config;
	  
	  return true;
}

bool hal_twi_get_config(struct HW_Object* hw_device, struct hal_twi_config* config){
	
	if(hw_device==NULL)
		return false;
		
	config = (struct hal_twi_config*) hw_device->config_struct;
	
	if(config==NULL)
		return false;
	else 
		return true;	
}


bool hal_twi_start(struct HW_Object* hw_device){
	
	if(hw_device==NULL)
		return false;

	struct hal_twi_config* config = (struct hal_twi_config*) hw_device->config_struct;
	
	if(config==NULL)
		return false;
	
	if(config->master)
		twi_master_enable(hw_device->hw_pointer);
	else
		TWI_SlaveInitializeModule(config->twi, config->chip_address, TWI_SLAVE_INTLVL_MED_gc);
	
	cpu_irq_enable();
	
	hw_device->started=true;
	
	return true;
}


	
bool hal_twi_stop(struct HW_Object* hw_device){
	
	if(hw_device==NULL)
		return false;

	struct hal_twi_config* config = (struct hal_twi_config*) hw_device->config_struct;
	
	if(config==NULL)
		return false;
	
	if(config->master)
		twi_master_disable(hw_device->hw_pointer);
	else
		twi_slave_disable(hw_device->hw_pointer);
	
	hw_device->started=false;
	
	return true;
}


status_code_t hal_twi_read_data(struct HW_Object*  hw_device, twi_package_t* packet){
	
	if(hw_device->started)
		return twi_master_transfer (hw_device->hw_pointer, packet, true);
	else return ERR_BUSY;
}


status_code_t hal_twi_write_data(struct HW_Object*  hw_device, twi_package_t* packet){
	
	if(hw_device->started)
		return twi_master_transfer (hw_device->hw_pointer, packet, false);
	else return ERR_BUSY;
}


 ISR(TWIC_TWIS_vect) {
	if(global_hw_registry.TWI_C.started) 
		TWI_SlaveInterruptHandler((TWI_Slave_t* )&TWIC);
 }
 
 ISR(TWIE_TWIS_vect) {
	  if(global_hw_registry.TWI_E.started)
		 TWI_SlaveInterruptHandler((TWI_Slave_t* )&TWIE);
  }
  

/** @} */
