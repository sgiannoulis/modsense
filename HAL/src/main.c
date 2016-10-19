
#include <conf_usart_example.h>
#include <asf.h>
#include <conf_spi_master.h>
#include "./asf/xmega/drivers/wifi/wifi.h"
#include "led.h"
#include "timer_10ns.h"
#include "ads1254.h"
#include <string.h>

#include "hal_interface.h"

#include "slip.h"
#include "socat.h"
#include "uip.h"
#include "etimer.h"
#include "autostart.h"
#include "random.h"
#include "definitions.h"
#include "er-example-server.h"
#include "cyclic_buffer_sd_mmc.h"
#include "sd_mmc.h"


void test_usarts( void );
void test_timers( void );
void test_gpio_hal(void);
void test_slip(void);
void test_eth_socat(void);
void test_uip_slip_processes(void);
void test_ads1254(void);
void test_rtc32_and_twi_ext(void);
int test_sd_mmc_software_spi(void);
void test_int_adc(void);
void test_gps(void);
void test_wifly(void);


//actual service for the modsense project 
bool ads1254_save_to_sdmmc_driver_init(void);
static void ads1254_isr_routine(void);

void power_adc_measurements_driver_init(void);
static void power_adc_handler(ADC_t *adc, uint8_t ch_mask, adc_result_t result);
static void power_isr_routine(void);
static void gps_isr_routine(void);




void test(void);
void test1(void);
void test2(void);
void test3(void);


/*! \brief Main function.
 */

struct HW_Object* HW_ETHERNET;
struct HW_Object* HW_WIFLY;

struct HW_Object* HW_TIMER2;
struct HW_Object* HW_COM3 ;
struct HW_Object* HW_GPIO ;

bool send_panos; 
#define SAMPLES_PER_PACKET 100



typedef struct  {
	char id[4];                                 // "HDA\0"
	char station_name[5];                 // "TST\0\0"
	unsigned char sampling_rate[2]; // 100
	unsigned char comps;            // number of channels
	unsigned char date[3];          // 0, 0, 0
	unsigned char time[3];          // 0, 0, 0
	char latitude[11];              // 3814.4648N
	char longitude[12];             // 02143.8936E
	uint32_t block_cnt;           // 0
	unsigned char gps_cnt[4] ;                  // 0
	unsigned char pll_data[4];
	unsigned char adc[ 1 ][2];
	ext_adc_result ads1254_result[SAMPLES_PER_PACKET];
	unsigned char crc[5];
//	unsigned char patching_bytes[64];
	//power measurements space
	uint16_t nr_power_samples;
	uint16_t power_samples[31];
} app_packet;

static app_packet packet_seism_power_t;

CLIF struct process * const autostart_processes[2];


int main(void)
{
	uint8_t temp0,temp1;
	bool res;
	/* Initialize the board.
	 * The board-specific conf_board.h file contains the configuration of
	 * the board initialization.
	 */
	sysclk_init();
	board_init();
	pmic_init();					//Enables all Interrupts, i want for now only low , so i use pmic_enable_level(PMIC_LVL_LOW);
	init_10us_step_counter();
	hal_system_init();	
	timeout_init();
	
	
	/************************************************************************/
	/* Calling all testing routines                                         */
	/************************************************************************/

	
	test_usarts();		

#ifdef DEBUG_WIFLY	
	test_wifly();
#endif
	
//	test_gps();			
	
//	test_gpio_hal();
	
	
//	test_timers();
	
//	test_ads1254();

//	test_int_adc();
	
//	test_sd_mmc_software_spi();
	
//	test_rtc32_and_twi_ext();
	
	ads1254_save_to_sdmmc_driver_init();
	
	power_adc_measurements_driver_init();
	
	test_gps();
	
	//CALLING THE ACTUAL USED MAIN LOOP FUNCTION DEPENDING ON THE ACTUAL PHYSICAL LAYER USED
		
		//test_slip();	

		test_eth_socat();
	
		//test_uip_slip_processes();
		
	//END OF CALLING
	
	/************************************************************************/
	/* End of calling all testing routines                                  */
	/************************************************************************/
	
	//Enabling low Interrupts before entering main loop if not enabled already 
	
	if(!pmic_level_is_enabled(PMIC_LVL_LOW))
		pmic_enable_level(PMIC_LVL_LOW);
			
	if(!cpu_irq_is_enabled())
		cpu_irq_enable();

	
	PRINTF("\r\nEntering main test loop, only serial redirection working \r\n");
	
	while(1) {
		
			
			res = ethernet_receive_byte(HW_ETHERNET, &temp1);
			if(res==true){
				//ethernet_send_byte(HW_ETHERNET, temp1);
				//PRINTF("Got a char from ethernet, time is : %lu \r\n", clock_time());
				hal_usart_send_byte(HW_COM3, temp1);
			}			
			res = hal_usart_receive_byte(HW_COM3, &temp0);
			if(res==true)
				ethernet_send_byte(HW_ETHERNET, temp0);
			
	}	  		
	
	return 1;
}

/************************************************************************/
/* Needed Routines for testing the Usart module                         */
/************************************************************************/

	void test_usarts( void ){
	
	uint8_t tx_buf[60] = "\r\nTesting Usarts HAL, if you see this usarts are working";
	//uint8_t rx_buf[50];
	uint8_t temp_buf[50];
	
	
			// USART options.
			usart_config USART_SERIAL_OPTIONS = {
				.baudrate = 115200,
				.charlength = USART_SERIAL_CHAR_LENGTH,
				.paritytype = USART_SERIAL_PARITY,
				.stopbits = USART_SERIAL_STOP_BIT
			};
	
	//Initializing the print Redirection to usart D1
	stdio_serial_init(&USARTD1, &USART_SERIAL_OPTIONS);
	//printf("doyleuei");
	
	/************************************************************************/
	/* Com4  usart init                                                  */
	/************************************************************************/
	HW_COM3 = hal_get_hw_obj(usart,3);  //3 is rs232
	
	hal_init_hw_obj( HW_COM3);
	hal_set_config_hw_obj( HW_COM3, &USART_SERIAL_OPTIONS);
	hal_start_hw_obj(HW_COM3);
	
	PRINTF("\n\rTests Starting for ModSense HAL \n\r");
	
	//Ethernet simple usage example
	hal_usart_send_string(HW_COM3,tx_buf,57);
	
	delay_1ms_step(5000);
	
	
	uint8_t num = hal_usart_receive_string(HW_COM3,tx_buf,10);
	
	hal_usart_send_string(HW_COM3,temp_buf,sprintf((char*)temp_buf,"\r\nDiavasa %i xaraktires poy einai : ",num));
	
	hal_usart_send_string(HW_COM3,tx_buf,num);
	
	PRINTF("\r\nMacro print Diavasa %i xaraktires",num);
	
	PRINTF("\r\nEnd of Testing Usarts\r\n");
}
	
