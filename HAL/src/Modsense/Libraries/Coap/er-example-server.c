/*
 * Copyright (c) 2013, Matthias Kovatsch
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 */

/**
 * \file
 *      Erbium (Er) REST Engine example (with CoAP-specific code)
 * \author
 *      Matthias Kovatsch <kovatsch@inf.ethz.ch>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "contiki.h"
#include "contiki-net.h"
#include "autostart.h"
#include "hal_interface.h"
#include "er-example-server.h"
#include "ads1254.h"
#include "timeout.h"
#include "cyclic_buffer_sd_mmc.h"

/* Define which resources to include to meet memory constraints. */
#define REST_RES_HELLO 0
#define REST_RES_CHUNKS 0
#define REST_RES_SEPARATE 0
#define REST_RES_PUSHING 0
#define REST_POWER_RES_PUSHING 1  //Modsense observe enabled power

#define REST_RES_EVENT 0
#define REST_RES_SUB 0
#define REST_RES_LEDS 0
#define REST_RES_TOGGLE 0
#define REST_RES_SEISMO 1 //MODSENSE seismograph
#define REST_RES_BATTERY 0
#define REST_RES_RADIO 0
#define REST_RES_MIRROR 0 /* causes largest code size */
#define REST_RES_POWER 0 // MODSESENSE power
#define CONFIG_TIME 1 //Config modsense debug
#define PLATFORM_HAS_LEDS 0

#include "erbium.h"


#if defined (PLATFORM_HAS_BUTTON)
#include "dev/button-sensor.h"
#endif
#if defined (PLATFORM_HAS_LEDS)
#include "led.h"
#endif
#if defined (PLATFORM_HAS_LIGHT)
#include "dev/light-sensor.h"
#endif
#if defined (PLATFORM_HAS_BATTERY)
#include "dev/battery-sensor.h"
#endif
#if defined (PLATFORM_HAS_SHT11)
#include "dev/sht11-sensor.h"
#endif
#if defined (PLATFORM_HAS_RADIO)
#include "dev/radio-sensor.h"
#endif


/* For CoAP-specific example: not required for normal RESTful Web service. */
#if WITH_COAP == 3
#include "er-coap-03.h"
#elif WITH_COAP == 7
#include "er-coap-07.h"
#elif WITH_COAP == 12
#include "er-coap-12.h"
#elif WITH_COAP == 13
#include "er-coap-13.h"
#else
#warning "Erbium example without CoAP-specifc functionality"
#endif /* CoAP-specific example */

/*#define DEBUG 0
#if DEBUG
//#define PRINTF(...) printf(__VA_ARGS__)
#define PRINTF(...) ethernet_send_string(&global_hw_registry.ETHERNET0 ,temporary_buffer,sprintf((char*)temporary_buffer, __VA_ARGS__ )) 
#define PRINT6ADDR(addr) PRINTF("[%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x]", ((uint8_t *)addr)[0], ((uint8_t *)addr)[1], ((uint8_t *)addr)[2], ((uint8_t *)addr)[3], ((uint8_t *)addr)[4], ((uint8_t *)addr)[5], ((uint8_t *)addr)[6], ((uint8_t *)addr)[7], ((uint8_t *)addr)[8], ((uint8_t *)addr)[9], ((uint8_t *)addr)[10], ((uint8_t *)addr)[11], ((uint8_t *)addr)[12], ((uint8_t *)addr)[13], ((uint8_t *)addr)[14], ((uint8_t *)addr)[15])
#define PRINTLLADDR(lladdr) PRINTF("[%02x:%02x:%02x:%02x:%02x:%02x]",(lladdr)->addr[0], (lladdr)->addr[1], (lladdr)->addr[2], (lladdr)->addr[3],(lladdr)->addr[4], (lladdr)->addr[5])
#else
#define PRINTF(...)
#define PRINT6ADDR(addr)
#define PRINTLLADDR(addr)
#endif
*/

/******************************************************************************/
#if REST_RES_HELLO
/*
 * Resources are defined by the RESOURCE macro.
 * Signature: resource name, the RESTful methods it handles, and its URI path (omitting the leading slash).
 */
RESOURCE(helloworld, METHOD_GET, "hello", "title=\"Hello world: ?len=0..\";rt=\"Text\"");

/*
 * A handler function named [resource name]_handler must be implemented for each RESOURCE.
 * A buffer for the response payload is provided through the buffer pointer. Simple resources can ignore
 * preferred_size and offset, but must respect the REST_MAX_CHUNK_SIZE limit for the buffer.
 * If a smaller block size is requested for CoAP, the REST framework automatically splits the data.
 */
void
helloworld_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  const char *len = NULL;
  /* Some data that has the length up to REST_MAX_CHUNK_SIZE. For more, see the chunk resource. */
  char const * const message = "Hello World! ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxy";
  int length = 12; /*           |<-------->| */

  /* The query string can be retrieved by rest_get_query() or parsed for its key-value pairs. */
  if (REST.get_query_variable(request, "len", &len)) {
    length = atoi(len);
    if (length<0) length = 0;
    if (length>REST_MAX_CHUNK_SIZE) length = REST_MAX_CHUNK_SIZE;
    memcpy(buffer, message, length);
  } else {
    memcpy(buffer, message, length);
  }

  REST.set_header_content_type(response, REST.type.TEXT_PLAIN); /* text/plain is the default, hence this option could be omitted. */
  REST.set_header_etag(response, (uint8_t *) &length, 1);
  REST.set_response_payload(response, buffer, length);
}
#endif

/******************************************************************************/
#if REST_RES_MIRROR
/* This resource mirrors the incoming request. It shows how to access the options and how to set them for the response. */
RESOURCE(mirror, METHOD_GET | METHOD_POST | METHOD_PUT | METHOD_DELETE, "debug/mirror", "title=\"Returns your decoded message\";rt=\"Debug\"");

void
mirror_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  /* The ETag and Token is copied to the header. */
  uint8_t opaque[] = {0x0A, 0xBC, 0xDE};

  /* Strings are not copied, so use static string buffers or strings in .text memory (char *str = "string in .text";). */
  static char location[] = {'/','f','/','a','?','k','&','e', 0};

  /* Getter for the header option Content-Type. If the option is not set, text/plain is returned by default. */
  unsigned int content_type = REST.get_header_content_type(request);

  /* The other getters copy the value (or string/array pointer) to the given pointers and return 1 for success or the length of strings/arrays. */
  uint32_t max_age_and_size = 0;
  const char *str = NULL;
  uint32_t observe = 0;
  const uint8_t *bytes = NULL;
  uint32_t block_num = 0;
  uint8_t block_more = 0;
  uint16_t block_size = 0;
  const char *query = "";
  int len = 0;

  /* Mirror the received header options in the response payload. Unsupported getters (e.g., rest_get_header_observe() with HTTP) will return 0. */

  int strpos = 0;
  /* snprintf() counts the terminating '\0' to the size parameter.
   * The additional byte is taken care of by allocating REST_MAX_CHUNK_SIZE+1 bytes in the REST framework.
   * Add +1 to fill the complete buffer, as the payload does not need a terminating '\0'. */
  if (content_type!=-1)
  {
    strpos += snprintf((char *)buffer, REST_MAX_CHUNK_SIZE+1, "CT %u\n", content_type);
  }
  
  /* Some getters such as for ETag or Location are omitted, as these options should not appear in a request.
   * Max-Age might appear in HTTP requests or used for special purposes in CoAP. */
  if (strpos<=REST_MAX_CHUNK_SIZE && REST.get_header_max_age(request, &max_age_and_size))
  {
    strpos += snprintf((char *)buffer+strpos, REST_MAX_CHUNK_SIZE-strpos+1, "MA %lu\n", max_age_and_size);
  }
  /* For HTTP this is the Length option, for CoAP it is the Size option. */
  if (strpos<=REST_MAX_CHUNK_SIZE && REST.get_header_length(request, &max_age_and_size))
  {
    strpos += snprintf((char *)buffer+strpos, REST_MAX_CHUNK_SIZE-strpos+1, "SZ %lu\n", max_age_and_size);
  }

  if (strpos<=REST_MAX_CHUNK_SIZE && (len = REST.get_header_host(request, &str)))
  {
    strpos += snprintf((char *)buffer+strpos, REST_MAX_CHUNK_SIZE-strpos+1, "UH %.*s\n", len, str);
  }

