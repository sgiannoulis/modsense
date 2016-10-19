/**
 * \addtogroup hal
 * @{
 */

/**
 * \defgroup hal_Usart Usart Hardware Abstraction Layer
 *
 * This is the HAL API for the Usart hardware module
 *
 * @{
 */

/**
 * \file
 * Header file for the Usart HAL
 * \author
 * Spilios Giannoulis <sgiannoulis@isi.gr>
 *
 */

#ifndef HAL_USART_H_
#define HAL_USART_H_

#include <asf.h>
#include "compiler.h"
#include "hal_interface.h"

//extern struct hw_registry_struct global_hw_registry;

//! Input parameters when initializing RS232 and similar modes.
struct usart_hal_config {
	//! Set baud rate of the USART (unused in slave modes).
	uint32_t baudrate;

	//! Number of bits to transmit as a character (5 to 9).
	USART_CHSIZE_t charlength;

	//! Parity type: USART_PMODE_DISABLED_gc, USART_PMODE_EVEN_gc,
	//! USART_PMODE_ODD_gc.
	USART_PMODE_t paritytype;

	//! Number of stop bits between two characters:
	//! true: 2 stop bits
	//! false: 1 stop bit
	bool stopbits;
	
	//! Com Buffer data structure pointer
	USART_data_t com_data;

}usart_hal_config ;

struct HW_Object;

/**
 * Initialize the Usart hardware module
 *
 * \param hw_device A pointer to the Usart.
 *
 * \retval True if initialization was successful
 */
bool hal_usart_init( struct HW_Object* hw_device);

/**
 * Set the configuration parameters to the Usart module
 *
 * \param hw_device A pointer to the Usart hardware module.
 *
 * \param config A pointer to the Usart configuration struct holding all user config data
 *
 * \retval True if configuration was successful
 */
bool hal_usart_set_config(struct HW_Object* hw_device, usart_config* config);

/**
 * Get the configuration parameters from the Usart module
 *
 * \param hw_device A pointer to the Usart hardware module.
 *
 * \param config A pointer to the Usart configuration struct to hold all Usart config data
 *
 * \retval True if getting configuration parameters was successful
 */
bool hal_usart_get_config(struct HW_Object* hw_device, usart_config* config);

/**
 * Starts the Usart module
 *
 * \param hw_device A pointer to the Usart hardware module.
 *
 * \retval True if start was successful
 */
bool hal_usart_start(struct HW_Object* hw_device);

/**
 * Stops the Usart module
 *
 * \param hw_device A pointer to the Usart hardware module.
 *
 * \retval True if stop was successful
 */
bool hal_usart_stop(struct HW_Object*  hw_device);

/**
 * Send a byte using the Usart module
 *
 * \param hw_device A pointer to the Usart hardware module.
 *
  * \param byte_data The byte to be transmitted.
  *
 * \retval True if send was successful
 */
bool hal_usart_send_byte(struct HW_Object*  hw_device, uint8_t byte_data);

/**
 * Receive a byte using the Usart module
 *
 * \param hw_device A pointer to the Usart hardware module.
 *
  * \param byte_data Pointer to the store location of the received byte.
  *
 * \retval True if receive was successful
 */
bool hal_usart_receive_byte(struct HW_Object*  hw_device, uint8_t* byte_data);

/**
 * Send a string using the Usart module
 *
 * \param hw_device A pointer to the Usart hardware module.
 *
 * \param string Pointer to the string to be transmitted.
 *
 * \param size Number of chars to be transmitted.
 *
 * \retval True if send was successful
 */
bool hal_usart_send_string(struct HW_Object*  hw_device, uint8_t* string, uint16_t size);

/**
 * Receive a string using the Usart module
 *
 * \param hw_device A pointer to the Usart hardware module.
 *
 * \param buffer Pointer to the buffer to store the data received.
 *
 * \param size Number of chars to be received.
 *
 * \retval Number of actually received bytes
 */
uint8_t hal_usart_receive_string(struct HW_Object*  hw_device, uint8_t* buffer, uint16_t size);



#endif /* HAL_USART_H_ */


/** @} */
/** @} */