/************************************************************************/
/* Needed Routines for testing the Timer module                         */
/************************************************************************/

//! Enum with index ID for all timeout channels

void test(void){
	
	LED_Toggle(LED_GREEN);
}

void test1(void){
	
	LED_Toggle(LED_RED);
	
}

void test2(void){
	
	static uint8_t time = 0;
	PRINTF("Time: %i\r\n",time++);
	//ethernet_send_string(HW_ETHERNET,"Time: %i\r\n",9);
}


void test_timers( void )
{
	
	
	PRINTF("\r\nTesting Timers HAL test , RED and Green led should count from 0-3\r\n");
	
	
	//Interrupts must be enabled prior to inititalizing timers
	if(!pmic_level_is_enabled(PMIC_LVL_LOW))
		pmic_enable_level(PMIC_LVL_LOW);
	
	if(!cpu_irq_is_enabled()) 
		cpu_irq_enable();
		
	/************************************************************************/
	/* MODSENSE HAL                                                                     */
	/************************************************************************/
	//---------------timer 3---------------------------------------
	static struct hal_timer_config HW_TIMER3_OPTIONS;
	HW_TIMER3_OPTIONS.callback=test;
	HW_TIMER3_OPTIONS.period_timeout=1*TIMEOUT_TICK_HZ;
	HW_TIMER3_OPTIONS.type=periodic;
		
	struct HW_Object* HW_TIMER0 = hal_get_hw_obj(timer,0); //it returns a pointer to an unused timer, so no need to the int parameter
	hal_init_hw_obj( HW_TIMER0);
	hal_set_config_hw_obj( HW_TIMER0, &HW_TIMER3_OPTIONS);
	hal_start_hw_obj(HW_TIMER0);
	
	//---------------timer 4---------------------------------------
	static struct hal_timer_config HW_TIMER4_OPTIONS;
	
	HW_TIMER4_OPTIONS.callback=test1;
	HW_TIMER4_OPTIONS.period_timeout=2*TIMEOUT_TICK_HZ;
	HW_TIMER4_OPTIONS.type=periodic;
		
	struct HW_Object* HW_TIMER1 = hal_get_hw_obj(timer,0); //it returns a pointer to an unused timer, so no need to the int parameter
	hal_init_hw_obj( HW_TIMER1);
	hal_set_config_hw_obj( HW_TIMER1, &HW_TIMER4_OPTIONS);
	hal_start_hw_obj(HW_TIMER1);
	
	//---------------------Timer 5--------------------------------
	static struct hal_timer_config HW_TIMER5_OPTIONS;
		
	HW_TIMER5_OPTIONS.callback=0;  //if not set you need to check with hal_timer_expired() in main loop //polling way
	HW_TIMER5_OPTIONS.period_timeout=4*TIMEOUT_TICK_HZ;
	HW_TIMER5_OPTIONS.type=periodic;
	
	HW_TIMER2 = hal_get_hw_obj(timer,0); //it returns a pointer to an unused timer, so no need to the int parameter
	hal_init_hw_obj( HW_TIMER2);
	hal_set_config_hw_obj( HW_TIMER2, &HW_TIMER5_OPTIONS);
	hal_start_hw_obj(HW_TIMER2);
	
	// Initialize LEDs
	LED_Off(LED_GREEN);
	LED_Off(LED_RED);
	
}



void test_gpio_hal(void){
	/************************************************************************/
	/* GPIO HAL                                                              */
	/************************************************************************/

	PRINTF("\r\nTesting GPIO , RED led should flash 9 times");
	
	struct HW_Object* Red_led =  hal_get_hw_obj(gpio,LED_RED);
	
	static struct gpio_hal_config gpio_conf;
	gpio_conf.flags=IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH;
	
	hal_init_hw_obj(Red_led);
	hal_set_config_hw_obj(Red_led,&gpio_conf);
	hal_start_hw_obj(Red_led);
	
	uint8_t i=10;
	
	while(i--){
		hal_gpio_low(Red_led);
		delay_1ms_step(500);
		hal_gpio_high(Red_led);
		delay_1ms_step(500);
	}
	
	PRINTF("\r\nEnd of Testing GPIO\r\n");
	
}

uint8_t tcp_send(void){

return soc_eth_packet_send(uip_buf,uip_len);
//return slipdev_send(uip_buf,uip_len);
/*
#if COM_DRIVER_USE==slip
	return slipdev_send(uip_buf,uip_len);
#endif

#if COM_DRIVER_USE==eth_socat
	return soc_eth_packet_send(uip_buf,uip_len);
#endif
*/

}