/* CoAP-specific example: actions not required for normal RESTful Web service. */
#if WITH_COAP > 1
  if (strpos<=REST_MAX_CHUNK_SIZE && coap_get_header_observe(request, &observe))
  {
    strpos += snprintf((char *)buffer+strpos, REST_MAX_CHUNK_SIZE-strpos+1, "Ob %lu\n", observe);
  }
  if (strpos<=REST_MAX_CHUNK_SIZE && (len = coap_get_header_token(request, &bytes)))
  {
    strpos += snprintf((char *)buffer+strpos, REST_MAX_CHUNK_SIZE-strpos+1, "To 0x");
    int index = 0;
    for (index = 0; index<len; ++index) {
        strpos += snprintf((char *)buffer+strpos, REST_MAX_CHUNK_SIZE-strpos+1, "%02X", bytes[index]);
    }
    strpos += snprintf((char *)buffer+strpos, REST_MAX_CHUNK_SIZE-strpos+1, "\n");
  }
  if (strpos<=REST_MAX_CHUNK_SIZE && (len = coap_get_header_etag(request, &bytes)))
  {
    strpos += snprintf((char *)buffer+strpos, REST_MAX_CHUNK_SIZE-strpos+1, "ET 0x");
    int index = 0;
    for (index = 0; index<len; ++index) {
        strpos += snprintf((char *)buffer+strpos, REST_MAX_CHUNK_SIZE-strpos+1, "%02X", bytes[index]);
    }
    strpos += snprintf((char *)buffer+strpos, REST_MAX_CHUNK_SIZE-strpos+1, "\n");
  }
  if (strpos<=REST_MAX_CHUNK_SIZE && (len = coap_get_header_uri_path(request, &str)))
  {
    strpos += snprintf((char *)buffer+strpos, REST_MAX_CHUNK_SIZE-strpos+1, "UP ");
    strpos += snprintf((char *)buffer+strpos, REST_MAX_CHUNK_SIZE-strpos+1, "%.*s\n", len, str);
  }
#if WITH_COAP == 3
  if (strpos<=REST_MAX_CHUNK_SIZE && (len = coap_get_header_location(request, &str)))
  {
    strpos += snprintf((char *)buffer+strpos, REST_MAX_CHUNK_SIZE-strpos+1, "Lo %.*s\n", len, str);
  }
  if (strpos<=REST_MAX_CHUNK_SIZE && coap_get_header_block(request, &block_num, &block_more, &block_size, NULL)) /* This getter allows NULL pointers to get only a subset of the block parameters. */
  {
    strpos += snprintf((char *)buffer+strpos, REST_MAX_CHUNK_SIZE-strpos+1, "Bl %lu%s (%u)\n", block_num, block_more ? "+" : "", block_size);
  }
#else
  if (strpos<=REST_MAX_CHUNK_SIZE && (len = coap_get_header_location_path(request, &str)))
  {
    strpos += snprintf((char *)buffer+strpos, REST_MAX_CHUNK_SIZE-strpos+1, "LP %.*s\n", len, str);
  }
  if (strpos<=REST_MAX_CHUNK_SIZE && (len = coap_get_header_location_query(request, &str)))
  {
    strpos += snprintf((char *)buffer+strpos, REST_MAX_CHUNK_SIZE-strpos+1, "LQ %.*s\n", len, str);
  }
  if (strpos<=REST_MAX_CHUNK_SIZE && coap_get_header_block2(request, &block_num, &block_more, &block_size, NULL)) /* This getter allows NULL pointers to get only a subset of the block parameters. */
  {
    strpos += snprintf((char *)buffer+strpos, REST_MAX_CHUNK_SIZE-strpos+1, "B2 %lu%s (%u)\n", block_num, block_more ? "+" : "", block_size);
  }
  /*
   * Critical Block1 option is currently rejected by engine.
   *
  if (strpos<=REST_MAX_CHUNK_SIZE && coap_get_header_block1(request, &block_num, &block_more, &block_size, NULL))
  {
    strpos += snprintf((char *)buffer+strpos, REST_MAX_CHUNK_SIZE-strpos+1, "B1 %lu%s (%u)\n", block_num, block_more ? "+" : "", block_size);
  }
  */
#endif /* CoAP > 03 */
#endif /* CoAP-specific example */

  if (strpos<=REST_MAX_CHUNK_SIZE && (len = REST.get_query(request, &query)))
  {
    strpos += snprintf((char *)buffer+strpos, REST_MAX_CHUNK_SIZE-strpos+1, "Qu %.*s\n", len, query);
  }
  if (strpos<=REST_MAX_CHUNK_SIZE && (len = REST.get_request_payload(request, &bytes)))
  {
    strpos += snprintf((char *)buffer+strpos, REST_MAX_CHUNK_SIZE-strpos+1, "%.*s", len, bytes);
  }

  if (strpos >= REST_MAX_CHUNK_SIZE)
  {
      buffer[REST_MAX_CHUNK_SIZE-1] = 0xBB; /* '»' to indicate truncation */
  }

  REST.set_response_payload(response, buffer, strpos);

  PRINTF("/mirror options received: %s\n", buffer);

  /* Set dummy header options for response. Like getters, some setters are not implemented for HTTP and have no effect. */
  REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
  REST.set_header_max_age(response, 17); /* For HTTP, browsers will not re-request the page for 17 seconds. */
  REST.set_header_etag(response, opaque, 2);
  REST.set_header_location(response, location); /* Initial slash is omitted by framework */
  REST.set_header_length(response, strpos); /* For HTTP, browsers will not re-request the page for 10 seconds. CoAP action depends on the client. */

/* CoAP-specific example: actions not required for normal RESTful Web service. */
#if WITH_COAP > 1
  coap_set_header_uri_host(response, "tiki");
  coap_set_header_observe(response, 10);
#if WITH_COAP == 3
  coap_set_header_block(response, 42, 0, 64); /* The block option might be overwritten by the framework when blockwise transfer is requested. */
