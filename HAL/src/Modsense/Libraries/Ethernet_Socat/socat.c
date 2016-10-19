/** \addtogroup slip
 * @{ */

#include "socat.h"
#include "definitions.h"

#include <string.h>  /* For memcpy() */

struct HW_Object* HW_COM_SOCAT;
//struct HW_Object* HW_COM_SOCAT_TEST;

static uint8_t socat_buf[UIP_BUFSIZE];

static uint16_t len, tmplen;

typedef enum {
	IDLE,
	READING_HEADER,
	READING_DATA
}state;


/*-----------------------------------------------------------------------------------*/
/**
 * Send the packet in the uip_buf and uip_appdata buffers using the
 * ethernet/wifi hw.
 *
 * The first 40 bytes of the packet (the IP and TCP headers) are read
 * from the uip_buf buffer, and the following bytes (the application
 * data) are read from the uip_appdata buffer.
 *
 * \return This function will always return UIP_FW_OK.
 */
/*-----------------------------------------------------------------------------------*/

uint8_t
soc_eth_packet_send(uint8_t* buffer, uint16_t lenght)  //it was len = uip_len and buffer = uip_buff
{
	  uint16_t i;
	  uint8_t *ptr;
	  uint8_t c;
	
	 //create the necessary  by socat 00 00 08 00 sequence before the packet
	 soc_eth_char_put(0x00);
	 soc_eth_char_put(0x00);
	 soc_eth_char_put(0x08);
	 soc_eth_char_put(0x00);
	
	  ptr = buffer;
	  for(i = 0; i < lenght; i++) {
		    if(i == UIP_TCPIP_HLEN) {
		  	   ptr = (char *)uip_appdata;
		    }
		    c = *ptr++;
			soc_eth_char_put(c);
	 }
	  
	 //send 0d 0a so that ethernet hw will send the packet immediately / DONT , wont work for various reasons
//	 soc_eth_char_put(0x0d);
//	 soc_eth_char_put(0x0a);
	  
	
	  return 0;
}
/*-----------------------------------------------------------------------------------*/
/**
 * Poll the SLIP device for an available packet.
 *
 * This function will poll the SLIP device to see if a packet is
 * available. It uses a buffer in which all avalaible bytes from the
 * RS232 interface are read into. When a full packet has been read
 * into the buffer, the packet is copied into the uip_buf buffer and
 * the length of the packet is returned.
 *
 * \return The length of the packet placed in the uip_buf buffer, or
 * zero if no packet is available.
 */
/*-----------------------------------------------------------------------------------*/
uint16_t soc_eth_packet_poll(uint8_t* buffer)
{
	uint8_t c;
	uint16_t i;
	
	uint16_t msb,lsb;
	static uint16_t packet_length=0;
	static state receive_state = IDLE;
	
	//PRINTF(" %u\r\n",msec_time());
	 
	 while(soc_eth_char_poll(&c)){
	 		 
		 switch (receive_state){
			 
			case IDLE:
				if(c==0x45){
				receive_state=READING_HEADER;
//				PRINTF("Start of packet, ");
				socat_buf[len] = c;
				++len;
				}
			break;
			 
			case READING_HEADER:
				socat_buf[len] = c;
				++len;
				
				if(len > UIP_BUFSIZE){ //buffer overflow , packet lost
					PRINTF("ERROR Buffer overflow \r\n");
					len = 0;
					receive_state = IDLE;
					return 0;
				}
				 
				if(len==UIP_IPH_LEN){
					receive_state=READING_DATA;
				
					struct uip_ip_hdr* ip_head = socat_buf;
					msb = ip_head->len[0];
					msb<<8;
					lsb = ip_head->len[1];
					packet_length = msb + lsb;
//					PRINTF("packet size is %u ",packet_length);
				}
			break;
			 
			case READING_DATA:
				
				socat_buf[len] = c;
				++len;
								
				if(len > UIP_BUFSIZE){ //buffer overflow , packet lost
					PRINTF("ERROR Buffer overflow \r\n");
					len = 0;
					receive_state = IDLE;
					return 0;
				}
				
				if(len>=packet_length){
					/* End of packet found, we copy our input buffer to the uip_buf
					buffer and return the size of the packet we copied. */
					//PRINTF("passed %u bytes to uip\r\n",len);
					memcpy(buffer, socat_buf, len); 
					tmplen = len;
					len = 0;
					receive_state = IDLE;
					return tmplen;
				}
			break;
			 
			default:
			return 0;
		 }		 
	 }
	 
	 return 0;
	 
}

/*-----------------------------------------------------------------------------------*/
/**
 * Initialize the socat module.
 *
 * This function does initialize the underlying RS232 device, and
 * the socat part.
 */
/*-----------------------------------------------------------------------------------*/

void
soc_eth_init(void)
{	
	
#ifdef USE_ETH	
	//Using the Ethernet
	HW_COM_SOCAT = hal_get_hw_obj(ethernet,0);
	
	hal_init_hw_obj( HW_COM_SOCAT);
	//hal_set_config_hw_obj( HW_COM_SOCAT, &USART_SERIAL_OPTIONS);
	hal_start_hw_obj(HW_COM_SOCAT);
#endif

#ifdef USE_WIFI	
	//Using the WIFLY
	HW_COM_SOCAT = hal_get_hw_obj(wifi,0);
		
	hal_init_hw_obj( HW_COM_SOCAT);
	//hal_set_config_hw_obj( HW_COM_SOCAT, &USART_SERIAL_OPTIONS);
	hal_start_hw_obj(HW_COM_SOCAT);
	//End of wifi
#endif
	
			
	len = 0;
	 
}

/*-----------------------------------------------------------------------------------*/
/**
 * The bind to receive function.
 */
 /*-----------------------------------------------------------------------------------*/
inline uint8_t soc_eth_char_poll(uint8_t *c){

#ifdef USE_ETH
	return ethernet_receive_byte(HW_COM_SOCAT,c);
#endif

#ifdef USE_WIFI
	return hal_wifi_receive_byte(HW_COM_SOCAT,c);
#endif

}


/*-----------------------------------------------------------------------------------*/
/**
 * The bind to send function.
 */
 /*-----------------------------------------------------------------------------------*/
inline void soc_eth_char_put(uint8_t c){
#ifdef USE_ETH
	ethernet_send_byte(HW_COM_SOCAT,c);
#endif

#ifdef USE_WIFI
	return hal_wifi_send_byte(HW_COM_SOCAT,c);
#endif

} 

/** @} */