void test_slip(){
	
	uint8_t counter = 50;
	
	PRINTF("\r\nStart of Testing SLIP\r\n");
	slipdev_init();
	//soc_eth_init();
	timeout_init();
	
	PRINTF("SLip init\r\n");
	
	//GENERAL uip and process init
	//process system init
	process_init();
	//start etimer process
	process_start(&etimer_process, NULL);
	
	//UIP and slip  system processes init
	process_start(&tcpip_process, NULL);
	
	
	uip_ipaddr_t hostaddr, netmask;

	uip_init();
	
	// uip_ipaddr(&hostaddr, 172,16,rimeaddr_node_addr.u8[0],rimeaddr_node_addr.u8[1]);
	uip_ipaddr(&hostaddr, 10,20,20,2);
	uip_ipaddr(&netmask, 255,255,255,0);
	
	//uip_ipaddr_copy(&meshif.ipaddr, &hostaddr);

	uip_sethostaddr(&hostaddr);
	uip_setnetmask(&netmask);
	
	PRINTF("uIP started with IP address %d.%d.%d.%d\r\n",	uip_ipaddr_to_quad(&uip_hostaddr));
	tcpip_set_outputfunc(&tcp_send);
	
	//autostart all processes with autostart tag , here is the er-rest-example process
	 autostart_start(autostart_processes);
	 
	 //process_start(&rest_server_example, NULL);
	
	//while(counter--){
	send_panos = false;
		
	while(1){
		
/*		if(uip_len==0){
			uip_len = soc_eth_packet_poll(uip_buf);
			if(uip_len>0)
				soc_eth_packet_send(uip_buf,uip_len);
			uip_len=0;
		}
*/		

#ifdef SEND_PANOS_ON
		if(send_panos){
					//ethernet_send_string(HW_ETHERNET,&packet_seism_power_t,960);
					hal_usart_send_string(HW_COM3,&packet_seism_power_t,960);
					send_panos=false;
		}
#endif		
		
		if(uip_len==0)
			//uip_len = soc_eth_packet_poll(uip_buf);
			uip_len = slipdev_poll(uip_buf);
		
		
		
		if(uip_len == 4 && strncmp((char*)&uip_buf[UIP_LLH_LEN], "?IPA", 4) == 0) {
			char buf[8];
			memcpy(&buf[0], "=IPA", 4);
			memcpy(&buf[4], &uip_hostaddr, 4);
			
			//uip_input();
		//	PRINTF("SLip packet IPA \r\n");
			soc_eth_packet_send(buf,8);
			//slipdev_send(buf, 8);
			uip_len=0;
			
		}
		else if(uip_len > 0){ 
		
			//PRINTF("%lu : SLip packet send to Uip \r\n",clock_time());
			//send to uip
			uip_input();	
			
			//if uip has a response, sent it back
			if(uip_len > 0){
				//soc_eth_packet_send(uip_buf,uip_len);
				slipdev_send(uip_buf,uip_len);
				
				//PRINTF("%lu :Sending response of UIP\r\n",clock_time());
				uip_len=0;
			}		
		}
		
		//Process loop
		int r;
	
		do {
			/* Reset watchdog. */
			//watchdog_periodic();
			r = process_run();
			
			//if uip has a response, sent it back
			if(uip_len > 0){
				
				slipdev_send(uip_buf,uip_len);
				//soc_eth_packet_send(uip_buf,uip_len);
				
				//PRINTF("%lu :Sending response of UIP after process run\r\n",clock_time());
				uip_len=0;
			}
			
		} while(r > 0);
		
		
		//udp periodic handling
/*		for(uint8_t i = 0; i < UIP_UDP_CONNS; i++) {
			uip_udp_periodic(i);
			if(uip_len > 0) {
				slipdev_send(uip_buf,uip_len);
				PRINTF("%lu :Sending response of UDP UIP\r\n",clock_time());
			}
		}
*/	

	
	}
	
	PRINTF("\r\nEnd of Testing SLIP\r\n");
}

void test_eth_socat(){
	
	//uint8_t counter = 50;
	
	PRINTF("\r\nStart of Testing Ethernet over socat\r\n");
	soc_eth_init();
//	timeout_init();
	
	PRINTF("Ethernet over Socat init\r\n");
	
	//GENERAL uip and process init
	process_init();
	//start etimer process
	process_start(&etimer_process, NULL);
	
	//UIP system processes init
	process_start(&tcpip_process, NULL);
	tcpip_set_outputfunc(&tcp_send);
	uip_ipaddr_t hostaddr, netmask;

	uip_init();
	
	uip_ipaddr(&hostaddr, 10,20,20,2);
	uip_ipaddr(&netmask, 255,255,255,0);
	
	uip_sethostaddr(&hostaddr);
	uip_setnetmask(&netmask);
	
	PRINTF("uIP started with IP address %d.%d.%d.%d\r\n",	uip_ipaddr_to_quad(&uip_hostaddr));

	
	//autostart all processes with autostart tag , here is the er-rest-example process
	 autostart_start(autostart_processes);
	 
	send_panos = false;
	
//	pmic_init();
		
//	if(!cpu_irq_is_enabled())
//		cpu_irq_enable();
	
	PRINTF("\r\nENTERING MAIN LOOP\r\n");
	
//	process_poll(&etimer_process);
	
	//Setup the watchdog for use in main loop
	wdt_set_timeout_period(WDT_TIMEOUT_PERIOD_8KCLK);
	wdt_enable();
	 
	while(1){
		
#ifdef SEND_PANOS_ON
		if(send_panos){
			//ethernet_send_string(HW_ETHERNET,&packet_seism_power_t,960);
			hal_usart_send_string(HW_COM3,&packet_seism_power_t,960);
			send_panos=false;
		}
#endif

#ifdef DEBUG_WIFLY
		{
		
		static uint8_t temp0;
	
		if(hal_wifi_receive_byte(HW_WIFLY, &temp0))
			hal_usart_send_byte(HW_COM3, temp0);
		
		if(hal_usart_receive_byte(HW_COM3, &temp0)){
			hal_wifi_send_byte(HW_WIFLY, temp0);
			//hal_usart_send_byte(HW_COM3, temp0);
			//hal_wifi_send_byte(HW_WIFLY,68);
		
			}
	
		}
#endif

		//Process loop
		int r;
		
		do {
			r = process_run();
			
			//if uip has a response, sent it back
			if(uip_len > 0){
				soc_eth_packet_send(uip_buf,uip_len);
				uip_len=0;
			}
			
		} while(r > 0);
		
		if(uip_len==0)
			uip_len = soc_eth_packet_poll(uip_buf);
		

		if(uip_len == 4 && strncmp((char*)&uip_buf[UIP_LLH_LEN], "?IPA", 4) == 0) {
			char buf[8];
			memcpy(&buf[0], "=IPA", 4);
			memcpy(&buf[4], &uip_hostaddr, 4);

			//uip_input();
			//	PRINTF("SLip packet IPA \r\n");
			soc_eth_packet_send(buf, 8);
			uip_len=0;
		}
		else if(uip_len > 0){
						
			//send to uip
			uip_input();
			
			//if uip has a response, sent it back
			if(uip_len > 0){
				soc_eth_packet_send(uip_buf,uip_len);
				uip_len=0;
			}
		}
	//RESET the internal watchdog		
	 wdt_reset();	
		
	}
	
	//It should never go here.	
	PRINTF("\r\nEnd of Testing Ethernet over socat\r\n");
}