#else
  coap_set_header_proxy_uri(response, "ftp://x");
  coap_set_header_block2(response, 42, 0, 64); /* The block option might be overwritten by the framework when blockwise transfer is requested. */
  coap_set_header_block1(response, 23, 0, 16);
  coap_set_header_accept(response, TEXT_PLAIN);
  coap_set_header_if_none_match(response);
#endif /* CoAP > 03 */
#endif /* CoAP-specific example */
}
#endif /* REST_RES_MIRROR */

/******************************************************************************/


typedef struct  {
	char id[4];                                 // "HDA\0"
	char station_name[5];                 // "TST\0\0"
	unsigned char sampling_rate[2]; // 100
	unsigned char comps;            // number of channels
	unsigned char date[3];          // 0, 0, 0
	unsigned char time[3];          // 0, 0, 0
	char latitude[11];              // 38,14.4648N
	char longitude[12];             // 021,43.8936E
	uint32_t block_cnt;           // 0
	unsigned char gps_cnt[4] ;                  // 0
	unsigned char pll_data[4];
	unsigned char adc[ 1 ][2];
	ext_adc_result ads1254_result[100];
	unsigned char crc[5];
	uint16_t nr_power_samples;
	uint16_t power_samples[31];
} packet_seismo;


//static packet_seismo packet_seism_t;

#if REST_RES_CHUNKS
/*
 * For data larger than REST_MAX_CHUNK_SIZE (e.g., stored in flash) resources must be aware of the buffer limitation
 * and split their responses by themselves. To transfer the complete resource through a TCP stream or CoAP's blockwise transfer,
 * the byte offset where to continue is provided to the handler as int32_t pointer.
 * These chunk-wise resources must set the offset value to its new position or -1 of the end is reached.
 * (The offset for CoAP's blockwise transfer can go up to 2'147'481'600 = ~2047 M for block size 2048 (reduced to 1024 in observe-03.)
 */
RESOURCE(chunks, METHOD_GET, "test/chunks", "title=\"Blockwise demo\";rt=\"Data\"");

#define CHUNKS_TOTAL    960

//static ext_adc_result ads1254_result1[100];

void chunks_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  
   int32_t strpos = 0;
   
	
	//seism_header temp_header;
    *packet_seism_t.id = "HDA\0";
	*packet_seism_t.station_name = "TST\0\0";
	*packet_seism_t.sampling_rate = 100;
	packet_seism_t.comps = 3;
	packet_seism_t.block_cnt =  0;
	
  /* Check the offset for boundaries of the resource data. */
  if (*offset>=CHUNKS_TOTAL)
  {
    REST.set_response_status(response, REST.status.BAD_OPTION);
    /* A block error message should not exceed the minimum block size (16). */

    const char *error_msg = "BlockOutOfScope";
    REST.set_response_payload(response, error_msg, strlen(error_msg));
    return;
  }



REST.set_header_content_type(response, REST.type.APPLICATION_X_OBIX_BINARY);
if(*offset==0){
	PRINTF("Started conversion within REST chunks, time is : %lu \r\n", clock_time());
	read_multiple_ads1254_result(&packet_seism_t.ads1254_result,100,10);
	while(!read_done_ads1254());
	PRINTF("Ended conversion within REST chunks, time is : %lu \r\n", clock_time());
}

	strpos = REST_MAX_CHUNK_SIZE;

	if (strpos > preferred_size)
		strpos = preferred_size;

	  
	// Truncate if above CHUNKS_TOTAL bytes. //
	if (*offset+(int32_t)strpos > CHUNKS_TOTAL)
		strpos = CHUNKS_TOTAL - *offset;
	
	PRINTF("offset = %i, strpos = %i  \r\n",*offset, strpos);
	
	REST.set_response_payload(response,&packet_seism_t,  strpos);

	
	// IMPORTANT for chunk-wise resources: Signal chunk awareness to REST engine. /
	*offset += strpos;

	// Signal end of resource representation. //
	if (*offset>=CHUNKS_TOTAL)
	{
		*offset = -1;
	}


  /* Generate data until reaching CHUNKS_TOTAL. */
/*  while (strpos<preferred_size)
  {
	  strpos += snprintf((char *)buffer+strpos, preferred_size-strpos+1, "|%ld|", *offset);
  }

  //* snprintf() does not adjust return value if truncated by size. 
  if (strpos > preferred_size)
  {
    strpos = preferred_size;
  }

  // Truncate if above CHUNKS_TOTAL bytes. //
  if (*offset+(int32_t)strpos > CHUNKS_TOTAL)
  {
    strpos = CHUNKS_TOTAL - *offset;
  }

  REST.set_response_payload(response, buffer, strpos);

  // IMPORTANT for chunk-wise resources: Signal chunk awareness to REST engine. /
  *offset += strpos;

  // Signal end of resource representation. //
  if (*offset>=CHUNKS_TOTAL)
  {
    *offset = -1;
  }
  */
}




#endif

/******************************************************************************/
#if REST_RES_SEPARATE && defined (PLATFORM_HAS_BUTTON) && WITH_COAP > 3
/* Required to manually (=not by the engine) handle the response transaction. */
#if WITH_COAP == 7
#include "er-coap-07-separate.h"
#include "er-coap-07-transactions.h"
#elif WITH_COAP == 12
#include "er-coap-12-separate.h"
#include "er-coap-12-transactions.h"
#elif WITH_COAP == 13
#include "er-coap-13-separate.h"
#include "er-coap-13-transactions.h"
#endif
/*
 * CoAP-specific example for separate responses.
 * Note the call "rest_set_pre_handler(&resource_separate, coap_separate_handler);" in the main process.
 * The pre-handler takes care of the empty ACK and updates the MID and message type for CON requests.
 * The resource handler must store all information that required to finalize the response later.
 */
RESOURCE(separate, METHOD_GET, "test/separate", "title=\"Separate demo\"");

/* A structure to store the required information */
typedef struct application_separate_store {
  /* Provided by Erbium to store generic request information such as remote address and token. */
  coap_separate_t request_metadata;
  /* Add fields for addition information to be stored for finalizing, e.g.: */
  char buffer[16];
} application_separate_store_t;

static uint8_t separate_active = 0;
static application_separate_store_t separate_store[1];

void
separate_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  /*
   * Example allows only one open separate response.
   * For multiple, the application must manage the list of stores.
   */
  if (separate_active)
  {
    coap_separate_reject();
  }
  else
  {
    separate_active = 1;

    /* Take over and skip response by engine. */
    coap_separate_accept(request, &separate_store->request_metadata);
    /* Be aware to respect the Block2 option, which is also stored in the coap_separate_t. */

    /*
     * At the moment, only the minimal information is stored in the store (client address, port, token, MID, type, and Block2).
     * Extend the store, if the application requires additional information from this handler.
     * buffer is an example field for custom information.
     */
    snprintf(separate_store->buffer, sizeof(separate_store->buffer), "StoredInfo");
  }
}

