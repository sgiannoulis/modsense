/**
 * \file
 *
 * \brief Example specific board configuration file
 *
 * Copyright (c) 2010 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */
#ifndef CONF_BOARD_H_INCLUDED
#define CONF_BOARD_H_INCLUDED


//#define BOARD USER_BOARD  //it is automatically selected from board.h 

//ALL of the below attributes are set from the my_board.h automatically if not manually set
#define CONF_BOARD_ENABLE_USARTC0 
#define CONF_BOARD_ENABLE_USARTC1
//#define CONF_BOARD_ENABLE_USARTD0  //it should not be active together with MMC_SPI
#define CONF_BOARD_ENABLE_USARTD1
#define CONF_BOARD_ENABLE_USARTF0
#define CONF_BOARD_ENABLE_USARTE0
//#define CONF_BOARD_ENABLE_USARTE1

//enable ads 1254 external adc
#define CONF_BOARD_ENABLE_ADS1254

//Enable external RTC via I2C C port bus
#define CONF_BOARD_ENABLE_EXT_RTC

//Enable USART D0 acting in software SPI mode
#define CONF_BOARD_SD_MMC_SPI

#define WIFI_ENABLED
#define GPS_ENABLED
#define LEDS_ENABLED
#define ENABLE_PORT_D_SPI


#endif /* CONF_BOARD_H_INCLUDED */
