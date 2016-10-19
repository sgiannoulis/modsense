/**
 * \addtogroup hal
 * @{
 */

/**
 * \defgroup hal_GPIO GPIO Hardware Abstraction Layer
 *
 * This is the HAL API for the GPIO hardware module
 *
 * @{
 */

/**
 * \file
 * Header file for the GPIO HAL
 * \author
 * Spilios Giannoulis <sgiannoulis@isi.gr>
 *
 */


#ifndef HAL_GPIO_H_
#define HAL_GPIO_H_

#define GPIO_COUNT 16

#include <asf.h>
#include "compiler.h"
#include "hal_interface.h"

typedef enum PORT_INTLVL_enum
{
	PORT_INTLVL_OFF_gc = (0x00<<0),  /* Interrupt Disabled */
	PORT_INTLVL_LO_gc = (0x01<<0),  /* Low Level */
	PORT_INTLVL_MED_gc = (0x02<<0),  /* Medium Level */
	PORT_INTLVL_HI_gc = (0x03<<0),  /* High Level */
} PORT_INTLVL_t;

/**
 * Contains configuration parameters for the GPIO module
 *
 */
struct gpio_hal_config {
	
	//! Port Number
//	uint8_t port;

	//! Pin Number
//	uint8_t pin_number;

	//! Port pin flags
	port_pin_flags_t flags;
	
	//!interrupt level, set to 0 to disable interrupts
	PORT_INTLVL_t interrupt_level;
	
	//!Interrupt team selection, only 2 teams supported, team 0 or 1 per GPIO port.
	bool use_int_team0;
};

/**
 * Initialize the GPIO hardware module
 *
 * \param hw_device A pointer to the GPIO hardware module.
 *
 * \retval True if initialization was successful
 */
bool hal_gpio_init( struct HW_Object * hw_device);

/**
 * Set the configuration parameters to the GPIO module
 *
 * \param hw_device A pointer to the GPIO hardware module.
 *
 * \param config A pointer to the GPIO configuration struct holding all user config data
 *
 * \retval True if configuration was successful
 */
bool hal_gpio_set_config(struct HW_Object* hw_device, struct gpio_hal_config* config);

/**
 * Get the configuration parameters from the GPIO module
 *
 * \param hw_device A pointer to the GPIO hardware module.
 *
 * \param config A pointer to the GPIO configuration struct to hold all GPIO config data
 *
 * \retval True if getting configuration parameters was successful
 */
bool hal_gpio_get_config(struct HW_Object* hw_device, struct gpio_hal_config* config);


/**
 * Starts the GPIO module
 *
 * \param hw_device A pointer to the GPIO hardware module.
 *
 * \retval True if start was successful
 */
bool hal_gpio_start(struct HW_Object* hw_device);

/**
 * Stops the GPIO module
 *
 * \param hw_device A pointer to the GPIO hardware module.
 *
 * \retval True if stop was successful
 */
bool hal_gpio_stop(struct HW_Object* hw_device);



/**
 * Toggle the GPIO pin logical status
 *
 * \param hw_device A pointer to the GPIO hardware module.
 *
 * \retval True if configuration was successful
 */
bool hal_gpio_toggle(struct HW_Object* hw_device);

/**
 * Set the GPIO pin logical status to high
 *
 * \param hw_device A pointer to the GPIO hardware module.
 *
 * \retval True if configuration was successful
 */
bool hal_gpio_high(struct HW_Object* hw_device);

/**
 * Set the GPIO pin logical status to low
 *
 * \param hw_device A pointer to the GPIO hardware module.
 *
 * \retval True if configuration was successful
 */
bool hal_gpio_low(struct HW_Object* hw_device);

/**
 * Check if the GPIO pin logical status is low
 *
 * \param hw_device A pointer to the GPIO hardware module.
 *
 * \retval True if GPIO pin logical status is low
 */
bool hal_gpio_is_low(struct HW_Object* hw_device);

/**
 * Check if the GPIO pin logical status is high
 *
 * \param hw_device A pointer to the GPIO hardware module.
 *
 * \retval True if GPIO pin logical status is high
 */
bool hal_gpio_is_high(struct HW_Object* hw_device);

#endif /* HAL_GPIO_H_ */


/** @} */
/** @} */