/**
 * \addtogroup hal
 * @{
 */

/**
 * \defgroup hal_Radio Radio Hardware Abstraction Layer
 *
 * This is the HAL API for the Radio hardware module
 *
 * @{
 */

/**
 * \file
 * Header file for the Radio HAL
 * \author
 * Spilios Giannoulis <sgiannoulis@isi.gr>
 *
 */


#ifndef HAL_RADIO_H_
#define HAL_RADIO_H_

#include <asf.h>
#include "compiler.h"
#include "hal_interface.h"

/**
 * Contains configuration parameters for the Radio module
 *
 */
typedef struct radio_config{
	
	//! Node IP Address
	uint8_t ipv4_addr[4];
		
	//! Node Netmask
	uint8_t ipv4_netmask[4];
	
	//! Gateway address
	uint8_t ipv4_gateway[4];
		
}radio_config;

/**
 * Initialize the Radio hardware module
 *
 * \param hw_device A pointer to the Radio hardware module.
 *
 * \retval True if initialization was successful
 */
bool radio_init(struct HW_Object* hw_device);

/**
 * Set the configuration parameters to the Radio module
 *
 * \param hw_device A pointer to the Radio hardware module.
 *
 * \param config A pointer to the Radio configuration struct holding all user config data
 *
 * \retval True if configuration was successful
 */
bool radio_set_config(struct HW_Object* hw_device, radio_config* config);

/**
 * Get the configuration parameters from the Radio module
 *
 * \param hw_device A pointer to the Radio hardware module.
 *
 * \param config A pointer to the Radio configuration struct to hold all Radio config data
 *
 * \retval True if getting configuration parameters was successful
 */
bool radio_get_config(struct HW_Object* hw_device, radio_config* config);


/**
 * Starts the Radio module
 *
 * \param hw_device A pointer to the Radio hardware module.
 *
 * \retval True if start was successful
 */
bool radio_start(struct HW_Object* hw_device);

/**
 * Stops the Radio module
 *
 * \param hw_device A pointer to the Radio hardware module.
 *
 * \retval True if stop was successful
 */
bool radio_stop(struct HW_Object* hw_device);

/**
 * Sends a radio packet 
 *
 * \param hw_device A pointer to the Radio hardware module.
 *
 * \param pkt A pointer to the pkt to be send.
 *
 * \retval True if stop was successful
 */
//bool radio_send_packet(struct HW_Object *hw_device, xbee_pkt* pkt);

/**
 * Receives a radio packet 
 *
 * \param hw_device A pointer to the Radio hardware module.
 *
 * \retval A pointer to the pkt received.
 */
//xbee_pkt* radio_receive_packet(struct HW_Object *hw_device);


#endif /* HAL_RADIO_H_ */

/** @} */
/** @} */
