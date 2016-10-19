/**
 * \addtogroup hal
 * @{
 */

/**
 * \defgroup hal_Wifi Wifly Hardware Abstraction Layer
 *
 * This is the HAL API for the Wifly hardware module
 *
 * @{
 */

/**
 * \file
 * Header file for the Wifly HAL
 * \author
 * Spilios Giannoulis <sgiannoulis@isi.gr>
 *
 */

#ifndef HAL_WIFI_H_
#define HAL_WIFI_H_


#include <asf.h>
#include "compiler.h"
#include "hal_interface.h"

/**
 * Contains configuration parameters for the Wifi module
 *
 */
struct hal_wifi_config {
	
	//! Node IP Address
	uint8_t ipv4_addr[4];
	//! Node Netmask
	uint8_t ipv4_netmask[4];
	//! Gateway address
	uint8_t ipv4_gateway[4];
	//! WLAN password
	uint8_t passphrase[16];

} hal_wifi_config;

struct HW_Object;

/**
 * Initialize the Wifi hardware module
 *
 * \param hw_device A pointer to the Wifi hardware module.
 *
 * \retval True if initialization was successful
 */
bool hal_wifi_init( struct HW_Object * hw_device);

/**
 * Set the configuration parameters to the Wifi module
 *
 * \param hw_device A pointer to the Wifi hardware module.
 *
 * \param config A pointer to the Wifi configuration struct holding all user config data
 *
 * \retval True if configuration was successful
 */
bool hal_wifi_set_config(struct  HW_Object* hw_device, struct hal_wifi_config* config);

/**
 * Get the configuration parameters from the Wifi module
 *
 * \param hw_device A pointer to the Wifi hardware module.
 *
 * \param config A pointer to the Wifi configuration struct to hold all Wifi config data
 *
 * \retval True if getting configuration parameters was successful
 */
bool hal_wifi_get_config(struct HW_Object* hw_device, struct hal_wifi_config* config);

/**
 * Starts the Wifi module
 *
 * \param hw_device A pointer to the Wifi hardware module.
 *
 * \retval True if start was successful
 */
bool hal_wifi_start(struct HW_Object* hw_device);

/**
 * Stops the Wifi module
 *
 * \param hw_device A pointer to the Wifi hardware module.
 *
 * \retval True if stop was successful
 */
bool hal_wifi_stop(struct HW_Object* hw_device);


/**
 * Send a byte using the Wifi module
 *
 * \param hw_device A pointer to the Wifi hardware module.
 *
  * \param byte_data The byte to be transmitted.
  *
 * \retval True if send was successful
 */
bool hal_wifi_send_byte(struct HW_Object* hw_device, uint8_t byte_data);

/**
 * Receive a byte using the Wifi module
 *
 * \param hw_device A pointer to the Wifi hardware module.
 *
  * \param byte_data Pointer to the store location of the received byte.
  *
 * \retval True if receive was successful
 */
bool  hal_wifi_receive_byte(struct HW_Object* hw_device,uint8_t* byte_data);

/**
 * Send a string using the Wifi module
 *
 * \param hw_device A pointer to the Wifi hardware module.
 *
 * \param string Pointer to the string to be transmitted.
 *
 * \param size Number of chars to be transmitted.
 *
 * \retval True if send was successful
 */
bool hal_wifi_send_string(struct HW_Object* hw_device, uint8_t* string, uint16_t size);

/**
 * Receive a string using the Wifi module
 *
 * \param hw_device A pointer to the Wifi hardware module.
 *
 * \param buffer Pointer to the buffer to store the data received.
 *
 * \param size Number of chars to be received.
 *
 * \retval Number of actually received bytes
 */
uint8_t hal_wifi_receive_string(struct HW_Object* hw_device, uint8_t* buffer, uint16_t size);


#endif /* HAL_WIFI_H_ */

/** @} */
/** @} */
