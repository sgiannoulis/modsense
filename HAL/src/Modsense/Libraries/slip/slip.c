/** \addtogroup slip
 * @{ */

#include "slip.h"
#include "definitions.h"
/*
 * This is a generic implementation of the SLIP protocol over an RS232
 * (serial) device.
 *
 * Huge thanks to Ullrich von Bassewitz <uz@cc65.org> of cc65 fame for
 * and endless supply of bugfixes, insightsful comments and
 * suggestions, and improvements to this code!
 */

//#include "uip.h"
//#include "uip-fw.h"
//#include "slip.h"

//uint8_t temporary_buffer1[500];

//#define PRINTF(...)  ethernet_send_string(&global_hw_registry.ETHERNET0 ,temporary_buffer1,sprintf((char*)temporary_buffer1, __VA_ARGS__ ))


#include <string.h>  /* For memcpy() */

struct HW_Object* HW_COM;

#define SLIP_END     0300
#define SLIP_ESC	 0333
#define SLIP_ESC_END 0334
#define SLIP_ESC_ESC 0335

#define SERIAL_PORT						3
#define USART_SLIP_BAUDRATE			   115200
#define USART_SLIP_CHAR_LENGTH         USART_CHSIZE_8BIT_gc
#define USART_SLIP_PARITY              USART_PMODE_DISABLED_gc
#define USART_SLIP_STOP_BIT            false

static uint8_t slip_buf[UIP_BUFSIZE];

static uint16_t len, tmplen;
static uint8_t lastc;

/*-----------------------------------------------------------------------------------*/
/**
 * Send the packet in the uip_buf and uip_appdata buffers using the
 * SLIP protocol.
 *
 * The first 40 bytes of the packet (the IP and TCP headers) are read
 * from the uip_buf buffer, and the following bytes (the application
 * data) are read from the uip_appdata buffer.
 *
 * \return This function will always return UIP_FW_OK.
 */
/*-----------------------------------------------------------------------------------*/