void test_uip_slip_processes(void){
	

	//random generator init, must replace the seed
	random_init(1023);

	//process system init
	process_init();

	//start etimer process
	process_start(&etimer_process, NULL);


	#if WITH_UIP


	//UIP and slip  system processes init
	process_start(&tcpip_process, NULL);
	

	//AUTOSTART all processes declared autostart
	autostart_start(autostart_processes);
	  
	{
		uip_ipaddr_t hostaddr, netmask;

		uip_init();

		// uip_ipaddr(&hostaddr, 172,16,rimeaddr_node_addr.u8[0],rimeaddr_node_addr.u8[1]);
		uip_ipaddr(&hostaddr, 10,20,20,1);
		uip_ipaddr(&netmask, 255,255,255,0);

		//uip_ipaddr_copy(&meshif.ipaddr, &hostaddr);

		uip_sethostaddr(&hostaddr);
		uip_setnetmask(&netmask);

		printf("uIP started with IP address %d.%d.%d.%d\r\n",
		uip_ipaddr_to_quad(&hostaddr));
	}
	#endif /* WITH_UIP */

#if AUTOSTART_ENABLE
autostart_start(autostart_processes);
#endif

}



void test_ads1254(void){
	
	//ADS1254 result struct
	ext_adc_result ads1254_result[100];
	float voltage1= 0.0;
	float voltage2= 0.0;
	float voltage3= 0.0;
	
	init_ads1254();	
	
	//PRINTF("Started conversion, time is : %lu \r\n", clock_time());
	
	read_multiple_ads1254_result(ads1254_result,100,10);
	
	while(!read_done_ads1254());
	//PRINTF("Ended conversion, time is : %lu \r\n", clock_time());
	
/*	for(uint8_t i=0;i<100;i++){
		printf("Channel1 RAW is %lx \r\n", ads1254_result[i].channel1);
		printf("Channel2 RAW is %lx \r\n", ads1254_result[i].channel2);
		printf("Channel3 RAW is %lx \r\n", ads1254_result[i].channel3);
	}*/

/*	while(1){
		
			read_ads1254_result(&ads1254_result);
				
			voltage1 = 0.05 + ads1254_result.channel1 * (5.0 / ADC_FULLSCALE);
			voltage2 = 0.05 + ads1254_result.channel2 * (5.0 / ADC_FULLSCALE);
			voltage3 = 0.05 + ads1254_result.channel3 * (5.0 / ADC_FULLSCALE);

			printf("result is %10.8f \r\n", voltage1);
			printf("result is %10.8f \r\n", voltage2);
			printf("result is %10.8f \r\n", voltage3);
			printf("Channel1 RAW is %lx \r\n", ads1254_result.channel1);
			printf("Channel2 RAW is %lx \r\n", ads1254_result.channel2);
			printf("Channel3 RAW is %lx \r\n", ads1254_result.channel3);
	}
	*/


}



void test_rtc32_and_twi_ext(void)
{

calendar_date calend_struct;
uint32_t time;

PRINTF("\r\nStart of Testing RTC32 ext via TWI \r\n");

struct HW_Object* system_time = hal_get_hw_obj(rtc_ext,0);
hal_init_hw_obj(system_time);
hal_start_hw_obj(system_time);

delay_1ms_step(2000);

time = hal_system_time_read_sec_time(system_time);
timestamp_to_calendar(time,&calend_struct );

//PRINTF("System time in RTC form is %u/%u/%u , %u:%u:%u \r\n",rtc_time.date, rtc_time.month_century,rtc_time.year, rtc_time.hours, rtc_time.minutes, rtc_time.seconds );
PRINTF("System time in Calendar form is %u/%u/%u , %u:%u:%u \r\n",calend_struct.date, calend_struct.month, calend_struct.year, calend_struct.hour, calend_struct.minute, calend_struct.second );
PRINTF("System time in Linux timestamp form %lu \r\n", time);

PRINTF("Delaying 5 seconds \r\n");
delay_1ms_step(5000);

time = hal_system_time_read_sec_time(system_time);
timestamp_to_calendar(time,&calend_struct );

//PRINTF("System time in RTC form is %u/%u/%u , %u:%u:%u \r\n",rtc_time.date, rtc_time.month_century,rtc_time.year, rtc_time.hours, rtc_time.minutes, rtc_time.seconds );
PRINTF("System time in Calendar form is %u/%u/%u , %u:%u:%u \r\n",calend_struct.date, calend_struct.month, calend_struct.year, calend_struct.hour, calend_struct.minute, calend_struct.second );
PRINTF("System time in Linux timestamp form %lu \r\n", time);

uint16_t i=0;

//write a random timestamp , this is 23/10/13 13:48 UTC .....
if(time<1388487242)
	hal_system_time_write_time(system_time,1388487242);

time = hal_system_time_read_sec_time(system_time);
timestamp_to_calendar(time,&calend_struct );

//PRINTF("System time in RTC form is %u/%u/%u , %u:%u:%u \r\n",rtc_time.date, rtc_time.month_century,rtc_time.year, rtc_time.hours, rtc_time.minutes, rtc_time.seconds );
PRINTF("System time in Calendar form is %u/%u/%u , %u:%u:%u \r\n",calend_struct.date, calend_struct.month, calend_struct.year, calend_struct.hour, calend_struct.minute, calend_struct.second );
PRINTF("System time in Linux timestamp form %lu \r\n", time);

PRINTF("\r\nEND of Testing RTC32 ext via TWI \r\n");

}

/************************************************************************/
/* TEST INTERNAL ADC                                                    */
/************************************************************************/

static volatile double last_measurement;
static struct HW_Object* int_adc_0;