void
separate_finalize_handler()
{
  if (separate_active)
  {
    coap_transaction_t *transaction = NULL;
    if ( (transaction = coap_new_transaction(separate_store->request_metadata.mid, &separate_store->request_metadata.addr, separate_store->request_metadata.port)) )
    {
      coap_packet_t response[1]; /* This way the packet can be treated as pointer as usual. */

      /* Restore the request information for the response. */
      coap_separate_resume(response, &separate_store->request_metadata, REST.status.OK);

      coap_set_payload(response, separate_store->buffer, strlen(separate_store->buffer));

      /*
       * Be aware to respect the Block2 option, which is also stored in the coap_separate_t.
       * As it is a critical option, this example resource pretends to handle it for compliance.
       */
      coap_set_header_block2(response, separate_store->request_metadata.block2_num, 0, separate_store->request_metadata.block2_size);

      /* Warning: No check for serialization error. */
      transaction->packet_len = coap_serialize_message(response, transaction->packet);
      coap_send_transaction(transaction);
      /* The engine will clear the transaction (right after send for NON, after acked for CON). */

      separate_active = 0;
    }
    else
    {
      /*
       * Set timer for retry, send error message, ...
       * The example simply waits for another button press.
       */
    }
  } /* if (separate_active) */
}
#endif

/******************************************************************************/
#if REST_POWER_RES_PUSHING
/*
 * Example for a periodic resource.
 * It takes an additional period parameter, which defines the interval to call [name]_periodic_handler().
 * A default post_handler takes care of subscriptions by managing a list of subscribers to notify.
 */
PERIODIC_RESOURCE(power, METHOD_GET, "Modsense/power", "title=\"Power Measurements\";obs", 1*CLOCK_SECOND);

static uint8_t* data_buffer;
 				  
static struct{
  uint32_t block_cnt;
  uint16_t nr_power_samples;
  uint16_t power_samples[31];
}power_matrix;
				   
				   
void
power_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
	static struct HW_Object* app_system_time = NULL;
	static 	uint32_t time = 0;
	uint16_t i;
	size_t len = 0;
	char *cnt = NULL;
	uint32_t blockcount = 0;

	
	static uint16_t power_measurement=0 ;
	
	//	static uint32_t sd_read_offset = SD_START_OFFSET;
	//	static struct HW_Object* sd_mmc_device = NULL;
	
	data_buffer=buffer;
	
	power_measurement++;

	uint16_t *accept = NULL;
	int num = REST.get_header_accept(request, &accept);
	//		snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "%u", power_measurement);
	//		REST.set_response_payload(response, (uint8_t *)buffer, strlen((char *)buffer));
	
	if ((num==0) || (num && accept[0]==REST.type.TEXT_PLAIN))
	{
		if ((len=REST.get_query_variable(request, "cnt", &cnt))) {

			blockcount = atol(cnt);
			
			if(read_hashed_sd_buffer(buffer,blockcount))
			{
				packet_seismo* temp_pointer = buffer;
				if( temp_pointer->block_cnt==blockcount){
					REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
					ltoa(temp_pointer->power_samples[0],(char *)buffer,10);
					REST.set_response_payload(response, (uint8_t *)buffer, strlen((char *)buffer));
				}
				else {
					
					if(app_system_time==NULL)
					app_system_time = hal_get_hw_obj(rtc_ext,0);
					
					if(!app_system_time->started){
						hal_init_hw_obj(app_system_time);
						hal_start_hw_obj(app_system_time);
					}
					time = hal_system_time_read_sec_time(app_system_time);
					
					REST.set_response_status(response, REST.status.NOT_FOUND);
					
					ltoa(time,buffer,10);
					REST.set_response_payload(response, buffer, strlen((char *)buffer));
					
				}
			}
			else
			PRINTF("Error reading flash");
		}
		else {  //If len==0 so there is no blockcount attribute on GET, send the last second
			if(app_system_time==NULL)
			app_system_time = hal_get_hw_obj(rtc_ext,0);
			
			if(!app_system_time->started){
				hal_init_hw_obj(app_system_time);
				hal_start_hw_obj(app_system_time);
			}
			blockcount = hal_system_time_read_sec_time(app_system_time)-1;
			
			if(read_hashed_sd_buffer(buffer,blockcount))
			{
				packet_seismo* temp_pointer = buffer;
				
				if( temp_pointer->block_cnt==blockcount){
					REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
					
					ltoa(temp_pointer->power_samples[0],(char *)buffer,10);
					REST.set_response_payload(response, (uint8_t *)buffer, strlen((char *)buffer));
							
				}
				else
					REST.set_response_status(response, REST.status.NOT_FOUND);
			}
			
			//REST.set_response_status(response, REST.status.BAD_REQUEST);
			//const char *msg = "Accepting only with ?cnt=blockid query variable";
			//REST.set_response_payload(response, msg, strlen(msg));
		}
		
		
			//REST.set_response_status(response, REST.status.BAD_REQUEST);
			//const char *msg = "Accepting only with ?cnt=blockid query variable";
			//REST.set_response_payload(response, msg, strlen(msg));
			//ltoa(time,buffer,10);
			//REST.set_response_payload(response, buffer, strlen((char *)buffer));
	
	}
	else if (num && (accept[0]==REST.type.APPLICATION_XML))
	{
		REST.set_header_content_type(response, REST.type.APPLICATION_XML);
		snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "<power rms=\"%u\"", power_measurement);

		REST.set_response_payload(response, buffer, strlen((char *)buffer));
	}
	else if (num && (accept[0]==REST.type.APPLICATION_JSON))
	{
		REST.set_header_content_type(response, REST.type.APPLICATION_JSON);
		snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "{'power':{'rms':%u}}", power_measurement);

		REST.set_response_payload(response, buffer, strlen((char *)buffer));
	}
	else if (num && (accept[0]==REST.type.APPLICATION_X_OBIX_BINARY))
	{
		
		if ((len=REST.get_query_variable(request, "cnt", &cnt))) {

			blockcount = atol(cnt);
			
			if(read_hashed_sd_buffer(buffer,blockcount))
			{
				packet_seismo* temp_pointer = buffer;
				if( temp_pointer->block_cnt==blockcount){
					REST.set_header_content_type(response, REST.type.APPLICATION_X_OBIX_BINARY);
					REST.set_response_payload(response, temp_pointer->power_samples, temp_pointer->nr_power_samples*sizeof(uint16_t));
				}
				else {
					
					if(app_system_time==NULL)
					app_system_time = hal_get_hw_obj(rtc_ext,0);
					
					if(!app_system_time->started){
						hal_init_hw_obj(app_system_time);
						hal_start_hw_obj(app_system_time);
					}
					time = hal_system_time_read_sec_time(app_system_time);
					
					REST.set_response_status(response, REST.status.NOT_FOUND);
					
					ltoa(time,buffer,10);
					REST.set_response_payload(response, buffer, strlen((char *)buffer));
					
				}
			}
			else
			PRINTF("Error reading flash");
		}
		else {  //If len==0 so there is no blockcount attribute on GET, send the last second
			if(app_system_time==NULL)
			app_system_time = hal_get_hw_obj(rtc_ext,0);
			
			if(!app_system_time->started){
				hal_init_hw_obj(app_system_time);
				hal_start_hw_obj(app_system_time);
			}
			blockcount = hal_system_time_read_sec_time(app_system_time)-1;
			
			if(read_hashed_sd_buffer(buffer,blockcount))
			{
				packet_seismo* temp_pointer = buffer;
				
				if( temp_pointer->block_cnt==blockcount){
					REST.set_header_content_type(response, REST.type.APPLICATION_X_OBIX_BINARY);
					//REST.set_response_payload(response, temp_pointer->block_cnt, sizeof(uint32_t));
					
	/*				static struct{
						uint32_t block_cnt;
						uint16_t nr_power_samples;
						uint16_t power_samples[31];
						}power_matrix ;
	*/					
					power_matrix.block_cnt = temp_pointer->block_cnt;
					power_matrix.nr_power_samples = temp_pointer->nr_power_samples;
					 
					for(i=0;i<power_matrix.nr_power_samples ; i++)
						power_matrix.power_samples[i]=temp_pointer->power_samples[i];
						
					REST.set_response_payload(response, &power_matrix, (3+power_matrix.nr_power_samples)*sizeof(uint16_t));
				}
				else
					REST.set_response_status(response, REST.status.NOT_FOUND);
			}
			
			//REST.set_response_status(response, REST.status.BAD_REQUEST);
			//const char *msg = "Accepting only with ?cnt=blockid query variable";
			//REST.set_response_payload(response, msg, strlen(msg));
		}
	}
	else
	{
		REST.set_response_status(response, REST.status.NOT_ACCEPTABLE);
		const char *msg = "Supporting content-types text/plain, application/xml, and application/json";
		REST.set_response_payload(response, msg, strlen(msg));
	}
 
  /* A post_handler that handles subscriptions will be called for periodic resources by the REST framework. */
}

