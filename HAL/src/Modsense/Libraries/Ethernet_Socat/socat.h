﻿/** \addtogroup lib
 * @{ */

/**
 * \defgroup Socat Ethernet-socat framing protocol
 *
 * @{
 */

#ifndef ETH_SOC_H_
#define ETH_SOC_H_

#include "compiler.h"
#include "uip.h"

#include "definitions.h"
//#include <asf.h>

/* Put a character on the serial device.
*
* This function is used by the SLIP implementation to put a character
* on the serial device. It must be implemented specifically for the
* system on which the SLIP implementation is to be run.
*
* \param c The character to be put on the serial device.
*/

inline void soc_eth_char_put(uint8_t c);

/**
* Poll the serial device for a character.
*
* This function is used by the socat implementation to poll the serial
* device for a character. 
*
* The function should return immediately regardless if a character is
* available or not. If a character is available it should be placed
* at the memory location pointed to by the pointer supplied by the
* argument c.
*
* \param c A pointer to a byte that is filled in by the function with
* the received character, if available.
*
* \retval 0 If no character is available.
* \retval Non-zero If a character is available.
*/
inline uint8_t soc_eth_char_poll(uint8_t *c);


void soc_eth_init(void);

uint8_t soc_eth_packet_send(uint8_t* buffer, uint16_t len);
uint16_t soc_eth_packet_poll(uint8_t* buffer);

/*

void slipdev_char_put(struct HW_Object* hw_com, uint8_t c);
uint8_t slipdev_char_poll(struct HW_Object* hw_com, uint8_t *c);


void slipdev_init(void);
uint8_t slipdev_send(struct HW_Object* hw_com,uint8_t* buffer, uint16_t len);
uint16_t slipdev_poll(struct HW_Object* hw_com, uint8_t* buffer);

*/

#endif /* ETH_SOC_H_ */

/** @} */

/** @} */