static void adc_handler(ADC_t *adc, uint8_t ch_mask, adc_result_t result)
{
	int32_t temperature;
	static int32_t index=0;
	
	//result = adc_get_signed_result(adc,ch_mask)	;
	/* Compute current temperature in Celsius, based on linearization
	 * of the temperature sensor adc data.
	 */
	/*
	if (result > 697) {
		temperature = (int8_t)((-0.0295 * result) + 40.5);
	} if (result > 420) {
		temperature = (int8_t)((-0.0474 * result) + 53.3);
	} else {
		temperature = (int8_t)((-0.0777 * result) + 65.1);
	}
	*/
	
	// Vsensor=result*Vref/Top = result/(VCC/1,6)/2047)
	//Ampere = Vsensor/2,02 * 30 
	if(result<32768)
		last_measurement = (double)((double)(result*2.0625/2047)*(double)(30/5));
	else 
	last_measurement=0;
	// Write temperature to serial
	printf("Ampere: %4.2f A, ", last_measurement);
	printf( "RAW ADC: %i ", result);
	printf( "Index: %u \n\r", index++);
	
	//start next conversion
	hal_int_adc_start_conversion(int_adc_0,ADC_CH0);
	
}

void test_int_adc(){
	
	static struct hal_adc_config config_info;
	
	//cpu_irq_enable();
	
	config_info.sign=ADC_SIGN_ON;
	config_info.resolution=ADC_RES_12;
	config_info.ref = ADC_REF_VCC;
	config_info.trig = ADC_TRIG_MANUAL;
	config_info.base_ev_ch = 0;
	config_info.nr_of_ch = 0;
	config_info.callback = adc_handler;
	
	config_info.internal_input = false;
	config_info.pos_ext_input = ADCCH_POS_PIN2;
	config_info.neg_ext_input = ADCCH_NEG_NONE;
	config_info.gain = 1;
	
	config_info.int_mode = ADCCH_MODE_COMPLETE;
	//config_info.nr_of_ch = ADC_CH0;
	
	printf("\x0C\n\r-- ADC Example --\n\r");
	
		
	int_adc_0 = hal_get_hw_obj(adc,0);
	hal_init_hw_obj(int_adc_0);
	hal_set_config_hw_obj(int_adc_0,&config_info);
	//hal_adc_set_config(int_adc_0,&config_info);
	hal_start_hw_obj(int_adc_0);
	
	hal_int_adc_start_conversion(int_adc_0,ADC_CH0);
	//adc_start_conversion(&ADCA, ADC_CH0);
	
	do {
		// Sleep until ADC interrupt triggers.
		sleepmgr_enter_sleep();
	} while (1);
	
	
}

/************************************************************************/
/* END OF TESTING INTERNAL ADC                                          */
/************************************************************************/


/************************************************************************/
/* Routines for testing SD MMC CARD                                     */
/************************************************************************/

//! \name Read/write access configurations
//! @{

//! Offset from the end of memory card to start the test
#define TEST_MEM_START_OFFSET  (1024lu * 1024lu * 1024 * 1lu) // 1gb

//! Memory area size dedicated for the read/write test
#define TEST_MEM_AREA_SIZE (1024lu * 1024lu * 1lu) // 1mb
/**
 * Size of each read or write access.
 * Increasing this number can get higher R/W performance.
 */
#define TEST_MEM_ACCESS_SIZE  (2lu * SD_MMC_BLOCK_SIZE)

#if TEST_MEM_START_OFFSET < TEST_MEM_AREA_SIZE
#  error TEST_MEM_START_OFFSET must be higher than TEST_MEM_AREA_SIZE
#endif
#if TEST_MEM_ACCESS_SIZE > TEST_MEM_AREA_SIZE
#  error TEST_MEM_AREA_SIZE must be higher than TEST_MEM_ACCESS_SIZE
#endif

//! The value used to generate test data
#define TEST_FILL_VALUE_U32     (0x5500AAFFU)

/**
 * \brief Card R/W tests
 *
 * \param slot   SD/MMC slot to test
 */