/*
 * Additionally, a handler function named [resource name]_handler must be implemented for each PERIODIC_RESOURCE.
 * It will be called by the REST manager process with the defined period.
 */
void
power_periodic_handler(resource_t *r)
{
	static struct HW_Object* push_system_time = NULL;
	uint32_t blockcount = 0;
  	uint16_t i=0;
	
	if(REST.coap_get_observers()){
	
		  /* Build notification. */
		  coap_packet_t notification[1]; /* This way the packet can be treated as pointer as usual. */
		  coap_init_message(notification, COAP_TYPE_NON, REST.status.OK, 0 );
		  //coap_set_payload(notification, content, snprintf(content, sizeof(content), "TICK %u", obs_counter));
	  	
		//Get system time in order to read latest flash readings
   		if(push_system_time==NULL)
   			push_system_time = hal_get_hw_obj(rtc_ext,0);
		if(!push_system_time->started){
		   hal_init_hw_obj(push_system_time);
		   hal_start_hw_obj(push_system_time);
		}
		//Read the time and set minus 1 to get the previous second
		blockcount = hal_system_time_read_sec_time(push_system_time)-1;
   
		//Read the SD card and send the appropriate power data.
		 if(read_hashed_sd_buffer(data_buffer,blockcount))  //change &packet_seism_t with data_buffer, WORKING ok.... so no need for extra buffer
		 {
			packet_seismo* temp_pointer = data_buffer;
		 
			 if( temp_pointer->block_cnt==blockcount)
			 {
				  REST.set_header_content_type(notification, REST.type.APPLICATION_X_OBIX_BINARY);
				  //REST.set_response_payload(response, temp_pointer->block_cnt, sizeof(uint32_t));
				  
	/*			  static struct{
					  uint32_t block_cnt;
					  uint16_t nr_power_samples;
					  uint16_t power_samples[31];
				  }power_matrix;
	*/			  
				  
				  power_matrix.block_cnt = temp_pointer->block_cnt;
				  power_matrix.nr_power_samples = temp_pointer->nr_power_samples;
				  
				  for(i=0;i<power_matrix.nr_power_samples ; i++)
				  power_matrix.power_samples[i]=temp_pointer->power_samples[i];
				  
				  REST.set_response_payload(notification, &power_matrix, (3+power_matrix.nr_power_samples)*sizeof(uint16_t));
				  
				  //REST.set_header_content_type(notification, REST.type.APPLICATION_X_OBIX_BINARY);
				 //REST.set_response_payload(notification, temp_pointer->power_samples, temp_pointer->nr_power_samples*sizeof(uint16_t));
			 }
			 else 
			 {
				 REST.set_response_status(notification, REST.status.NOT_FOUND);
				 //REST.set_response_payload(response, buffer, strlen((char *)buffer));
			 }
		 }
		 else
		 PRINTF("Error reading flash");


		//PRINTF("TICK %u for /%s\n", obs_counter, r->url);



		/* Notify the registered observers with the given message type, observe option, and payload. */
		 REST.notify_subscribers(r, blockcount, notification);
	}
}
#endif

/******************************************************************************/
#if REST_RES_PUSHING
/*
 * Example for a periodic resource.
 * It takes an additional period parameter, which defines the interval to call [name]_periodic_handler().
 * A default post_handler takes care of subscriptions by managing a list of subscribers to notify.
 */
PERIODIC_RESOURCE(pushing, METHOD_GET, "test/push", "title=\"Periodic demo\";obs", 1*CLOCK_SECOND);

void
pushing_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  REST.set_header_content_type(response, REST.type.TEXT_PLAIN);

  /* Usually, a CoAP server would response with the resource representation matching the periodic_handler. */
  const char *msg = "It's periodic!";
  REST.set_response_payload(response, msg, strlen(msg));
 
 //pushing_periodic_handler(&resource_pushing);
  /* A post_handler that handles subscriptions will be called for periodic resources by the REST framework. */
}

/*
 * Additionally, a handler function named [resource name]_handler must be implemented for each PERIODIC_RESOURCE.
 * It will be called by the REST manager process with the defined period.
 */
void
pushing_periodic_handler(resource_t *r)
{
  static uint16_t obs_counter = 0;
  static char content[11];

  ++obs_counter;

  //PRINTF("TICK %u for /%s\n", obs_counter, r->url);

  /* Build notification. */
  coap_packet_t notification[1]; /* This way the packet can be treated as pointer as usual. */
  coap_init_message(notification, COAP_TYPE_NON, REST.status.OK, 0 );
  coap_set_payload(notification, content, snprintf(content, sizeof(content), "TICK %u", obs_counter));

  /* Notify the registered observers with the given message type, observe option, and payload. */
  REST.notify_subscribers(r, obs_counter, notification);
}
#endif
/******************************************************************************/
#if REST_RES_EVENT && defined (PLATFORM_HAS_BUTTON)
/*
 * Example for an event resource.
 * Additionally takes a period parameter that defines the interval to call [name]_periodic_handler().
 * A default post_handler takes care of subscriptions and manages a list of subscribers to notify.
 */
EVENT_RESOURCE(event, METHOD_GET, "sensors/button", "title=\"Event demo\";obs");

void
event_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
  /* Usually, a CoAP server would response with the current resource representation. */
  const char *msg = "It's eventful!";
  REST.set_response_payload(response, (uint8_t *)msg, strlen(msg));

  /* A post_handler that handles subscriptions/observing will be called for periodic resources by the framework. */
}

/* Additionally, a handler function named [resource name]_event_handler must be implemented for each PERIODIC_RESOURCE defined.
 * It will be called by the REST manager process with the defined period. */
