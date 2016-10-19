/**
 * \addtogroup hal
 * @{
 */

/**
 * \defgroup hal_hw_object Hardware Object General Interface
 *
 * This is the HAL API for the Hardware Object interface
 *
 * @{
 */

/**
 * \file
 * Header file for the hw_object interface of HAL
 * \author
 * Spilios Giannoulis <sgiannoulis@isi.gr>
 *
 */


#ifndef HAL_INTERFACE_H_
#define HAL_INTERFACE_H_

#include <asf.h>
#include "compiler.h"
#include "hal_usart.h"
#include "hal_ethernet.h"
#include "hal_timers.h"
#include "hal_gpio.h"
#include "hal_radio.h"
#include "hal_spi.h"
#include "hal_twi.h"
#include "hal_system_time.h"
#include "hal_sd_mmc.h"
#include "hal_adc.h"
#include "hal_gps.h"
#include "hal_wifi.h"

#define true 1
#define false 0

//! Hardware types enumeration
typedef enum hw_type{
	usart,
	spi,
	twi,
	gpio,
	ethernet,
	gps,
	wifi,
	rtc_int,
	rtc_ext,
	zbee,
	timer,
	adc,
	sd_mmc
	}hw_type;


//*************************************************//
//*********Global Structures of HW ****************//

//! HW object registry global structure
struct HW_Object {
	
	//! Hardware type 
	hw_type type;
	//! Hardware number used to identify between modules of same hardware type
	uint8_t number;
	//! Pointer to the actual hw base address if possible
	void* hw_pointer;
	//! Pointer to the struct containing all the configuration parameters of the object
	void* config_struct;
	//! Flag if object is already used
	bool used;
	//! Flag if object is started
	bool started;
}HW_Object;





//!The global HW structure containing all available HW and the corresponding configuration structure
struct hw_registry_struct
{
	//********************************************************************//
	//*********Lower HAL layer grouped as standard HW devices****************//
	
	//! USARTC0 -> COM0 (GPS usart)
	struct HW_Object COM0;
	
	//! USARTC1 -> COM1 (WIFI/XBEE usart 1)
	struct HW_Object COM1;
	
	//! USARTD0 -> COM2 used 
	struct HW_Object COM2;
	
	//! USARTD1 -> COM3 (RS232 to PC debug usart)
	struct HW_Object COM3;
	
	//! USARTE0 -> COM4 (WIFI/XBEE usart 0)
	struct HW_Object COM4;
	
	//! USARTF0 -> COM5 (Ethernet Usart)
	struct HW_Object COM5;

	//! SPI
	struct HW_Object SPI_C;
	struct HW_Object SPI_D;
	
	//!TWI
	struct HW_Object TWI_C;
	struct HW_Object TWI_E;
	
	//! Internal and External ADC
	struct HW_Object ADC_INT0;
	struct HW_Object ADC_INT1;
	struct HW_Object ADC_EXT;
	
	//! RTC32 internal and external clock
	struct HW_Object RTC_INT;
	struct HW_Object RTC_EXT;  
	
	//! CPU object
	struct HW_Object CPU;
	
	//! Table of available GPIO pins
	struct HW_Object GPIO[GPIO_COUNT];
	
//********************************************************************//
//*********Upper HAL layer grouped as external devices****************//
	
	//! Software Timers
	struct HW_Object TIMERS[TIMERS_COUNT];
	
	//! Red and Green LEDs
	struct HW_Object LED0;
	struct HW_Object LED1;
	
	//MMC card
	
	//! WIFI Module 
	struct HW_Object WIFI;
	
	//! Ethernet Module
	struct HW_Object ETHERNET0;
	
	//! GPS Module
	struct HW_Object GPS;
	
	//! Sensors Module
	struct HW_Object SEISMIC;
	struct HW_Object AMPERES;
		
	//! SD card Module
	struct HW_Object SD_MMC;
		
	}hw_registry_struct; 


//! Declaration of the Global Registry of HW objects available
extern struct hw_registry_struct global_hw_registry;


/**
 * \name HAL routines to access the available HW
 * @{
 */

//********************************************************************//
//*********HAL routines to access the available HW ****************//

/**
 * Initialize the HAL interface hardware object system
 *
 * \retval True if initialization was successful
 */
bool hal_system_init(void);

/**
 * Get a pointer to a certain hardware object
 *
 * \param type Type of the hardware object
 *
 * \param hw_number Number of the hardware object, not always usable
 *
 * \retval Pointer to the requested hardware object
 */
struct HW_Object* hal_get_hw_obj(hw_type type, uint8_t hw_number);

/**
 * Release a hardware object
 *
 * \param hw_pointer Pointer to the hardware object
 *
 * \retval True if releasing the hardware object was successful
 */
bool hal_release_hw_obj(struct HW_Object* hw_pointer);

/** @} */


/**
 * \name HAL routines to initialize and configure the available HW
 * @{
 */
/**
 * Initialize a hardware object
 *
 * \param hw_pointer Pointer to the hardware object
 *
 * \retval True if initializing the hardware object was successful
 */
bool hal_init_hw_obj(struct HW_Object* hw_pointer);

/**
 * Set the configuration parameters to the hardware object by
 * calling internally the specific hardware object set configuration routine.
 *
 * \param hw_pointer A pointer to the hardware object.
 *
 * \param config_struct A pointer to the hardware object specific struct holding all user config data
 *
 * \retval True if configuration was successful
 */
bool hal_set_config_hw_obj(struct HW_Object* hw_pointer, void* config_struct);

/**
 * Get the configuration parameters of the hardware object by
 * calling internally the specific hardware object get configuration routine.
 *
 * \param hw_pointer A pointer to the hardware object.
 *
 * \param config_struct A pointer to the hardware object specific struct to hold all object config data
 *
 * \retval True if getting configuration parameters was successful
 */
bool hal_get_config_hw_obj(struct HW_Object* hw_pointer, void* config_struct);

/** @} */

/**
 * \name HAL routines to start and stop the available HW
 * @{
 */

/**
 * Starts the  hardware object
 *
 * \param hw_pointer A pointer to the hardware object.
 *
 * \retval True if start was successful
 */
bool hal_start_hw_obj(struct HW_Object* hw_pointer);

/**
 * Stops the  hardware object
 *
 * \param hw_pointer A pointer to the hardware object.
 *
 * \retval True if stop was successful
 */
bool hal_stop_hw_obj(struct HW_Object* hw_pointer);

/** @} */

#endif /* HAL_INTERFACE_H_ */


/** @} */
/** @} */
