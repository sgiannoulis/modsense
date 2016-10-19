/**
 * \addtogroup hal
 * @{
 */

/**
 * \defgroup hal_Ethernet Ethernet Hardware Abstraction Layer
 *
 * This is the HAL API for the Ethernet hardware module
 *
 * @{
 */

/**
 * \file
 * Header file for the Ethernet HAL
 * \author
 * Spilios Giannoulis <sgiannoulis@isi.gr>
 *
 */

#ifndef HAL_ETHERNET_H_
#define HAL_ETHERNET_H_

#include <asf.h>
#include "compiler.h"
#include "hal_interface.h"

/**
 * Contains configuration parameters for the Ethernet module
 *
 */
struct hal_ethernet_config {
	
	//! Node IP Address
	uint8_t ipv4_addr[4];
	//! Node Netmask
	uint8_t ipv4_netmask[4];
	//! Gateway address
	uint8_t ipv4_gateway[4];

} hal_ethernet_config;

struct HW_Object;

/**
 * Initialize the Ethernet hardware module
 *
 * \param hw_device A pointer to the Ethernet hardware module.
 *
 * \retval True if initialization was successful
 */
bool ethernet_init( struct HW_Object * hw_device);

/**
 * Set the configuration parameters to the Ethernet module
 *
 * \param hw_device A pointer to the Ethernet hardware module.
 *
 * \param config A pointer to the Ethernet configuration struct holding all user config data
 *
 * \retval True if configuration was successful
 */
bool ethernet_set_config(struct  HW_Object* hw_device, struct hal_ethernet_config* config);

/**
 * Get the configuration parameters from the Ethernet module
 *
 * \param hw_device A pointer to the Ethernet hardware module.
 *
 * \param config A pointer to the Ethernet configuration struct to hold all Ethernet config data
 *
 * \retval True if getting configuration parameters was successful
 */
bool ethernet_get_config(struct HW_Object* hw_device, struct hal_ethernet_config* config);

/**
 * Starts the Ethernet module
 *
 * \param hw_device A pointer to the Ethernet hardware module.
 *
 * \retval True if start was successful
 */
bool ethernet_start(struct HW_Object* hw_device);

/**
 * Stops the Ethernet module
 *
 * \param hw_device A pointer to the Ethernet hardware module.
 *
 * \retval True if stop was successful
 */
bool ethernet_stop(struct HW_Object* hw_device);


/**
 * Send a byte using the Ethernet module
 *
 * \param hw_device A pointer to the Ethernet hardware module.
 *
  * \param byte_data The byte to be transmitted.
  *
 * \retval True if send was successful
 */
bool ethernet_send_byte(struct HW_Object* hw_device, uint8_t byte_data);

/**
 * Receive a byte using the Ethernet module
 *
 * \param hw_device A pointer to the Ethernet hardware module.
 *
  * \param byte_data Pointer to the store location of the received byte.
  *
 * \retval True if receive was successful
 */
bool  ethernet_receive_byte(struct HW_Object* hw_device,uint8_t* byte_data);

/**
 * Send a string using the Ethernet module
 *
 * \param hw_device A pointer to the Ethernet hardware module.
 *
 * \param string Pointer to the string to be transmitted.
 *
 * \param size Number of chars to be transmitted.
 *
 * \retval True if send was successful
 */
bool ethernet_send_string(struct HW_Object* hw_device, uint8_t* string, uint16_t size);

/**
 * Receive a string using the Ethernet module
 *
 * \param hw_device A pointer to the Ethernet hardware module.
 *
 * \param buffer Pointer to the buffer to store the data received.
 *
 * \param size Number of chars to be received.
 *
 * \retval Number of actually received bytes
 */
uint8_t ethernet_receive_string(struct HW_Object* hw_device, uint8_t* buffer, uint16_t size);


#endif /* HAL_ETHERNET_H_ */

/** @} */
/** @} */