void
event_event_handler(resource_t *r)
{
  static uint16_t event_counter = 0;
  static char content[12];

  ++event_counter;

 // PRINTF("TICK %u for /%s\n", event_counter, r->url);

  /* Build notification. */
  coap_packet_t notification[1]; /* This way the packet can be treated as pointer as usual. */
  coap_init_message(notification, COAP_TYPE_CON, REST.status.OK, 0 );
  coap_set_payload(notification, content, snprintf(content, sizeof(content), "EVENT %u", event_counter));

  /* Notify the registered observers with the given message type, observe option, and payload. */
  REST.notify_subscribers(r, event_counter, notification);
}
#endif /* PLATFORM_HAS_BUTTON */

/******************************************************************************/
#if REST_RES_SUB
/*
 * Example for a resource that also handles all its sub-resources.
 * Use REST.get_url() to multiplex the handling of the request depending on the Uri-Path.
 */
RESOURCE(sub, METHOD_GET | HAS_SUB_RESOURCES, "test/path", "title=\"Sub-resource demo\"");

void
sub_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  REST.set_header_content_type(response, REST.type.TEXT_PLAIN);

  const char *uri_path = NULL;
  int len = REST.get_url(request, &uri_path);
  int base_len = strlen(resource_sub.url);

  if (len==base_len)
  {
	snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "Request any sub-resource of /%s", resource_sub.url);
  }
  else
  {
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, ".%.*s", len-base_len, uri_path+base_len);
  }

  REST.set_response_payload(response, buffer, strlen((char *)buffer));
}
#endif

/******************************************************************************/
#if defined (PLATFORM_HAS_LEDS)
/******************************************************************************/
#if REST_RES_LEDS
/*A simple actuator example, depending on the color query parameter and post variable mode, corresponding led is activated or deactivated*/
RESOURCE(leds, METHOD_POST | METHOD_PUT , "actuators/leds", "title=\"LEDs: ?color=r|g, POST/PUT mode=on|off\";rt=\"Control\"");

void
leds_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  size_t len = 0;
  const char *color = NULL;
  const char *mode = NULL;
  uint8_t led = 0;
  int success = 1;

  if ((len=REST.get_query_variable(request, "color", &color))) {
    PRINTF("color %.*s\n", len, color);

    if (strncmp(color, "r", len)==0) {
      led = LED_RED;
    } else if(strncmp(color,"g", len)==0) {
      led = LED_GREEN;
  //  } else if (strncmp(color,"b", len)==0) {
  //    led = LED_BLUE;
    } else {
      success = 0;
    }
  } else {
    success = 0;
  }


  if (success && (len=REST.get_post_variable(request, "mode", &mode))) {
    PRINTF("mode %s\n", mode);

    if (strncmp(mode, "on", len)==0) {
      LED_On(led);
    } else if (strncmp(mode, "off", len)==0) {
      LED_Off(led);
    } else {
      success = 0;
    }
  } else {
    success = 0;
  }

  if (!success) {
    REST.set_response_status(response, REST.status.BAD_REQUEST);
  }
}
#endif

/******************************************************************************/
#if REST_RES_TOGGLE
/* A simple actuator example. Toggles the red led */
RESOURCE(toggle, METHOD_POST, "actuators/toggle", "title=\"Red LED\";rt=\"Control\"");
void
toggle_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  LED_Toggle(LED_RED);
}
#endif
#endif /* PLATFORM_HAS_LEDS */

/******************************************************************************/
//#if REST_RES_LIGHT && defined (PLATFORM_HAS_LIGHT)
#if REST_RES_SEISMO

/* A simple getter example. Returns the reading from seismograph by reading the sd mmc for a packet*/
RESOURCE(seismo, METHOD_GET, "Modsense/seismograph", "title=\"Seismograph 100 3-channel readings\";rt=\"Binary\"");
void
seismo_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  //uint16_t light_photosynthetic = light_sensor.value(LIGHT_SENSOR_PHOTOSYNTHETIC);
  //uint16_t light_solar = light_sensor.value(LIGHT_SENSOR_TOTAL_SOLAR);
	static struct HW_Object* app_system_time = NULL;
	static 	uint32_t time = 0;
	uint16_t i=0;
	
	size_t len = 0;
	char *cnt = NULL;	 
	uint32_t blockcount = 0;

	 
	uint16_t light_photosynthetic ;
	uint16_t light_solar ;
//	static uint32_t sd_read_offset = SD_START_OFFSET;
//	static struct HW_Object* sd_mmc_device = NULL;
	
	light_photosynthetic=100;
	light_solar=150;

  uint16_t *accept = NULL;
  int num = REST.get_header_accept(request, &accept);

/*  if ((num==0) || (num && accept[0]==REST.type.TEXT_PLAIN))
  {
    REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "%u;%u", light_photosynthetic, light_solar);

    REST.set_response_payload(response, (uint8_t *)buffer, strlen((char *)buffer));
  }
  else if (num && (accept[0]==REST.type.APPLICATION_XML))
  {
    REST.set_header_content_type(response, REST.type.APPLICATION_XML);
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "<light photosynthetic=\"%u\" solar=\"%u\"/>", light_photosynthetic, light_solar);

    REST.set_response_payload(response, buffer, strlen((char *)buffer));
  }
  else if (num && (accept[0]==REST.type.APPLICATION_JSON))
  {
    REST.set_header_content_type(response, REST.type.APPLICATION_JSON);
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "{'light':{'photosynthetic':%u,'solar':%u}}", light_photosynthetic, light_solar);

    REST.set_response_payload(response, buffer, strlen((char *)buffer));
  }
 */
  
  /*else */ if (num && (accept[0]==REST.type.APPLICATION_X_OBIX_BINARY))
	{
	
	//method with cyclic bufffer
	//while(!read_sd_buffer(&packet_seism_t));
	
	//Updated method with index
	
	 if ((len=REST.get_query_variable(request, "cnt", &cnt))) {
		 //PRINTF("blockcount %.*s\n", len, cnt);
     
		// PRINTF(cnt);
		 blockcount = atol(cnt);
		 //PRINTF("= %lu \r\n",blockcount);
		 
		 //if(read_hashed_sd_buffer(&packet_seism_t,blockcount))
		 if(read_hashed_sd_buffer(buffer,blockcount))
		 {
			packet_seismo* temp_pointer = buffer;
			if( temp_pointer->block_cnt==blockcount){
				//PRINTF("read = %lu \r\n",packet_seism_t.block_cnt);
				REST.set_header_content_type(response, REST.type.APPLICATION_X_OBIX_BINARY);
				REST.set_response_payload(response, buffer, 960);
			}
			else { //packet not found in sd card
				 
				 //REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
				 //PRINTF("NOT found, stamp = %lu \r\n",packet_seism_t.block_cnt);
				 if(app_system_time==NULL)
					app_system_time = hal_get_hw_obj(rtc_ext,0);
				 
				 if(!app_system_time->started){
					 hal_init_hw_obj(app_system_time);
					 hal_start_hw_obj(app_system_time); 
				 }
				// if(time==0)
					//time = hal_system_time_read_sec_time(app_system_time) - clock_seconds();
					time = hal_system_time_read_sec_time(app_system_time);
				
				if((blockcount<time)&&((time-blockcount)<MAX_SECS_SDCARD)){
					calendar_date calend_struct;
					
					//packet within sd card zone, but not found , recreate a packet and send 0x800000s as answer
					sprintf(&temp_pointer->id,"HDA\0");
					//packet_seism_power_t.id = "HDA\0";
					sprintf(&temp_pointer->station_name, "TST\0\0");
					//packet_seism_power_t.station_name = "TST\0\0";
					*temp_pointer->sampling_rate = 100;
					temp_pointer->comps = 3;
					
					timestamp_to_calendar(blockcount,&calend_struct );
					
					temp_pointer->date[0]=calend_struct.date;
					temp_pointer->date[1]=calend_struct.month;
					temp_pointer->date[2]=calend_struct.year-2000;
					
					temp_pointer->time[2]=calend_struct.second;
					temp_pointer->time[1]=calend_struct.minute;
					temp_pointer->time[0]=calend_struct.hour;
					
					temp_pointer->block_cnt=blockcount;
					
					for(i=0;i<100;i++){
						memset(&(temp_pointer->ads1254_result[i]),0,9);
						temp_pointer->ads1254_result[i].data[2]=0x80;
						temp_pointer->ads1254_result[i].data[5]=0x80;
						temp_pointer->ads1254_result[i].data[8]=0x80;
					}
				
					REST.set_header_content_type(response, REST.type.APPLICATION_X_OBIX_BINARY);
					REST.set_response_payload(response, buffer, 960);
					
				}
				else{
				 
					REST.set_response_status(response, REST.status.NOT_FOUND);
					ltoa(time,buffer,10);
					REST.set_response_payload(response, buffer, strlen((char *)buffer));
				}
				
				
				 
			}
		 }
		 else 
			PRINTF("Error reading flash");
		 
	 }
	 else {
		 REST.set_response_status(response, REST.status.BAD_REQUEST);
		 const char *msg = "Accepting only with ?cnt=blockid query variable";
		 REST.set_response_payload(response, msg, strlen(msg));
	 }	
	
  }
  else
  {
    REST.set_response_status(response, REST.status.NOT_ACCEPTABLE);
    const char *msg = "Supporting binary content-type only";
    REST.set_response_payload(response, msg, strlen(msg));
  }
}
#endif /* PLATFORM_HAS_SEISMO */