uint8_t
slipdev_send(uint8_t* buffer, uint16_t len)  //it was len = uip_len and buffer = uip_buff
{
	  uint16_t i;
	  uint8_t *ptr;
	  uint8_t c;
	
	 // slipdev_char_put(SLIP_END);
	
	  ptr = buffer; //TODO check here if buffer needs dereference &
	  for(i = 0; i < len; ++i) {
		    if(i == UIP_TCPIP_HLEN) {
		  	   ptr = (char *)uip_appdata;
		    }
		    c = *ptr++;
		    switch(c) {
			    case SLIP_END:
			      slipdev_char_put(SLIP_ESC);
			      slipdev_char_put(SLIP_ESC_END);
			      break;
			    case SLIP_ESC:
			      slipdev_char_put(SLIP_ESC);
			      slipdev_char_put(SLIP_ESC_ESC);
			      break;
			    default:
			      slipdev_char_put(c);
			      break;
		    }
	  }
	  slipdev_char_put(SLIP_END);
	
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
uint16_t slipdev_poll(uint8_t* buffer)
{
	  uint8_t c;
	
	//find a SLIP END to mark the start of a packet
	/*
	 while(slipdev_char_poll(&c)){
		  if(c==SLIP_END)
			break;
	 }
	 if(c!=SLIP_END)
		return 0;
	*/
	
	  //PRINTF("Slip_dev_poll : Polling for packet"); 
	 
	  while(slipdev_char_poll(&c)) {
		   
		 //   PRINTF("Slip_dev_poll : Got char %c",c);
			
			switch(c) {
			    case SLIP_ESC:
			      lastc = c;
			      break;
			
			    case SLIP_END:
			      lastc = c;
			      /* End marker found, we copy our input buffer to the uip_buf
			         buffer and return the size of the packet we copied. */
			      memcpy(buffer, slip_buf, len); //TODO check here if buffer needs dereference &
			      tmplen = len;
			      len = 0;
			      return tmplen;
			
			    default:
			      if(lastc == SLIP_ESC) {
				        lastc = c;
				        /* Previous read byte was an escape byte, so this byte will be
				           interpreted differently from others. */
				        switch(c) {
					        case SLIP_ESC_END:
					          c = SLIP_END;
					          break;
					        case SLIP_ESC_ESC:
					          c = SLIP_ESC;
					          break;
				        }
				      } else {
				        lastc = c;
			      }
			
			      slip_buf[len] = c;
			      ++len;
			
			      if(len > UIP_BUFSIZE) {
				        len = 0;
			      }
			
			      break;
		    }
	  }
	  //PRINTF("Slip_dev_poll : No packet found " );
	   
	  return 0;
}
/*-----------------------------------------------------------------------------------*/
/**
 * Initialize the SLIP module.
 *
 * This function does not initialize the underlying RS232 device, but
 * only the SLIP part.
 */
/*-----------------------------------------------------------------------------------*/

void
slipdev_init(void)
{	
	// USART options.
	usart_config USART_SERIAL_OPTIONS = {
		.baudrate = USART_SLIP_BAUDRATE,
		.charlength = USART_SLIP_CHAR_LENGTH,
		.paritytype = USART_SLIP_PARITY,
		.stopbits = USART_SLIP_STOP_BIT
	};
		
	HW_COM = hal_get_hw_obj(usart,SERIAL_PORT);  //the gateway to PC through RS232 to USB converter
	hal_init_hw_obj( HW_COM);
	hal_set_config_hw_obj( HW_COM, &USART_SERIAL_OPTIONS);
	hal_start_hw_obj(HW_COM);
	
	
	lastc = len = 0;
	  
	 
}

/*-----------------------------------------------------------------------------------*/
/**
 * The bind to receive function.
 */
 /*-----------------------------------------------------------------------------------*/
uint8_t slipdev_char_poll(uint8_t *c){
	return hal_usart_receive_byte(HW_COM,c);
}


/*-----------------------------------------------------------------------------------*/
/**
 * The bind to send function.
 */
 /*-----------------------------------------------------------------------------------*/
void slipdev_char_put(uint8_t c){
	hal_usart_send_byte(HW_COM,c);
} 


#ifdef slip_process_on

PROCESS_THREAD(slip_process, ev, data)
{
	PROCESS_BEGIN();

	//rxbuf_init();

	while(1) {
		PROCESS_YIELD_UNTIL(ev == PROCESS_EVENT_POLL);
		
		slip_active = 1;

		/* Move packet from rxbuf to buffer provided by uIP. */
		uip_len = slip_poll_handler(&uip_buf[UIP_LLH_LEN],
		UIP_BUFSIZE - UIP_LLH_LEN);
		#if !UIP_CONF_IPV6
		if(uip_len == 4 && strncmp((char*)&uip_buf[UIP_LLH_LEN], "?IPA", 4) == 0) {
			char buf[8];
			memcpy(&buf[0], "=IPA", 4);
			memcpy(&buf[4], &uip_hostaddr, 4);
			if(input_callback) {
				input_callback();
			}
			slip_write(buf, 8);
		} else if(uip_len > 0
		&& uip_len == (((uint16_t)(BUF->len[0]) << 8) + BUF->len[1])
		&& uip_ipchksum() == 0xffff) {
			#define IP_DF   0x40
			if(BUF->ipid[0] == 0 && BUF->ipid[1] == 0 && BUF->ipoffset[0] & IP_DF) {
				static uint16_t ip_id;
				uint16_t nid = ip_id++;
				BUF->ipid[0] = nid >> 8;
				BUF->ipid[1] = nid;
				nid = uip_htons(nid);
				nid = ~nid;		/* negate */
				BUF->ipchksum += nid;	/* add */
				if(BUF->ipchksum < nid) { /* 1-complement overflow? */
					BUF->ipchksum++;
				}
			}
			#ifdef SLIP_CONF_TCPIP_INPUT
			SLIP_CONF_TCPIP_INPUT();
			#else
			tcpip_input();
			#endif
			} else {
			uip_len = 0;
			SLIP_STATISTICS(slip_ip_drop++);
		}
		#else /* UIP_CONF_IPV6 */
		if(uip_len > 0) {
			if(input_callback) {
				input_callback();
			}
			#ifdef SLIP_CONF_TCPIP_INPUT
			SLIP_CONF_TCPIP_INPUT();
			#else
			tcpip_input();
			#endif
		}
		#endif /* UIP_CONF_IPV6 */
	}

	PROCESS_END();
}
/*---------------------------------------------------------------------------*/

#endif //slip_process_on

/** @} */