/**
 * \addtogroup hal
 * @{
 */

/**
 * \defgroup hal_Timer Timer Hardware Abstraction Layer
 *
 * This is the HAL API for the Timer hardware module
 *
 * @{
 */

/**
 * \file
 * Header file for the Timer HAL
 * \author
 * Spilios Giannoulis <sgiannoulis@isi.gr>
 *
 */


#ifndef HAL_TIMERS_H_
#define HAL_TIMERS_H_

#include <asf.h>
#include "compiler.h"
#include "hal_interface.h"

//! Type definition of timers type
typedef enum {periodic,single} timer_type;
	
//! Configuration parameters when initializing a Timer
struct hal_timer_config {
	
	//! Type of timer , can be SingleFired, Periodic
	timer_type type;

	//! period of timer if periodic, else it is timeout time
	uint16_t period_timeout;

	//! callback function to be called when the timer expires
	callback_fun callback;

} hal_timer_config;


/**
 * Initialize the Timer
 *
 * \param hw_device A pointer to the Timer.
 *
 * \retval True if initialization was successful
 */
bool hal_timer_init(struct HW_Object*  hw_device);

/**
 * Set the configuration parameters to the Timer
 *
 * \param hw_device A pointer to the Timer.
 *
 * \param config A pointer to the Timer configuration struct holding all user config data.
 *
 * \retval True if configuration was successful
 */
bool hal_timer_set_config(struct HW_Object*  hw_device, struct hal_timer_config* config);

/**
 * Get the configuration parameters from the Timer module
 *
 * \param hw_device A pointer to the Timer hardware module.
 *
 * \param config A pointer to the Timer configuration struct to hold all Timer config data
 *
 * \retval True if getting configuration parameters was successful
 */
bool hal_timer_get_config(struct HW_Object*  hw_device, struct hal_timer_config* config);

/**
 * Starts the Timer module
 *
 * \param hw_device A pointer to the Timer hardware module.
 *
 * \retval True if start was successful
 */
bool hal_timer_start(struct HW_Object*  hw_device);

/**
 * Stops the Timer module
 *
 * \param hw_device A pointer to the Timer hardware module.
 *
 * \retval True if stop was successful
 */
bool hal_timer_stop(struct HW_Object*  hw_device);


/**
 * Restarts the Timer pointed by the parameter
 *
 * \param hw_device A pointer to the Timer to be restarted.
 *
 * \retval True if restart was successful
 */
bool hal_timer_restart(struct HW_Object*  hw_device);

/**
 * Checks if the Timer has expired
 *
 * \param hw_device A pointer to the Timer to be checked.
 *
 * \retval True if Timer has expired
 */
bool hal_timer_expired(struct HW_Object*  hw_device);



#endif /* HAL_TIMERS_H_ */

/** @} */
/** @} */