//#if REST_RES_LIGHT && defined (PLATFORM_HAS_LIGHT)
#if REST_RES_POWER

/* A simple getter example. Returns the reading from seismograph by reading the sd mmc for a packet*/
RESOURCE(power, METHOD_GET, "sensors/power", "title=\"Power meter readings\";rt=\"Binary\"");
void
power_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{

	static struct HW_Object* app_system_time = NULL;
	static 	uint32_t time = 0;
	
	size_t len = 0;
	char *cnt = NULL;
	uint32_t blockcount = 0;

	
	static uint16_t power_measurement=0 ;
	
	//	static uint32_t sd_read_offset = SD_START_OFFSET;
	//	static struct HW_Object* sd_mmc_device = NULL;
	
	power_measurement++;

	uint16_t *accept = NULL;
	int num = REST.get_header_accept(request, &accept);

	if ((num==0) || (num && accept[0]==REST.type.TEXT_PLAIN))
	{
		REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
		snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "%u", power_measurement);

		REST.set_response_payload(response, (uint8_t *)buffer, strlen((char *)buffer));
	}
	else if (num && (accept[0]==REST.type.APPLICATION_XML))
	{
		REST.set_header_content_type(response, REST.type.APPLICATION_XML);
		snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "<power rms=\"%u\"", power_measurement);

		REST.set_response_payload(response, buffer, strlen((char *)buffer));
	}
	else if (num && (accept[0]==REST.type.APPLICATION_JSON))
	{
		REST.set_header_content_type(response, REST.type.APPLICATION_JSON);
		snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "{'power':{'rms':%u}}", power_measurement);

		REST.set_response_payload(response, buffer, strlen((char *)buffer));
	}
	else if (num && (accept[0]==REST.type.APPLICATION_X_OBIX_BINARY))
	{
		
		if ((len=REST.get_query_variable(request, "cnt", &cnt))) {

			blockcount = atol(cnt);
	
			if(read_hashed_sd_buffer(buffer,blockcount))
			{
				packet_seismo* temp_pointer = buffer;
				if( temp_pointer->block_cnt==blockcount){
					REST.set_header_content_type(response, REST.type.APPLICATION_X_OBIX_BINARY);
					REST.set_response_payload(response, temp_pointer->power_samples, temp_pointer->nr_power_samples*sizeof(uint16_t));
				}
				else {
					
					if(app_system_time==NULL)
					app_system_time = hal_get_hw_obj(rtc_ext,0);
					
					if(!app_system_time->started){
						hal_init_hw_obj(app_system_time);
						hal_start_hw_obj(app_system_time);
					}
					time = hal_system_time_read_sec_time(app_system_time);
					
					REST.set_response_status(response, REST.status.NOT_FOUND);
					
					ltoa(time,buffer,10);
					REST.set_response_payload(response, buffer, strlen((char *)buffer));
					
				}
			}
			else
			PRINTF("Error reading flash");
		}
		else {
			REST.set_response_status(response, REST.status.BAD_REQUEST);
			const char *msg = "Accepting only with ?cnt=blockid query variable";
			REST.set_response_payload(response, msg, strlen(msg));
		}	
	}
	else
	{
		REST.set_response_status(response, REST.status.NOT_ACCEPTABLE);
		const char *msg = "Supporting content-types text/plain, application/xml, and application/json";
		REST.set_response_payload(response, msg, strlen(msg));
	}
}
#endif /* REST_RES_POWER */

#if CONFIG_TIME
/*A simple config set/get parameter example*/
RESOURCE(set_time, METHOD_GET | METHOD_POST | METHOD_PUT , "Modsense/time", "title=\"Set_Time: POST/PUT time=unix timestamp\";rt=\"Config\"");


void set_time_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
	size_t len = 0;
	const char *char_time = NULL;
	uint32_t time = 0;
	static struct HW_Object* app_system_time = NULL;
	
	rest_resource_flags_t request_type = REST.get_method_type(request);
	
	if(request_type==METHOD_POST){
		if (len=REST.get_post_variable(request, "time", &char_time)) {
			
			//PRINTF("mode %s\n", char_time);
			
			time = atol(char_time);
			
			if(app_system_time==NULL)
				app_system_time = hal_get_hw_obj(rtc_ext,0);
			
			if(!app_system_time->started){
				hal_init_hw_obj(app_system_time);
				hal_start_hw_obj(app_system_time);
			}
			
			hal_system_time_write_time(app_system_time,time);
			
		}
		else
		REST.set_response_status(response, REST.status.BAD_REQUEST);
	}
	else if(request_type==METHOD_GET){
		
		if(app_system_time==NULL)
			app_system_time = hal_get_hw_obj(rtc_ext,0);
		
		if(!app_system_time->started){
			hal_init_hw_obj(app_system_time);
			hal_start_hw_obj(app_system_time);
		}
		time = hal_system_time_read_sec_time(app_system_time);
		
		REST.set_response_status(response, REST.status.OK);
		
		ltoa(time,buffer,10);
		REST.set_response_payload(response, buffer, strlen((char *)buffer));
	
	}
	
}
#endif  /*CONFIG_TIME */

