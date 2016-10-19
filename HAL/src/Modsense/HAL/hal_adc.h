/**
 * \addtogroup hal
 * @{
 */

/**
 * \defgroup hal_ADC ADC Hardware Abstraction Layer
 *
 * This is the HAL API for the ADC hardware module
 *
 * @{
 */

/**
 * \file
 * Header file for the ADC HAL
 * \author
 * Spilios Giannoulis <sgiannoulis@isi.gr>
 *
 */


#ifndef HAL_ADC_H_
#define HAL_ADC_H_

#include <asf.h>
#include "compiler.h"
#include "hal_interface.h"
#include "ads1254.h"

typedef enum available_adc {
	
	int_ADCA,
	int_ADCB,
	ext_ADS1254

} available_adc;


//! Structure to hold the ADC config parameters 
struct hal_adc_config {
	
	//!selecting internal  ADCA, ADCB or external ADC ADS1254 to use
	 available_adc adc_device;

	//! Configuration parameters for the internal ADC module
	/**@{ */
		
	//! Conversion sign
	enum adc_sign sign;
	//! Conversion resolution
	enum adc_resolution resolution;
	//! Reference voltage
	enum adc_reference ref;
	//! Triggering method
	enum adc_trigger trig;
	//! Channel to use
	uint8_t nr_of_ch;
	//! Event channel to use as trigger if event triggering used
	uint8_t base_ev_ch;
	
	//!Internal or external selection
	bool internal_input;
	//!internal input selection if internal input used
	uint8_t int_input_sel;
	
	//! Positive input selection if external input used
	enum adcch_positive_input pos_ext_input;
	//! Negative input selection if external input used
	enum adcch_negative_input neg_ext_input;
	//! Gain selection
	uint8_t gain;
	
	//! Interrupt mode if enabled
	enum adcch_mode int_mode;
	
	
	//! Pointer to function to call if interrupt is enabled at end of conversion for internal ADC
	adc_callback_t callback;
	
	/** @} */
	
	//! Configuration parameters for the selected channel
	//struct adc_channel_config adcch_conf;
	
	//! Configuration parameters for the external ADC module
	/**@{ */
			
	/** @} */
		
} hal_adc_config;

/**
 * Initialize the ADC module.
 *
 * \param hw_device A pointer to the ADC hardware module.
 *
 * \retval True if initialization was successful
 */
bool hal_adc_init( struct HW_Object * hw_device);

/**
 * Set the configuration parameters to the ADC module
 *
 * \param hw_device A pointer to the ADC hardware module.
 *
 * \param config A pointer to the ADC configuration struct holding all user config data
 *
 * \retval True if configuration was successful
 */
bool hal_adc_set_config(struct HW_Object* hw_device, struct hal_adc_config* config);

/**
 * Get the configuration parameters from the ADC module
 *
 * \param hw_device A pointer to the ADC hardware module.
 *
 * \param config A pointer to the ADC configuration struct to be filled with ADC config data
 *
 * \retval True if getting configuration parameters was successful
 */
bool hal_adc_get_config(struct HW_Object* hw_device, struct hal_adc_config* config);

/**
 * Starts the conversion in the ADC module
 *
 * \param hw_device A pointer to the ADC hardware module.
 *
 */
bool hal_adc_start(struct HW_Object* hw_device);

/**
 * Stops the ADC module.
 *
 * \param hw_device A pointer to the ADC hardware module.
 *
 */
bool hal_adc_stop(struct HW_Object* hw_device);


 
//* internal ADC usage routines
bool hal_int_adc_start_conversion(struct HW_Object* hw_device,uint8_t ch_mask);
//Routines if no interrupt mode is enabled
void hal_int_adc_wait_for_conversion_end(struct HW_Object* hw_device,uint8_t ch_mask);
int16_t hal_int_adc_get_signed_result(struct HW_Object* hw_device, uint8_t ch_mask);
uint16_t hal_int_adc_get_unsigned_result(struct HW_Object* hw_device, uint8_t ch_mask);


//*external ADC usage routines
void hal_ext_adc_read_result(ext_adc_result* result);
void hal_ext_adc_wait_for_conversion_end(struct HW_Object* hw_device);
//void read_ads1254_result(ext_adc_result* result);
//bool read_done_ads1254(void);



#endif /* HAL_ADC_H_ */

/** @} */
/** @} */
