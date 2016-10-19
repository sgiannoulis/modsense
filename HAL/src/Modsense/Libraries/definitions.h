/*
 * definitions.h
 *
 * Created: 18/6/2013 7:45:46 μμ
 *  Author: spilios
 */ 


#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_

#include "compiler.h"
#include "hal_interface.h"

//#include "uip.h"
//#include "process.h"

#define BOARD USER_BOARD
//#define SEND_PANOS_ON
//#define DEBUG_WIFLY


//HERE DEFINE WHICH COMMUNICATION WILL BE USED, ETHERNET OR WIFI
#define USE_ETH
//#define USE_WIFI

uint8_t temporary_buffer[100];
//#define PRINTF(...)  ethernet_send_string(&global_hw_registry.ETHERNET0 ,temporary_buffer,sprintf((char*)temporary_buffer, __VA_ARGS__ )) 

//#define PRINTF(...)  hal_usart_send_string(&global_hw_registry.COM3 ,temporary_buffer,sprintf((char*)temporary_buffer, __VA_ARGS__ ))
#define PRINTF(...)

#define WITH_COAP  13
//#define REST coap_rest_implementation;


#define CLOCK_SECOND TIMEOUT_TICK_HZ
//#define CLOCK_SECOND 1000


//Runtime passed in contiki
#define AUTOSTART_ENABLE 1
#define UIP_CONF_TCP 1
#define WITH_UIP6 0
#define WITH_UIP 1
#define UIP_CONF_UDP 1



//Some UIP etc defines needed , decided to put them here instead of uip.h to keep uip.h as original

#define CCIF
#define CLIF

#define UIP_CONF_BUFFER_SIZE 1280
#define UIP_CONF_LLH_LEN 0
#define UIP_CONF_ACTIVE_OPEN 0

/* Header sizes. */
#if UIP_CONF_IPV6
#define UIP_IPH_LEN    40
#define UIP_FRAGH_LEN  8
#else /* UIP_CONF_IPV6 */
#define UIP_IPH_LEN    20    /* Size of IP header */
#endif /* UIP_CONF_IPV6 */

#define UIP_UDPH_LEN    8    /* Size of UDP header */
#define UIP_TCPH_LEN   20    /* Size of TCP header */

#ifdef UIP_IPH_LEN
#define UIP_ICMPH_LEN   4    /* Size of ICMP header */
#endif

#define UIP_IPUDPH_LEN (UIP_UDPH_LEN + UIP_IPH_LEN)    /* Size of IP + UDP header */
#define UIP_IPTCPH_LEN (UIP_TCPH_LEN + UIP_IPH_LEN)    /* Size of IP + TCP header */
#define UIP_TCPIP_HLEN UIP_IPTCPH_LEN
#define UIP_IPICMPH_LEN (UIP_IPH_LEN + UIP_ICMPH_LEN) /* size of ICMP+ IP header */
#define UIP_LLIPH_LEN (UIP_LLH_LEN + UIP_IPH_LEN)    /* size of L2 + IP header */


#define UIP_CONF_UDP_CONNS       4 // was 12

typedef unsigned short uip_stats_t;
typedef uint32_t clock_time_t;

//uint16_t 
//typedef uip_ip4addr_t uip_ipaddr_t;

#define uip_ipaddr(addr, addr0,addr1,addr2,addr3) do {  \
	(addr)->u8[0] = addr0;                              \
	(addr)->u8[1] = addr1;                              \
	(addr)->u8[2] = addr2;                              \
	(addr)->u8[3] = addr3;                              \
} while(0)

//extern uip_ipaddr_t manos_ip;
extern uint16_t  manos_port;


#ifndef uip_ipaddr_copy
#define uip_ipaddr_copy(dest, src) (*(dest) = *(src))
#endif



#endif /* DEFINITIONS_H_ */