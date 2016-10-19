/**
 * \addtogroup hal_ADC
 * @{
 */



/**
 * \file
 * Source file for the ADC HAL
 * \author
 * Spilios Giannoulis <sgiannoulis@isi.gr>
 *
 */ 


#include "hal_adc.h"
#include <string.h>

	
bool hal_adc_init( struct HW_Object * hw_device){
	
	if(hw_device->number==2)
		init_ads1254();	
		
	return true;
		
}



bool hal_adc_set_config(struct HW_Object* hw_device, struct hal_adc_config* config){
	
	struct adc_config         adc_conf;
	struct adc_channel_config adcch_conf;
	
	hw_device->config_struct=config;
	
	if(hw_device->number<2){
		// Clear the configuration structures.
		memset(&adc_conf, 0, sizeof(struct adc_config));
		memset(&adcch_conf, 0, sizeof(struct adc_channel_config));
	
		 adc_read_configuration(hw_device->hw_pointer, &adc_conf);
		 adcch_read_configuration(hw_device->hw_pointer, config->nr_of_ch, &adcch_conf);
	 
		adc_set_conversion_parameters(&adc_conf, config->sign, config->resolution,config->ref);
		adc_set_clock_rate(&adc_conf, 2000000UL);
		adc_set_conversion_trigger(&adc_conf, config->trig, config->nr_of_ch, config->base_ev_ch);

		if(config->internal_input)
			adc_enable_internal_input(&adc_conf, config->int_input_sel);

		adc_write_configuration(hw_device->hw_pointer, &adc_conf);
		adc_set_callback(hw_device->hw_pointer, config->callback);

		/* Configure ADC channel */
		if(!config->internal_input)
			adcch_set_input(&adcch_conf,config->pos_ext_input, config->neg_ext_input, config->gain);
		adcch_set_interrupt_mode(&adcch_conf, config->int_mode);
		adcch_enable_interrupt(&adcch_conf);

		adcch_write_configuration(hw_device->hw_pointer, (1U << config->nr_of_ch), &adcch_conf);
		
	}
	
	return true;
	
}



bool hal_adc_get_config(struct HW_Object* hw_device, struct hal_adc_config* config){
	
	struct hal_adc_config* hw_config = (struct hal_adc_config*)hw_device->config_struct;
	
	config->adc_device = hw_config->adc_device;
	config->base_ev_ch = hw_config->base_ev_ch;
	config->callback = hw_config->callback;
	//config->clk_adc = hw_config->clk_adc;
	config->gain = hw_config->gain;
	config->int_input_sel = hw_config->int_input_sel;
	config->internal_input = hw_config->internal_input;
	config->int_mode = hw_config->int_mode;
	config->neg_ext_input = hw_config->neg_ext_input;
	config->nr_of_ch = hw_config->nr_of_ch;
	config->pos_ext_input = hw_config->pos_ext_input;
	config->ref = hw_config->ref;
	config->resolution = hw_config->resolution;
	config->sign = hw_config->sign;
	config->trig = hw_config->trig;
	
	return true;
}


bool hal_adc_start(struct HW_Object* hw_device){
	
	if(hw_device->number<2){
		adc_enable(hw_device->hw_pointer);
	}
	
	if(!cpu_irq_is_enabled())
		cpu_irq_enable();
	
	return true;
}


bool hal_adc_stop(struct HW_Object* hw_device){
	
	if(hw_device->number<2){
		adc_disable(hw_device->hw_pointer);
	}
	return true;
}


//* internal ADC usage routines
bool hal_int_adc_start_conversion(struct HW_Object* hw_device,uint8_t ch_mask){
	
	adc_start_conversion(hw_device->hw_pointer, ch_mask);
	
	return true;
}
//Routines if no interrupt mode is enabled
void hal_int_adc_wait_for_conversion_end(struct HW_Object* hw_device, uint8_t ch_mask){
	
	adc_wait_for_interrupt_flag(hw_device->hw_pointer, ch_mask);
	
}

int16_t hal_int_adc_get_signed_result(struct HW_Object* hw_device, uint8_t ch_mask){
	
	return adc_get_signed_result(hw_device->hw_pointer, ch_mask);
}

uint16_t hal_int_adc_get_usigned_result(struct HW_Object* hw_device, uint8_t ch_mask){
	
	return adc_get_unsigned_result(hw_device->hw_pointer, ch_mask);
}


//*external ADC usage routines
void hal_ext_adc_read_result(ext_adc_result* result){
	read_ads1254_result(result);
}

void hal_ext_adc_wait_for_conversion_end(struct HW_Object* hw_device){
	
	while(!read_done_ads1254());
		
}


//void read_ads1254_result(ext_adc_result* result);
//bool read_done_ads1254(void);


	

/** @} */