/**
 * \addtogroup hal_mmc 
 * @{
 */

/**
 * \file
 * Header file for the ADC HAL
 * \author
 * Spilios Giannoulis <sgiannoulis@isi.gr>
 *
 */

#include "hal_sd_mmc.h"
#include "sd_mmc.h"
#include "led.h"


uint32_t max_sd_sector;

/**
 * \brief Display basic information of the card.
 * \note This function should be called only after the card has been
 *       initialized successfully.
 *
 * \param slot   SD/MMC slot to test
 */
void print_debug_sd_info(struct HW_Object*  hw_device)
{
	uint8_t slot = hw_device->number;
	
	printf("Card information:\n\r");
	printf("    ");
	switch (sd_mmc_get_type(slot)) {
	case CARD_TYPE_SD | CARD_TYPE_HC:
		printf("SDHC");
		break;
	case CARD_TYPE_SD:
		printf("SD");
		break;
	case CARD_TYPE_MMC | CARD_TYPE_HC:
		printf("MMC High Density");
		break;
	case CARD_TYPE_MMC:
		printf("MMC");
		break;
	case CARD_TYPE_SDIO:
		printf("SDIO\n\r");
		return;
	case CARD_TYPE_SD_COMBO:
		printf("SD COMBO");
		break;
	case CARD_TYPE_UNKNOWN:
	default:
		printf("Unknown\n\r");
		return;
	}
	printf("\n\r    %d MB\n\r", (uint16_t)(sd_mmc_get_capacity(slot)/1024));
}


bool hal_sd_mmc_init( struct HW_Object* hw_device){
	
	sd_mmc_err_t err;
	uint8_t slot = hw_device->number;
	
	// Initialize SD MMC stack
	sd_mmc_init();
		
	//Just for making the com2 look as taken for the HAL interface so no other can use it while SD hal is active
	hal_get_hw_obj(usart,2);
	
		printf("Please plug an SD, MMC or SDIO card in slot %d.\n\r", slot+1);

		// Wait for a card and ready
		do {
			err = sd_mmc_check(slot);
			if ((SD_MMC_ERR_NO_CARD != err)
			&& (SD_MMC_INIT_ONGOING != err)
			&& (SD_MMC_OK != err)) {
				printf("Card install FAILED\n\r");
				printf("Please unplug and re-plug the card.\n\r");
				LED_On(LED_RED);
				while (SD_MMC_ERR_NO_CARD != sd_mmc_check(slot)) {
				}
			}
		} while (SD_MMC_OK != err);
		
		LED_Off(LED_RED);
		// Display basic card information
		
		print_debug_sd_info(hw_device);
		
		max_sd_sector = sd_mmc_get_capacity(slot) *	(1024 / SD_MMC_BLOCK_SIZE);
		//max_sd_sector =  (sd_mmc_get_capacity(slot))<<1;
		
		if (sd_mmc_get_type(slot) & (CARD_TYPE_SD | CARD_TYPE_MMC))
			return true;
}

bool hal_sd_mmc_set_config(struct HW_Object* hw_device, struct hal_sd_mmc_config* config){
	
	hw_device->config_struct = config;
	
	return true;
}



bool hal_sd_mmc_get_config(struct HW_Object* hw_device, struct hal_sd_mmc_config* config){
	
	struct hal_sd_mmc_config* hw_config = (struct hal_sd_mmc_config*)hw_device->config_struct;
	
	config->sectors_access_size = hw_config->sectors_access_size;
	
	return true;
}


bool hal_sd_mmc_start(struct HW_Object* hw_device) {
	hw_device->started=true;
	return true;
}


bool hal_sd_mmc_stop(struct HW_Object*  hw_device) {
	hw_device->started=false;
	return true;
}


bool hal_sd_mmc_write_data(struct HW_Object*  hw_device, uint32_t sd_offset,const void *src_data , uint16_t nb_block ){
	
	if((hw_device->started==false)||hal_sd_mmc_busy(hw_device))
		return false;
		
		if (SD_MMC_OK != sd_mmc_init_write_blocks(hw_device->number,sd_offset, nb_block)) {
			printf("[FAIL3]\n\r");
			return false;
		}
		
		for (uint16_t nb_trans = 0; nb_trans < (nb_block/SD_ACCESS_SIZE); nb_trans++) {
			if (SD_MMC_OK != sd_mmc_start_write_blocks(src_data, SD_ACCESS_SIZE )) {
				printf("[FAIL4]\n\r");
				return false;
			}
			if (SD_MMC_OK != sd_mmc_wait_end_of_write_blocks()) {
				printf("[FAIL5]\n\r");
				return false;
			}
		}
		return true;
}


bool hal_sd_mmc_read_data(struct HW_Object*  hw_device, uint32_t sd_offset,const void *buffer , uint16_t nb_block ) {
	
	if((hw_device->started==false)||hal_sd_mmc_busy(hw_device))
		return false;
	
	if (SD_MMC_OK != sd_mmc_init_read_blocks(hw_device->number,	sd_offset, nb_block)) {
		printf("[FAIL1]\n\r");
		return false;
	}
	
	for (uint16_t nb_trans = 0; nb_trans < (nb_block/SD_ACCESS_SIZE); nb_trans++) {
		if (SD_MMC_OK != sd_mmc_start_read_blocks(buffer, SD_ACCESS_SIZE )) {
			printf("[FAIL2]\n\r");
			return false;
		}
		if (SD_MMC_OK != sd_mmc_wait_end_of_read_blocks()) {
			printf("[FAIL3]\n\r");
			return false;
		}
	}
	return true;	
}


bool hal_sd_mmc_busy(struct HW_Object*  hw_device) {
	
	return !sd_mmc_free(hw_device->number);
}

/** @} */

