/**
 * \addtogroup hal
 * @{
 */

/**
 * \defgroup hal_twi TWI Hardware Abstraction Layer
 *
 * This is the HAL API for the TWI hardware module
 *
 * @{
 */

/**
 * \file
 * Header file for the TWI HAL
 * \author
 * Spilios Giannoulis <sgiannoulis@isi.gr>
 *
 */ 


#ifndef HAL_TWI_H_
#define HAL_TWI_H_


#include <asf.h>
#include "compiler.h"
#include "hal_interface.h"


//! Input parameters when initializing TWI and similar modes.
struct hal_twi_config {
	
	//! Common mode parameters
	// @{
	//! Set the TWI working mode , master or slave
	bool master;
	//! Select our chip address.
	char chip_address;
	// @} 
	
	//! Master mode parameters
	//! The baudrate of the TWI bus.
	unsigned long speed;
	
	
	  
	//!slave mode parameters
	// @{
	//! Pointer to preallocated slave twi structure
	TWI_Slave_t* twi;
	
	//! Handler for processing incoming data
	void (*processDataFunction) (void);
	// @} 
}hal_twi_config ;

struct HW_Object;

/**
 * Initialize the TWI hardware module
 *
 * \param hw_device A pointer to the TWI hardware module.
 *
 * \retval True if initialization was successful
 */
bool hal_twi_init( struct HW_Object* hw_device);

/**
 * Set the configuration parameters to the TWI module
 *
 * \param hw_device A pointer to the TWI hardware module.
 *
 * \param config A pointer to the TWI configuration struct holding all user config data
 *
 * \retval True if configuration was successful
 */
bool hal_twi_set_config(struct HW_Object* hw_device, struct hal_twi_config* config);

/**
 * Get the configuration parameters from the TWI module
 *
 * \param hw_device A pointer to the TWI hardware module.
 *
 * \param config A pointer to the TWI configuration struct to hold all TWI config data
 *
 * \retval True if getting configuration parameters was successful
 */
bool hal_twi_get_config(struct HW_Object* hw_device, struct hal_twi_config* config);

/**
 * Starts the TWI module
 *
 * \param hw_device A pointer to the TWI hardware module.
 *
 * \retval True if start was successful
 */
bool hal_twi_start(struct HW_Object* hw_device);

/**
 * Stops the TWI module
 *
 * \param hw_device A pointer to the TWI hardware module.
 *
 * \retval True if stop was successful
 */
bool hal_twi_stop(struct HW_Object*  hw_device);



/**
 * Read data using the TWI module, ONLY for master mode
 *
 * \param hw_device A pointer to the TWI hardware module.
 *
 * \param packet Pointer to the packet to store the data received from slave.
 *
 * \retval Type of status_code_t , indicating result status.
 */
status_code_t hal_twi_read_data(struct HW_Object*  hw_device, twi_package_t* packet);

/**
 * Write data using the TWI module, ONLY for master mode
 *
 * \param hw_device A pointer to the TWI hardware module.
 *
 * \param packet Pointer to the packet containing the data to be send to slave. 
 *
 * \retval Type of status_code_t , indicating result status.
 */
status_code_t hal_twi_write_data(struct HW_Object*  hw_device, twi_package_t* packet);



#endif /* HAL_TWI_H_ */

/** @} */
/** @} */
