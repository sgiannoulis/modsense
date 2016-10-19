/**
 * \addtogroup hal
 * @{
 */

/**
 * \defgroup hal_GPS GPS Hardware Abstraction Layer
 *
 * This is the HAL API for the GPS hardware module
 *
 * @{
 */

/**
 * \file
 * Header file for the GPS HAL
 * \author
 * Spilios Giannoulis <sgiannoulis@isi.gr>
 *
 */


#ifndef HAL_GPS_H_
#define HAL_GPS_H_

#include <asf.h>
#include "compiler.h"
#include "hal_interface.h"
#include "definitions.h"


struct hal_gps_config {
	
	//! Type of messages to receive
	uint8_t messages_mask;
	
	//! Duty cycling period, check every N seconds
	uint16_t gps_on_period;

} hal_gps_config;

typedef struct gps_data{
	bool gps_locked;
	unsigned char date[7];          // 0, 0, 0
	unsigned char time[7];          // 0, 0, 0
	uint8_t bin_date[3];          // 0, 0, 0
	uint8_t bin_time[3];          // 0, 0, 0
	char latitude[12];              // 3814.4648N
	char longitude[13];             // 02143.8936E
}gps_data_t;




/**
 * Initialize the GPS hardware module
 *
 * \param hw_device A pointer to the GPS hardware module.
 *
 * \retval True if initialization was successful
 */
bool hal_gps_init( struct HW_Object * hw_device);

/**
 * Set the configuration parameters to the GPS module
 *
 * \param hw_device A pointer to the GPS hardware module.
 *
 * \param config A pointer to the GPS configuration struct holding all user config data
 *
 * \retval True if configuration was successful
 */
bool gps_set_config(struct  HW_Object* hw_device, struct hal_gps_config* config);

/**
 * Get the configuration parameters from the GPS module
 *
 * \param hw_device A pointer to the GPS hardware module.
 *
 * \param config A pointer to the GPS configuration struct to hold all GPS config data
 *
 * \retval True if getting configuration parameters was successful
 */
bool gps_get_config(struct HW_Object* hw_device, struct hal_gps_config* config);

/**
 * Starts the GPS module
 *
 * \param hw_device A pointer to the GPS hardware module.
 *
 * \retval True if start was successful
 */
bool gps_start(struct HW_Object* hw_device);

/**
 * Stops the GPS module
 *
 * \param hw_device A pointer to the GPS hardware module.
 *
 * \retval True if stop was successful
 */
bool gps_stop(struct HW_Object* hw_device);


gps_data_t* get_gps_data();


#endif /* HAL_GPS_H_ */



/** @} */
/** @} */
