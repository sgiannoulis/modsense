/**
 * \addtogroup hal
 * @{
 */

/**
 * \defgroup hal_mmc Multimedia SD card Hardware Abstraction Layer
 *
 * This is the HAL API for the SD MMC hardware module
 *
 * @{
 */

/**
 * \file
 * Header file for the SD MMC HAL
 * \author
 * Spilios Giannoulis <sgiannoulis@isi.gr>
 *
 */ 


#ifndef HAL_SD_MMC_H_
#define HAL_SD_MMC_H_

#include <asf.h>
#include "compiler.h"
#include "hal_interface.h"

//! How many sectors can be accessed at once
#define SD_ACCESS_SIZE 2

//! Input parameters when initializing SD MMC and similar modes.
struct hal_sd_mmc_config {

	//! How many sectors per write/read operation will be used
	uint8_t sectors_access_size;

}hal_sd_mmc_config ;

struct HW_Object;

/**
 * Initialize the SD MMC hardware module
 *
 * \param hw_device A pointer to the SD MMC hardware module.
 *
 * \retval True if initialization was successful
 */
bool hal_sd_mmc_init( struct HW_Object* hw_device);

/**
 * Set the configuration parameters to the SD MMC module
 *
 * \param hw_device A pointer to the SD MMC hardware module.
 *
 * \param config A pointer to the SD MMC configuration struct holding all user config data
 *
 * \retval True if configuration was successful
 */
bool hal_sd_mmc_set_config(struct HW_Object* hw_device, struct hal_sd_mmc_config* config);

/**
 * Get the configuration parameters from the SD MMC module
 *
 * \param hw_device A pointer to the SD MMC hardware module.
 *
 * \param config A pointer to the SD MMC configuration struct to hold all SD MMC config data
 *
 * \retval True if getting configuration parameters was successful
 */
bool hal_sd_mmc_get_config(struct HW_Object* hw_device, struct hal_sd_mmc_config* config);

/**
 * Starts the SD MMC module
 *
 * \param hw_device A pointer to the SD MMC hardware module.
 *
 * \retval True if start was successful
 */
bool hal_sd_mmc_start(struct HW_Object* hw_device);

/**
 * Stops the SD MMC module
 *
 * \param hw_device A pointer to the SD MMC hardware module.
 *
 * \retval True if stop was successful
 */
bool hal_sd_mmc_stop(struct HW_Object*  hw_device);


/**
 * Write data to the SD MMC card
 *
 * \param hw_device A pointer to the SD MMC hardware module.
 *
 * \param sd_offset Starting sector number of the SD MMC card to write at.
 *
 * \param src_data Pointer to data that will be written to SD Card.
 *
 * \param nb_block Number of total sectors to be written.
 *
 * \retval True if writing procedure ended successfully.
 */
bool hal_sd_mmc_write_data(struct HW_Object*  hw_device, uint32_t sd_offset,const void *src_data , uint16_t nb_block );

/**
 * Write data to the SD MMC card
 *
 * \param hw_device A pointer to the SD MMC hardware module.
 *
 * \param sd_offset Starting sector number of the SD MMC card to read from.
 *
 * \param buffer Pointer to buffer that will hold the read data.
 *
 * \param nb_block Number of total sectors to be read.
 *
 * \retval True if reading procedure ended successfully.
 */
bool hal_sd_mmc_read_data(struct HW_Object*  hw_device, uint32_t sd_offset,const void *buffer , uint16_t nb_block );


/**
 * Checks if SD card is busy
 *
 * \param hw_device A pointer to the SD MMC hardware module.
 *
 * \retval True if device is busy.
 */
bool hal_sd_mmc_busy(struct HW_Object*  hw_device);

/**
 * Prints to standard IO the information of the SD card
 *
 * \param hw_device A pointer to the SD MMC hardware module.
 */
void print_debug_sd_info(struct HW_Object*  hw_device);

#endif /* HAL_SD_MMC_H_ */

/** @} */
/** @} */


