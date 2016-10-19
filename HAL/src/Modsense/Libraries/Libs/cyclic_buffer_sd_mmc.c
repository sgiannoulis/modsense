/*
 * cyclic_buffer_sd_mmc.c
 *
 * Created: 26/9/2013 2:41:23 μμ
 *  Author: spilios
 */ 

#include "cyclic_buffer_sd_mmc.h"
#include "definitions.h"
#include "timeout.h"
#include "timer_10ns.h"
#include "sd_mmc.h"


#define SAVE_CONFIG_OFFSET 128lu

//#define MAX_SLOTS (SD_MAX_OFFSET- SD_START_OFFSET)/2
//#define MAX_SLOTS 1925055lu   //for some precompiler error the above wont work correctly. is loses the /2 



//PRIVATE global variables
static struct HW_Object* hw_timer_buffer = NULL;
static struct HW_Object* sd_mmc_device = NULL;
static bool buffer_inited=false;
static struct HW_Object* hw_system_time = NULL;
static bool cyclic;

static void cyclic_buffer_isr_routine(void);


struct cyclic_buffer_point {
	uint32_t write_sd_offset;
	uint32_t read_sd_offset;
};

static struct cyclic_buffer_point cyclic_buffer_pointers;


void cyclic_buffer_isr_routine(void){
	
//	uint32_t clock_time(void);
	//! Returns seconds , unix timestamp like, since 1970
//	uint32_t clock_seconds(void);
	
	//system_time = hal_get_hw_obj(rtc_ext,0);
	//PRINTF("SecInt: %lu ,",clock_seconds());
	
	uint32_t temp_time = hal_system_time_read_sec_time(hw_system_time);
	PRINTF("RTC: %lu\r\n",temp_time);
		
 	PRINTF("write %lu , ",cyclic_buffer_pointers.write_sd_offset);
 	PRINTF("read %lu \r\n",cyclic_buffer_pointers.read_sd_offset);

	 
	 
 hal_sd_mmc_write_data(sd_mmc_device,SAVE_CONFIG_OFFSET,&cyclic_buffer_pointers,2);
	
}

bool set_config_sd_buffer(struct HW_Object*  hw_device, bool is_cyclic){
	
	cyclic=is_cyclic;
	
	if(hw_device){
		sd_mmc_device=hw_device;
		return true;
	}
	
return false;	
}

bool init_sd_buffer(){
	
	if(!sd_mmc_device->started)
		 if(!hal_init_hw_obj( sd_mmc_device)){
			 PRINTF("ERROR, cant init SD MMC");
			 return false;
		 }
	
	
	if (!buffer_inited&&cyclic)
	{
		static struct hal_timer_config HW_TIMER2_OPTIONS;
		HW_TIMER2_OPTIONS.callback=cyclic_buffer_isr_routine;
		//HW_TIMER_OPTIONS.callback=NULL;
		HW_TIMER2_OPTIONS.period_timeout=60000;
		HW_TIMER2_OPTIONS.type=periodic;
	
		hw_timer_buffer = hal_get_hw_obj(timer,0); //it returns a pointer to an unused timer, so no need to the int parameter
		if(!hal_init_hw_obj( hw_timer_buffer))
			PRINTF("ERROR, cant init HW TIMER");
		hal_set_config_hw_obj( hw_timer_buffer, &HW_TIMER2_OPTIONS);
	
		//TODO , must remove after testing
		hw_system_time = hal_get_hw_obj(rtc_ext,0);
		hal_init_hw_obj(hw_system_time);
		hal_start_hw_obj(hw_system_time);
				
	}
	
	if (!buffer_inited&&!cyclic)
	{
	
	//TODO , must remove after testing
	hw_system_time = hal_get_hw_obj(rtc_ext,0);
	hal_init_hw_obj(hw_system_time);
	hal_start_hw_obj(hw_system_time);
	
	}
	
	buffer_inited=true;
	return true;
}

bool start_sd_buffer(void){
	
	//Start the sd_mcc if not started
	if(!sd_mmc_device->started)
		if(!hal_start_hw_obj(sd_mmc_device))
			return false;
		

	
	if(cyclic){
		
		//READ THE SD_READ_OFFSET AND THE SD_WRITE OFFSET...
		if(!hal_sd_mmc_read_data(sd_mmc_device,SAVE_CONFIG_OFFSET,&cyclic_buffer_pointers,2)){
			PRINTF("error reading sector 128");
			return false;
		}
				
	//	PRINTF("write %lu ,",cyclic_buffer_pointers.write_sd_offset);
	//	PRINTF(", read %lu \r\n",cyclic_buffer_pointers.read_sd_offset);
	
		//First time use of SD card chance
		if((cyclic_buffer_pointers.read_sd_offset<SD_START_OFFSET)||(cyclic_buffer_pointers.read_sd_offset>SD_MAX_OFFSET))
			cyclic_buffer_pointers.read_sd_offset=SD_START_OFFSET;
	
		if((cyclic_buffer_pointers.write_sd_offset<SD_START_OFFSET)||(cyclic_buffer_pointers.write_sd_offset>SD_MAX_OFFSET))
			cyclic_buffer_pointers.write_sd_offset=SD_START_OFFSET;
	

	//		PRINTF("Write saved pointer %lu ,",cyclic_buffer_pointers.write_sd_offset);
	//		PRINTF("read saved pointer %lu \r\n",cyclic_buffer_pointers.read_sd_offset);
		
	//	cyclic_buffer_pointers.read_sd_offset=128;
	//	cyclic_buffer_pointers.write_sd_offset=128000;
	//	PRINTF("write %d , read %d \r\n",cyclic_buffer_pointers.write_sd_offset,cyclic_buffer_pointers.read_sd_offset);
	
	
	

	
		//Start the software timer to periodically save the pointer data once per minute at the start of the minute
		if(hw_timer_buffer!=NULL){
			//synchronize with the 1st msec of a second
			while(msec_time()!=0);
	
			//add another 500 msec to be sure we will be in the middle of a second, to not mess with the ADS routines
			delay_1ms_step(500);
	
			if(!hal_start_hw_obj(hw_timer_buffer))
				return false;
		}
		
	}
	return true;
}