static void main_test_memory(uint8_t slot)
{
	
    struct HW_Object* hw_system_time = hal_get_hw_obj(rtc_ext,0);
    hal_init_hw_obj(hw_system_time);
    hal_start_hw_obj(hw_system_time);
    //  now = hal_system_time_read_sec_time(hw_system_time);
	
	//! Buffer used by read/write tests
	COMPILER_WORD_ALIGNED
	static uint8_t buf_test[TEST_MEM_ACCESS_SIZE];
	
	uint32_t last_blocks_addr, i, nb_trans;
	uint32_t tick_start, time_ms;

	// Compute the last address
	last_blocks_addr = sd_mmc_get_capacity(slot) *
			(1024 / SD_MMC_BLOCK_SIZE);
	if (last_blocks_addr < (TEST_MEM_START_OFFSET / 512lu)) {
		printf("[Memory is too small.]\n\r");
		return;
	}
	last_blocks_addr -= (TEST_MEM_START_OFFSET / SD_MMC_BLOCK_SIZE);

	printf("Card R/W test:\n\r");

	
	printf("    Read... ");
	tick_start = hal_system_time_read_sec_time(hw_system_time);
	if (SD_MMC_OK != sd_mmc_init_read_blocks(slot,
			last_blocks_addr,
			TEST_MEM_AREA_SIZE / SD_MMC_BLOCK_SIZE)) {
		printf("[FAIL]\n\r");
		return;
	}
	for (nb_trans = 0; nb_trans < (TEST_MEM_AREA_SIZE / TEST_MEM_ACCESS_SIZE);
			nb_trans++) {
		if (SD_MMC_OK != sd_mmc_start_read_blocks(buf_test,
					TEST_MEM_ACCESS_SIZE / SD_MMC_BLOCK_SIZE)) {
			printf("[FAIL]\n\r");
			return;
		}
		if (SD_MMC_OK != sd_mmc_wait_end_of_read_blocks()) {
			printf("[FAIL]\n\r");
			return;
		}		
	}
	time_ms =  hal_system_time_read_sec_time(hw_system_time) - tick_start;
	if (time_ms) { // Valid time_ms
		printf(" %d KBps ", (int)(((TEST_MEM_AREA_SIZE
				* 1000lu) / 1024lu) / time_ms));
	}
	printf("[OK]\n\r");

	if (sd_mmc_is_write_protected(slot)) {
		printf("Card is write protected [WRITE TEST SKIPPED]\n\r");
		return;
	}
	
	// Fill buffer with a pattern
	for (i = 0; i < (TEST_MEM_ACCESS_SIZE / sizeof(uint32_t)); i++) {
		((uint32_t*)buf_test)[i] = TEST_FILL_VALUE_U32;
	}

	printf("    Write pattern... ");
	if (SD_MMC_OK != sd_mmc_init_write_blocks(slot,
			last_blocks_addr,
			TEST_MEM_AREA_SIZE / SD_MMC_BLOCK_SIZE)) {
		printf("[FAIL]\n\r");
		return;
	}
	tick_start = hal_system_time_read_sec_time(hw_system_time);
	for (nb_trans = 0; nb_trans < (TEST_MEM_AREA_SIZE / TEST_MEM_ACCESS_SIZE);
			nb_trans++) {
		((uint32_t*)buf_test)[0] = nb_trans; // Unique value for each area
		if (SD_MMC_OK != sd_mmc_start_write_blocks(buf_test,
				TEST_MEM_ACCESS_SIZE / SD_MMC_BLOCK_SIZE)) {
			printf("[FAIL]\n\r");
			return;
		}
		if (SD_MMC_OK != sd_mmc_wait_end_of_write_blocks()) {
			printf("[FAIL]\n\r");
			return;
		}
	}
	time_ms =  hal_system_time_read_sec_time(hw_system_time) - tick_start;
	if (time_ms) { // Valid time_ms
		printf("%lu time ,  %d KBps ", time_ms, (int)(((TEST_MEM_AREA_SIZE
				* 1000lu) / 1024lu) / time_ms));
	}
	printf("[OK]\n\r");

	printf("    Read and check pattern... ");
	if (SD_MMC_OK != sd_mmc_init_read_blocks(slot,
			last_blocks_addr,
			TEST_MEM_AREA_SIZE / SD_MMC_BLOCK_SIZE)) {
		printf("Read [FAIL]\n\r");
		return;
	}
	for (nb_trans = 0; nb_trans < (TEST_MEM_AREA_SIZE / TEST_MEM_ACCESS_SIZE);
			nb_trans++) {
		// Clear buffer
		for (i = 0; i < (TEST_MEM_ACCESS_SIZE / sizeof(uint32_t)); i++) {
			((uint32_t*)buf_test)[i] = 0xFFFFFFFF;
		}
		// Fill buffer
		if (SD_MMC_OK != sd_mmc_start_read_blocks(buf_test,
					TEST_MEM_ACCESS_SIZE / SD_MMC_BLOCK_SIZE)) {
			printf("Read [FAIL]\n\r");
			return;
		}
		if (SD_MMC_OK != sd_mmc_wait_end_of_read_blocks()) {
			printf("Read [FAIL]\n\r");
			return;
		}
		// Check the unique value of the area
		if (((uint32_t*)buf_test)[0] != nb_trans) {
			printf("Check [FAIL]\n\r");
			return;
		}
		// Check buffer
		for (i = 1; i < (TEST_MEM_ACCESS_SIZE / sizeof(uint32_t)); i++) {
			if (((uint32_t*)buf_test)[i] != TEST_FILL_VALUE_U32) {
				printf("Check [FAIL]\n\r");
				//return;
			}
		}
	}
	printf("[OK]\n\r");
}

int test_sd_mmc_software_spi(void)
{
	uint8_t slot = 0;
	sd_mmc_err_t err;

	PRINTF("\r\nStart of Testing SD MMC ext via software SPI \r\n");
	
	//Interrupts must be enabled prior to initializing sd_mmc stack
//	if(!pmic_level_is_enabled(PMIC_LVL_LOW))
//		pmic_enable_level(PMIC_LVL_LOW);
		
//	if(!cpu_irq_is_enabled())
//		cpu_irq_enable();
		
	irq_initialize_vectors();
	cpu_irq_enable();

	// Initialize SD MMC stack
	sd_mmc_init();

	printf("\x0C\n\r-- SD/MMC/SDIO Card Example --\n\r");
	printf("-- Compiled: %s %s --\n\r", __DATE__, __TIME__);
	while (1) {
		if (slot == sd_mmc_nb_slot()) {
			slot = 0;
		}
		printf("Please plug an SD, MMC or SDIO card in slot %d.\n\r", slot+1);

		// Wait for a card and ready
		do {
			err = sd_mmc_check(slot);
			if ((SD_MMC_ERR_NO_CARD != err)
			&& (SD_MMC_INIT_ONGOING != err)
			&& (SD_MMC_OK != err)) {
				printf("Card install FAILED\n\r");
				printf("Please unplug and re-plug the card.\n\r");
				while (SD_MMC_ERR_NO_CARD != sd_mmc_check(slot)) {
				}
			}
		} while (SD_MMC_OK != err);
		
		struct HW_Object temp;
		temp.number=0;
		
		// Display basic card information
		print_debug_sd_info(&temp);
		
		if (sd_mmc_get_type(slot) & (CARD_TYPE_SD | CARD_TYPE_MMC)) {
			// SD/MMC Card R/W
			main_test_memory(slot);
		}

		printf("Test finished, you may unplug the card.\n\r");
		
		//while (SD_MMC_OK == sd_mmc_check(slot)) {
		//}
		PRINTF("\r\nEnd of Testing SD MMC ext via software SPI \r\n");
		return 1;
		slot++;
	}
}

/************************************************************************/
/* END of Routines for testing SD MMC CARD                                     */
/************************************************************************/

#define PERIOD_ADC 10
#define SD_MAX_OFFSET (1024lu*1024lu*1880lu)/SD_MMC_BLOCK_SIZE  



static ext_adc_result *ads1254_result, *start_of_packet;
static uint16_t count_local;
struct HW_Object* HW_TIMER_ADS1254;
	
uint32_t sd_offset;
uint8_t slot;