/******************************************************************************/
#if REST_RES_BATTERY && defined (PLATFORM_HAS_BATTERY)
/* A simple getter example. Returns the reading from light sensor with a simple etag */
RESOURCE(battery, METHOD_GET, "sensors/battery", "title=\"Battery status\";rt=\"Battery\"");
void
battery_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  int battery = battery_sensor.value(0);

  const uint16_t *accept = NULL;
  int num = REST.get_header_accept(request, &accept);

  if ((num==0) || (num && accept[0]==REST.type.TEXT_PLAIN))
  {
    REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "%d", battery);

    REST.set_response_payload(response, (uint8_t *)buffer, strlen((char *)buffer));
  }
  else if (num && (accept[0]==REST.type.APPLICATION_JSON))
  {
    REST.set_header_content_type(response, REST.type.APPLICATION_JSON);
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "{'battery':%d}", battery);

    REST.set_response_payload(response, buffer, strlen((char *)buffer));
  }
  else
  {
    REST.set_response_status(response, REST.status.NOT_ACCEPTABLE);
    const char *msg = "Supporting content-types text/plain and application/json";
    REST.set_response_payload(response, msg, strlen(msg));
  }
}
#endif /* PLATFORM_HAS_BATTERY */


#if defined (PLATFORM_HAS_RADIO) && REST_RES_RADIO
/* A simple getter example. Returns the reading of the rssi/lqi from radio sensor */
RESOURCE(radio, METHOD_GET, "sensor/radio", "title=\"RADIO: ?p=lqi|rssi\";rt=\"RadioSensor\"");

void
radio_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  size_t len = 0;
  const char *p = NULL;
  uint8_t param = 0;
  int success = 1;

  const uint16_t *accept = NULL;
  int num = REST.get_header_accept(request, &accept);

  if ((len=REST.get_query_variable(request, "p", &p))) {
    PRINTF("p %.*s\n", len, p);
    if (strncmp(p, "lqi", len)==0) {
      param = RADIO_SENSOR_LAST_VALUE;
    } else if(strncmp(p,"rssi", len)==0) {
      param = RADIO_SENSOR_LAST_PACKET;
    } else {
      success = 0;
    }
  } else {
    success = 0;
  }

  if (success) {
    if ((num==0) || (num && accept[0]==REST.type.TEXT_PLAIN))
    {
      REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
      snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "%d", radio_sensor.value(param));

      REST.set_response_payload(response, (uint8_t *)buffer, strlen((char *)buffer));
    }
    else if (num && (accept[0]==REST.type.APPLICATION_JSON))
    {
      REST.set_header_content_type(response, REST.type.APPLICATION_JSON);

      if (param == RADIO_SENSOR_LAST_VALUE) {
        snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "{'lqi':%d}", radio_sensor.value(param));
      } else if (param == RADIO_SENSOR_LAST_PACKET) {
        snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "{'rssi':%d}", radio_sensor.value(param));
      }

      REST.set_response_payload(response, buffer, strlen((char *)buffer));
    }
    else
    {
      REST.set_response_status(response, REST.status.NOT_ACCEPTABLE);
      const char *msg = "Supporting content-types text/plain and application/json";
      REST.set_response_payload(response, msg, strlen(msg));
    }
  } else {
    REST.set_response_status(response, REST.status.BAD_REQUEST);
  }
}
#endif



PROCESS(rest_server_example, "Erbium Example Server");
AUTOSTART_PROCESSES(&rest_server_example);

PROCESS_THREAD(rest_server_example, ev, data)
{
  PROCESS_BEGIN();

  PRINTF("Starting Erbium Example Server\r\n");

#ifdef RF_CHANNEL
  PRINTF("RF channel: %u\r\n", RF_CHANNEL);
#endif
#ifdef IEEE802154_PANID
  PRINTF("PAN ID: 0x%04X\n", IEEE802154_PANID);
#endif

  PRINTF("uIP buffer: %u\r\n", UIP_BUFSIZE);
  PRINTF("LL header: %u\r\n", UIP_LLH_LEN);
  PRINTF("IP+UDP header: %u\r\n", UIP_IPUDPH_LEN);
  PRINTF("REST max chunk: %u\r\n", REST_MAX_CHUNK_SIZE);

  /* Initialize the REST engine. */
  rest_init_engine();

  /* Activate the application-specific resources. */
#if REST_RES_HELLO
  rest_activate_resource(&resource_helloworld);
#endif
#if REST_RES_MIRROR
  rest_activate_resource(&resource_mirror);
#endif
#if REST_RES_CHUNKS
  rest_activate_resource(&resource_chunks);
#endif

#if CONFIG_TIME
rest_activate_resource(&resource_set_time);
#endif

#if REST_RES_PUSHING
  rest_activate_periodic_resource(&periodic_resource_pushing);
#endif

#if REST_POWER_RES_PUSHING
rest_activate_periodic_resource(&periodic_resource_power);
#endif



#if defined (PLATFORM_HAS_BUTTON) && REST_RES_EVENT
  rest_activate_event_resource(&resource_event);
#endif
#if defined (PLATFORM_HAS_BUTTON) && REST_RES_SEPARATE && WITH_COAP > 3
  /* No pre-handler anymore, user coap_separate_accept() and coap_separate_reject(). */
  rest_activate_resource(&resource_separate);
#endif
#if defined (PLATFORM_HAS_BUTTON) && (REST_RES_EVENT || (REST_RES_SEPARATE && WITH_COAP > 3))
  SENSORS_ACTIVATE(button_sensor);
#endif
#if REST_RES_SUB
  rest_activate_resource(&resource_sub);
#endif
#if defined (PLATFORM_HAS_LEDS)
#if REST_RES_LEDS
  rest_activate_resource(&resource_leds);
#endif
#if REST_RES_TOGGLE
  rest_activate_resource(&resource_toggle);
#endif
#endif /* PLATFORM_HAS_LEDS */
//#if defined (PLATFORM_HAS_LIGHT) && REST_RES_LIGHT
#if REST_RES_SEISMO
  //SENSORS_ACTIVATE(light_sensor);
  rest_activate_resource(&resource_seismo);
#endif

#if REST_RES_POWER
//SENSORS_ACTIVATE(light_sensor);
rest_activate_resource(&resource_power);
#endif

#if defined (PLATFORM_HAS_BATTERY) && REST_RES_BATTERY
  SENSORS_ACTIVATE(battery_sensor);
  rest_activate_resource(&resource_battery);
#endif
#if defined (PLATFORM_HAS_RADIO) && REST_RES_RADIO
  SENSORS_ACTIVATE(radio_sensor);
  rest_activate_resource(&resource_radio);
#endif

  /* Define application-specific events here. */
  while(1) {
    PROCESS_WAIT_EVENT();
#if defined (PLATFORM_HAS_BUTTON)
    if (ev == sensors_event && data == &button_sensor) {
      PRINTF("BUTTON\n");
#if REST_RES_EVENT
      /* Call the event_handler for this application-specific event. */
      event_event_handler(&resource_event);
#endif
#if REST_RES_SEPARATE && WITH_COAP>3
      /* Also call the separate response example handler. */
      separate_finalize_handler();
#endif
    }
#endif /* PLATFORM_HAS_BUTTON */
  } /* while (1) */

  PROCESS_END();
}