bool stop_sd_buffer(){
	
	if(hw_timer_buffer!=NULL)
		hal_stop_hw_obj(hw_timer_buffer);
		
	//SD mmc might be used elsewhere, should i stop here ? 
	hal_stop_hw_obj(sd_mmc_device);
	
}


bool write_sd_buffer(const void *src_data){
	
	PRINTF("W: %lu\r\n",cyclic_buffer_pointers.write_sd_offset);
	
	if(hal_sd_mmc_write_data(sd_mmc_device,cyclic_buffer_pointers.write_sd_offset,src_data,2)){
	
		cyclic_buffer_pointers.write_sd_offset += 2;
		cyclic_buffer_pointers.write_sd_offset %= SD_MAX_OFFSET;
		
		//Arrange the cyclic sd_offset pointer to SD card
		//if(sd_offset ==SD_MAX_OFFSET)
		//	sd_offset = SD_START_OFFSET;
		return true;
	}

	return false;
	
}




bool read_sd_buffer(void *buffer){
	
	//Check if read pointer catch up with write pointer, if equal no data is available
	if(cyclic_buffer_pointers.read_sd_offset==cyclic_buffer_pointers.write_sd_offset)
		return false;
	
	PRINTF("R: %lu\r\n",cyclic_buffer_pointers.read_sd_offset);
	
	//struct HW_Object* system_time_obj = hal_get_hw_obj(rtc_ext,0);	
	//PRINTF("SecInt: %lu\r\n",clock_seconds());
	//PRINTF("RTC: %lu\r\n",hal_system_time_read_sec_time(hw_system_time));
		
		
	if(hal_sd_mmc_read_data(sd_mmc_device,cyclic_buffer_pointers.read_sd_offset,buffer,2)){
	
		cyclic_buffer_pointers.read_sd_offset += 2;
		cyclic_buffer_pointers.read_sd_offset %= SD_MAX_OFFSET;
	
		//Arrange the cyclic sd_offset pointer to SD card
		//if(sd_offset ==SD_MAX_OFFSET)
		//		sd_offset = SD_START_OFFSET;
	    return true;
	}
	else
	 return false;
}

bool write_hashed_sd_buffer(const void *src_data, uint32_t index){

	uint32_t	time_start=0;
	
	uint32_t sd_index  = ( index % ((SD_MAX_OFFSET- SD_START_OFFSET)/2) )*SD_ACCESS_SIZE + SD_START_OFFSET ;
	
	if(sd_index>SD_MAX_OFFSET){
		PRINTF("ERROR, passed max offset in sd");		
		return false;
	}
	time_start=msec_time();
	
	if(hal_sd_mmc_write_data(sd_mmc_device,sd_index,src_data,2)){
		PRINTF("%u ",msec_time());
		PRINTF("W: stamp %lu\r\n",index);
		//PRINTF(" to %lu\r\n",sd_index);	
		return true;
	//Try once more to write	
/*	}else if(hal_sd_mmc_write_data(sd_mmc_device,sd_index,src_data,2)){
		PRINTF("%u ",msec_time());
		PRINTF("W: stamp %lu\r\n",index);
		//PRINTF(" to %lu\r\n",sd_index);
		return true;*/  	
	}
	
	return false;
}

bool read_hashed_sd_buffer(void *buffer, uint32_t index){

	uint32_t sd_index  = SD_START_OFFSET + (index%((SD_MAX_OFFSET- SD_START_OFFSET)/2))*SD_ACCESS_SIZE;
	/*
	PRINTF("max slots %lu\r\n", ((SD_MAX_OFFSET- SD_START_OFFSET)/2));
	PRINTF("modulo %lu\r\n", index%((SD_MAX_OFFSET- SD_START_OFFSET)/2));
	PRINTF("index %lu\r\n", (index%((SD_MAX_OFFSET- SD_START_OFFSET)/2))*2);
	PRINTF("index final %lu\r\n", (index%((SD_MAX_OFFSET- SD_START_OFFSET)/2))*2+130);
	*/
	
	
	if(sd_index>SD_MAX_OFFSET){
		PRINTF("ERROR, passed max offset in sd");
		return false;
	}
		
	if(hal_sd_mmc_read_data(sd_mmc_device,sd_index,buffer,2)){
		PRINTF("R",index);
		//PRINTF("R: stamp %lu\r\n",index);
		//PRINTF(" from %lu\r\n",sd_index);
		return true;
	}
	else
		return false;
}


uint32_t get_write_offset_sd_buffer(){
	return 	cyclic_buffer_pointers.write_sd_offset;
}


uint32_t get_read_offset_sd_buffer(){
	return 	cyclic_buffer_pointers.read_sd_offset;
}