calendar_date calend_struct;
struct HW_Object* system_time;
struct HW_Object* sd_mmc_device;
//struct HW_Object* xport;
uint32_t  time;

 void ads1254_isr_routine(void){
	
	uint32_t tick_start, time_ms;
	static bool last_packet_failed = false;
	static uint16_t count=0;
	static bool packet_written =  true;
	
	
	//if(msec_time()==0 && !packet_written && count==100){
	if(!packet_written){
		//so no packet written during the last try, write it now or discard it
		
		PRINTF("\n\r%u",msec_time());
		PRINTF("-%u L ",count);

		//CALL the write to MMC SD card
		ads1254_result = start_of_packet;
		count_local = SAMPLES_PER_PACKET-1;
		
		LED_Toggle(LED_RED);
		
		//send data for debug through ethernet serial
		send_panos = true;

		count=0;
		
		if(!write_hashed_sd_buffer(&packet_seism_power_t,packet_seism_power_t.block_cnt))
			PRINTF("ERROR: Failed to write packet at 0 msec time %lu\r\n",hal_system_time_read_sec_time(system_time));
		
		packet_written=true;
	}
	
	if(msec_time()==100 || msec_time()==900){
		//continue with the new packet, do 2 tries on writing its header 
		
		uint32_t system_sec_time = 	hal_system_time_read_sec_time(system_time);
		timestamp_to_calendar(system_sec_time,&calend_struct );
	
		packet_seism_power_t.date[0]=calend_struct.date;
		packet_seism_power_t.date[1]=calend_struct.month;
		packet_seism_power_t.date[2]=calend_struct.year-2000;
	
		packet_seism_power_t.time[2]=calend_struct.second;
		packet_seism_power_t.time[1]=calend_struct.minute;
		packet_seism_power_t.time[0]=calend_struct.hour;

		packet_seism_power_t.block_cnt = system_sec_time;
	}
	//printf("%lu\r\n",hal_system_time_read_msec_time(system_time)); 
	
	//Protect from buffer overrun
	if(count==100){
		ads1254_result = start_of_packet;
		count_local = SAMPLES_PER_PACKET-1;
	}
		
	read_ads1254_result(ads1254_result++);
//	printf("%u-",msec_time());
	
	//read_ads1254_result(&packet_seism_power_t.ads1254_result[(msec_time()/10)]);
	count++;
//	printf("%u ADS ",msec_time());
//	printf("pl %u ",msec_time()/10);
//	printf("cnt %u\r\n",count);
	
//	if(!count_local--){
	if(msec_time()==990||count==100){
	//if(msec_time()==990){
		
		PRINTF("\n\r%u",msec_time());
		PRINTF("-%u ",count);
						
		//CALL the write to MMC SD card
		ads1254_result = start_of_packet;
		count_local = SAMPLES_PER_PACKET-1;
		
		LED_Toggle(LED_RED);
		
		//write packet of data
//		if(count==100){
			//Write to SD card with new API
			//if(!write_sd_buffer(&packet_seism_power_t)){
		if(write_hashed_sd_buffer(&packet_seism_power_t,packet_seism_power_t.block_cnt)){
			//printf("not free isr %lu\r\n",clock_time());
			packet_written=true;
			count=0;
			//send data for debug through ethernet serial
			send_panos = true;			
		}	
		else {
			packet_written=false;
		}
		
	}

}


bool ads1254_save_to_sdmmc_driver_init(void){
	
	//This routine is responsible for setting an interrupt to tick every period msec and make the adc conversion
	//Then store the data when reaching full 900 bytes = 1 second of data to the SD MMC as a packet ready to be sent to server
	
	PRINTF("\r\nSTARTING the daemon for making ADS1254 ADC reads and storing them to SD MMC\r\n");
	
	static struct hal_timer_config ADS_TIMER_OPTIONS;
	ADS_TIMER_OPTIONS.callback=ads1254_isr_routine;
	ADS_TIMER_OPTIONS.period_timeout=PERIOD_ADC;
	ADS_TIMER_OPTIONS.type=periodic;
	
	HW_TIMER_ADS1254 = hal_get_hw_obj(timer,0); //it returns a pointer to an unused timer, so no need to the int parameter
	if(!hal_init_hw_obj( HW_TIMER_ADS1254))
		PRINTF("ERROR, cant init HW TIMER");
	hal_set_config_hw_obj( HW_TIMER_ADS1254, &ADS_TIMER_OPTIONS);
	
	system_time = hal_get_hw_obj(rtc_ext,0);
	hal_init_hw_obj(system_time);
	hal_start_hw_obj(system_time);
	time = hal_system_time_read_sec_time(system_time);
	
	
	sd_mmc_device = hal_get_hw_obj(sd_mmc,0);
	if(!hal_init_hw_obj( sd_mmc_device))
		PRINTF("ERROR, cant init SD MMC");
	hal_start_hw_obj(sd_mmc_device);
	
	//Init the ADS1254 system
	init_ads1254();	
	
	ads1254_result = (ext_adc_result*) &packet_seism_power_t.ads1254_result;
	start_of_packet = ads1254_result;
	count_local = SAMPLES_PER_PACKET-1;
	
	//Initialize and start the cyclic SD buffer
	set_config_sd_buffer(sd_mmc_device,false);
	init_sd_buffer();
	start_sd_buffer();
	
	//initialize the header of the packet
	//seism_header temp_header;
	sprintf(&packet_seism_power_t.id,"HDA\0");
	//packet_seism_power_t.id = "HDA\0";
	sprintf(&packet_seism_power_t.station_name, "TST\0\0");
	//packet_seism_power_t.station_name = "TST\0\0";
	*packet_seism_power_t.sampling_rate = 100;
	packet_seism_power_t.comps = 3;
	packet_seism_power_t.block_cnt =  0;
	
	//initialize the packet buffer for first use with 0x800000 in each sample position,only 0x80 is needed to be written
	for(uint16_t i=0;i<SAMPLES_PER_PACKET;i++){
		
		packet_seism_power_t.ads1254_result[i].data[2]=0x80;
		packet_seism_power_t.ads1254_result[i].data[5]=0x80;
		packet_seism_power_t.ads1254_result[i].data[8]=0x80;
		
	}
	
	//Start the software timer , after synchronizing with the start of a second
	while(hal_system_time_read_msec_time(system_time)%1000);
	hal_start_hw_obj(HW_TIMER_ADS1254);
	PRINTF("Starting ADS interrupt , %lu \r\n ",hal_system_time_read_msec_time(system_time));
	
	PRINTF("ADS1254 DAEMON started\r\n");
	
	return true;
}

/************************************************************************/
/* POWER  ADC reading routines                                          */
/************************************************************************/
#define POWER_SAMPLES 5 //max 30
#define POWER_ADC_PERIOD 1000/POWER_SAMPLES

static struct HW_Object* power_adc, *power_timer;

