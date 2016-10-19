/**
 * \addtogroup hal
 * @{
 */

/**
 * \defgroup hal_twi System Time HAL Layer
 *
 * This is the HAL API for the external RTC32 module
 *
 * @{
 */

/**
 * \file
 * Header file for the external RTC HAL
 * \author
 * Spilios Giannoulis <sgiannoulis@isi.gr>
 *
 */


#ifndef HAL_SYSTEM_TIME_H_
#define HAL_SYSTEM_TIME_H_

#include <asf.h>
#include "compiler.h"
#include "hal_interface.h"

//! Structure that can be used to read the whole system time information, based on RTC reg types, read datasheet for info
typedef struct rtc_data{
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	uint8_t day;
	uint8_t date;
	uint8_t month_century;
	uint8_t year;

} rtc_time_t;

//extern uint32_t system_global_time;


/**
 * Initialize the External RTC hardware module
 *
 * \param hw_device A pointer to the External RTC hardware module.
 *
 * \retval True if initialization was successful
 */
bool hal_system_time_init( struct HW_Object* hw_device);


/**
 * Starts the External RTC module
 *
 * \param hw_device A pointer to the External RTC hardware module.
 *
 * \retval True if start was successful
 */
bool hal_system_time_start(struct HW_Object* hw_device);

/**
 * Stops the External RTC module
 *
 * \param hw_device A pointer to the External RTC hardware module.
 *
 * \retval True if stop was successful
 */
bool hal_system_time_stop(struct HW_Object*  hw_device);



/**
 * Returns the system time in linux timestamp
 *
 * \param hw_device A pointer to the time hardware module.
 *
 * \retval System time in seconds since 1/1/1970 
 */
uint32_t hal_system_time_read_sec_time(struct HW_Object*  hw_device);

/**
 * Returns the system time in msecs
 *
 * \param hw_device A pointer to the time hardware module.
 *
 * \retval System time in msec since 1/1/1970 with overflow 
 */
uint32_t hal_system_time_read_msec_time(struct HW_Object*  hw_device);

/**
 * Receive a string using the External RTC module, ONLY for master mode
 *
 * \param hw_device A pointer to the External RTC hardware module.
 *
 * \param packet Pointer to the packet to store the data received.
 *
 * \retval True if operation was successful 
 */
bool hal_system_time_write_time(struct HW_Object*  hw_device, uint32_t timestamp);


/**
 * Gets the GPS time from the GPS module to synchronize the system time
 *
 * \param hw_device A pointer to the External RTC hardware module.
 *
 * \param packet Pointer to the packet to store the data received.
 *
 * \retval True if operation was successful 
 */
bool hal_system_time_syncronize(struct HW_Object*  hw_device);


/**
 * Transforms the RTC32 format to the calendar library format 
 *
 * \param input A pointer to the RTC32 formatted input data.
 *
 * \param output Pointer to the calendar struct format to store the transformed data.
 *
 * \retval True if transformation was successful 
 */
bool rtc32_to_calendar_form(rtc_time_t* input,calendar_date* output);

/**
 * Transforms the calendar format to the RTC32 library format 
 *
 * \param input A pointer to the calendar formatted input data.
 *
 * \param output Pointer to the RTC32 struct format to store the transformed data.
 *
 *
 * \retval True if transformation was successful 
 */
bool calendar_to_rtc32_form(calendar_date* input,rtc_time_t* output);

#endif /* HAL_SYSTEM_TIME_H_ */