/**
 * \addtogroup hal
 * @{
 */

/**
 * \defgroup hal_I2C I2C Hardware Abstraction Layer
 *
 * This is the HAL API for the I2C hardware module
 *
 * @{
 */

/**
 * \file
 * Header file for the I2C HAL
 * \author
 * Spilios Giannoulis <sgiannoulis@isi.gr>
 *
 */

#ifndef HAL_I2C_H_
#define HAL_I2C_H_

/**
 * Contains configuration parameters for the I2C module
 *
 */
struct i2c_hal_config {
	
	//! Set baud rate of the i2c (unused in slave modes).
	uint32_t baudrate;

}i2c_hal_config ;

struct HW_Object;

/**
 * Initialize the I2C hardware module
 *
 * \param hw_device A pointer to the I2C hardware module.
 *
 * \retval True if initialization was successful
 */
bool i2c_init( struct HW_Object* hw_device);

/**
 * Set the configuration parameters to the I2C module
 *
 * \param hw_device A pointer to the I2C hardware module.
 *
 * \param config A pointer to the I2C configuration struct holding all user config data
 *
 * \retval True if configuration was successful
 */
bool i2c_set_config(struct HW_Object* hw_device, struct i2c_hal_config* config);

/**
 * Get the configuration parameters from the I2C module
 *
 * \param hw_device A pointer to the I2C hardware module.
 *
 * \param config A pointer to the I2C configuration struct to hold all I2C config data
 *
 * \retval True if getting configuration parameters was successful
 */
bool i2c_get_config(struct HW_Object* hw_device, struct i2c_hal_config* config);


/**
 * Starts the I2C module
 *
 * \param hw_device A pointer to the I2C hardware module.
 *
 * \retval True if start was successful
 */
bool i2c_start(struct HW_Object* hw_device);

/**
 * Stops the I2C module
 *
 * \param hw_device A pointer to the I2C hardware module.
 *
 * \retval True if stop was successful
 */
bool i2c_stop(struct HW_Object*  hw_device);


/**
 * Send a packet through I2C bus
 *
 * \param hw_device A pointer to the I2C hardware module.
 *
 * \param package A pointer to the I2C packet to be send.
 *
 * \retval True if send was successful
 */
bool i2c_send_packet(struct HW_Object*  hw_device, twi_package_t *package);


/**
 * Receive a packet through I2C bus
 *
 * \param hw_device A pointer to the I2C hardware module.
 *
 * \param package A pointer to store the I2C packet received .
 *
 * \retval Number of bytes received
 */
uint8_t i2c_receive_packet(struct HW_Object*  hw_device, twi_package_t *package);



#endif /* HAL_I2C_H_ */


/** @} */
/** @} */