void power_isr_routine(){

	hal_int_adc_start_conversion(power_adc,ADC_CH0);

}

static void power_adc_handler(ADC_t *adc, uint8_t ch_mask, adc_result_t result)
{
	static uint8_t index=0;
	uint32_t final_result=0;
	

/*	
	if(result<32768)
		last_measurement = (double)((double)(result*2.0625/2047)*(double)(30/5));
	else 
		last_measurement=0;
	// Write temperature to appropriate place in result vector
	final_result = last_measurement*1000; //in mA
*/
	if(result<5||result>32768)	
		final_result=0;
	else
		// Vsensor=result*Vref/Top = result*(VCC/1,6)/2047)
		//Ampere = Vsensor/5 * 30
		final_result =  (result*60454)/10000;
	
	
	
	packet_seism_power_t.power_samples[index] = final_result;
	index = (index +1)%POWER_SAMPLES;

//	PRINTF("%4.2f A, ", last_measurement);
	PRINTF(" P%u ",msec_time());
//	PRINTF("%u mA, ", final_result);
//	PRINTF( "RAW ADC: %i \r\n", result);
//	printf( "Index: %u \n\r", index++);
	
	//start next conversion
	//hal_int_adc_start_conversion(int_adc_0,ADC_CH0);
	
}


void power_adc_measurements_driver_init(void){
	
	PRINTF("\r\nSTARTING the daemon for making POWER reads and storing them to SD MMC\r\n");
	
	struct hal_adc_config config_info;
	
	//Configuration of the internal ADC
	config_info.sign=ADC_SIGN_ON;
	config_info.resolution=ADC_RES_12;
	config_info.ref = ADC_REF_VCC;
	config_info.trig = ADC_TRIG_MANUAL;
	config_info.base_ev_ch = 0;
	config_info.nr_of_ch = 0;
	config_info.callback = power_adc_handler;
	
	config_info.internal_input = false;
	config_info.pos_ext_input = ADCCH_POS_PIN2;
	config_info.neg_ext_input = ADCCH_NEG_NONE;
	config_info.gain = 1;
	
	config_info.int_mode = ADCCH_MODE_COMPLETE;
	//config_info.nr_of_ch = ADC_CH0;
	
	
	//ADC CONFIG and init/start
	power_adc = hal_get_hw_obj(adc,0);
	hal_init_hw_obj(power_adc);
	hal_set_config_hw_obj(power_adc,&config_info);
	hal_start_hw_obj(power_adc);
	
	//TIMER configuration
	static struct hal_timer_config POWER_TIMER_OPTIONS;
	POWER_TIMER_OPTIONS.callback=power_isr_routine;
	POWER_TIMER_OPTIONS.period_timeout= POWER_ADC_PERIOD; 
	POWER_TIMER_OPTIONS.type=periodic;
	
	power_timer = hal_get_hw_obj(timer,0); //it returns a pointer to an unused timer, so no need to the int parameter
	if(!hal_init_hw_obj( power_timer))
	PRINTF("ERROR, cant init HW TIMER");
	hal_set_config_hw_obj( power_timer, &POWER_TIMER_OPTIONS);
	
	//Start the software timer , after synchronizing with 120msec. 
	while(hal_system_time_read_msec_time(system_time)%1000);
	delay_1ms_step(120);
	hal_start_hw_obj(power_timer);
	PRINTF("Starting internal ADC power measurements interrupt , %lu \r\n ",hal_system_time_read_msec_time(system_time));
	
	
	packet_seism_power_t.nr_power_samples=POWER_SAMPLES;
	
}

/************************************************************************/
/* GPS test and attributes                                              */
/************************************************************************/

#define GPS_PERIOD 60000  //1 min 
#define GPS_REUSE 120 //1 min * 60 = 1 H

static struct hal_gps_config config_info;
static struct HW_Object* gps_obj,*gps_timer;

void gps_isr_routine(){

	//restart the gps
	static uint8_t min_counter = 0;
	
	min_counter++;
	
	
	if(min_counter>=GPS_REUSE){
		PRINTF("\r\nStarting GPS\r\n");
		if(!gps_obj->started)
			hal_start_hw_obj(gps_obj);	
		min_counter=0;
	}


}

void test_gps(void){
	
	//TIMER configuration
	static struct hal_timer_config GPS_TIMER_OPTIONS;
	GPS_TIMER_OPTIONS.callback=gps_isr_routine;
	GPS_TIMER_OPTIONS.period_timeout= GPS_PERIOD;
	GPS_TIMER_OPTIONS.type=periodic;
	

	
	gps_obj = hal_get_hw_obj(gps,0);
	
	hal_init_hw_obj(gps_obj);
	hal_set_config_hw_obj(gps_obj, &config_info);
	
	hal_start_hw_obj(gps_obj);
	
	gps_timer = hal_get_hw_obj(timer,0); //it returns a pointer to an unused timer, so no need for the int parameter
	if(gps_timer==NULL)
		PRINTF("ERROR, couldn't allocate timer");
	if(!hal_init_hw_obj( gps_timer))
		PRINTF("ERROR, cant init HW TIMER");
	hal_set_config_hw_obj( gps_timer, &GPS_TIMER_OPTIONS);
	hal_start_hw_obj(gps_timer);
			
}
	
void test_wifly(void){

	static uint8_t temp0;
	
	HW_WIFLY = hal_get_hw_obj(wifi,0);
	hal_init_hw_obj( HW_WIFLY);
	//USART_SERIAL_OPTIONS.baudrate=230400;
	//hal_set_config_hw_obj( HW_WIFLY, &USART_SERIAL_OPTIONS);
	hal_start_hw_obj(HW_WIFLY);
	
	PRINTF("STARTING TESTING OF WIFLY COMMUNICATIONS\r\n");	
	while(true){
	
	
	if(hal_wifi_receive_byte(HW_WIFLY, &temp0))
		hal_usart_send_byte(HW_COM3, temp0);
	
	if(hal_usart_receive_byte(HW_COM3, &temp0)){
		hal_wifi_send_byte(HW_WIFLY, temp0);
		//hal_usart_send_byte(HW_COM3, temp0);
		//hal_wifi_send_byte(HW_WIFLY,68);
		}
	
	}

}
