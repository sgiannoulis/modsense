/**
 * \addtogroup hal
 * @{
 */

/**
 * \defgroup hal_spi SPI Hardware Abstraction Layer
 *
 * This is the HAL API for the SPI hardware module
 *
 * @{
 */

/**
 * \file
 * Header file for the SPI HAL
 * \author
 * Spilios Giannoulis <sgiannoulis@isi.gr>
 *
 */ 


#ifndef HAL_hal_spi_H_
#define HAL_hal_spi_H_


#include <asf.h>
#include "compiler.h"
#include "hal_interface.h"



//! Input parameters when initializing SPI and similar modes.
struct hal_spi_config {

	//! This is actually the chip select port pin number
	//	uint8_t spi_slave_id;
	
	//! Mode of operation of the spi bus , can be SPI_MODE_0 - SPI_MODE_3
	uint8_t spi_mode;
	
	//! Set baud rate of the SPI, usually between 400.000 and 12.000.000 
	uint32_t baudrate;

}hal_spi_config ;

struct HW_Object;

/**
 * Initialize the SPI hardware module
 *
 * \param hw_device A pointer to the SPI hardware module.
 *
 * \retval True if initialization was successful
 */
bool hal_spi_init( struct HW_Object* hw_device);

/**
 * Set the configuration parameters to the SPI module
 *
 * \param hw_device A pointer to the SPI hardware module.
 *
 * \param config A pointer to the SPI configuration struct holding all user config data
 *
 * \retval True if configuration was successful
 */
bool hal_spi_set_config(struct HW_Object* hw_device, struct hal_spi_config* config);

/**
 * Get the configuration parameters from the SPI module
 *
 * \param hw_device A pointer to the SPI hardware module.
 *
 * \param config A pointer to the SPI configuration struct to hold all SPI config data
 *
 * \retval True if getting configuration parameters was successful
 */
bool hal_spi_get_config(struct HW_Object* hw_device, struct hal_spi_config* config);

/**
 * Starts the SPI module
 *
 * \param hw_device A pointer to the SPI hardware module.
 *
 * \retval True if start was successful
 */
bool hal_spi_start(struct HW_Object* hw_device);

/**
 * Stops the SPI module
 *
 * \param hw_device A pointer to the SPI hardware module.
 *
 * \retval True if stop was successful
 */
bool hal_spi_stop(struct HW_Object*  hw_device);

/**
 * Selects the spi device to use
 *
 * \param hw_device A pointer to the SPI hardware module.
 *
 * \param spi_slave_id This is actually the chip select port pin number
 *
 * \retval True if select was successful
 */
bool hal_spi_device_select(struct HW_Object* hw_device,uint8_t spi_slave_id);

/**
 * Deselects the spi device
 *
 * \param hw_device A pointer to the SPI hardware module.
 *
 * \param spi_slave_id The chip select port pin number
 *
 * \retval True if deselect was successful
 */
bool hal_spi_device_deselect(struct HW_Object* hw_device, uint8_t spi_slave_id);


/**
 * Send a byte using the SPI module
 *
 * \param hw_device A pointer to the SPI hardware module.
 *
  * \param byte_data The byte to be transmitted.
  *
 * \retval True if send was successful
 */
bool hal_spi_send_byte(struct HW_Object*  hw_device, uint8_t byte_data);

/**
 * Receive a byte using the SPI module
 *
 * \param hw_device A pointer to the SPI hardware module.
 *
  * \param byte_data Pointer to the store location of the received byte.
  *
 * \retval True if receive was successful
 */
bool hal_spi_receive_byte(struct HW_Object*  hw_device, uint8_t* byte_data);


/**
 * Send a string using the SPI module
 *
 * \param hw_device A pointer to the SPI hardware module.
 *
 * \param string Pointer to the string to be transmitted.
 *
 * \param size Number of chars to be transmitted.
 *
 * \retval True if send was successful
 */
bool hal_spi_send_string(struct HW_Object*  hw_device, uint8_t* string, uint8_t size);

/**
 * Receive a string using the SPI module
 *
 * \param hw_device A pointer to the SPI hardware module.
 *
 * \param buffer Pointer to the buffer to store the data received.
 *
 * \param size Number of chars to be received.
 *
 * \retval Number of actually received bytes
 */
uint8_t hal_spi_receive_string(struct HW_Object*  hw_device, uint8_t* buffer, uint8_t size);



#endif /* HAL_hal_spi_H_ */

/** @} */
/** @} */
