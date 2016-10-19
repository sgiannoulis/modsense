/** \addtogroup lib
    @{ */
/**
 * \defgroup buffer Cyclic Buffer on SD MMC 
 *
 * Cyclic Buffer to assist on storing and managing data from the seismograph
 *
 * @{
 */


#ifndef CYCLIC_BUFFER_SD_MMC_H_
#define CYCLIC_BUFFER_SD_MMC_H_


#include <asf.h>
#include "compiler.h"
#include "hal_interface.h"
#include "sd_mmc.h"

#define SD_MAX_OFFSET (1024lu*1024lu*1880lu)/SD_MMC_BLOCK_SIZE
#define SD_START_OFFSET 130lu
#define MAX_SECS_SDCARD (SD_MAX_OFFSET- SD_START_OFFSET)/2


bool set_config_sd_buffer(struct HW_Object*  hw_device, bool is_cyclic);
bool init_sd_buffer();


bool start_sd_buffer(void);
bool stop_sd_buffer(void);



bool write_sd_buffer(const void *src_data);
bool read_sd_buffer(void *buffer);

uint32_t get_write_offset_sd_buffer(void);
uint32_t get_read_offset_sd_buffer(void);

/**********************************************/
/*Secondary option to use, hash indexed buffer */

bool write_hashed_sd_buffer(const void *src_data, uint32_t index);
bool read_hashed_sd_buffer(void *buffer, uint32_t index);  


#endif /* CYCLIC_BUFFER_SD_MMC_H_ */

/** @} */
/** @} */