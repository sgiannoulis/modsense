/**
 * \file
 *
 * \brief Modsense node init.
 *
 * This file contains board initialization function.
 *
 * Copyright (c) 2010 - 2012 Atmel Corporation. All rights reserved.
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
#include <conf_board.h>
#include <board.h>
#include <ioport.h>
/**
 * \addtogroup modesense node
 * @{
 */

void board_init(void)
{	
	//ioport_init();

	//Led Ports
	#ifdef LEDS_ENABLED
	ioport_configure_pin(LED_RED, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	ioport_configure_pin(LED_GREEN, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	#endif
	
	//Usart ports C0
	#ifdef CONF_BOARD_ENABLE_USARTC0	
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTC, 3), IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTC, 2), IOPORT_DIR_INPUT);
	//sysclk_enable_module(SYSCLK_PORT_C, SYSCLK_USART0);
	#endif
	
	//Usart ports C1
	#ifdef CONF_BOARD_ENABLE_USARTC1
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTC, 7), IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTC, 6), IOPORT_DIR_INPUT);
	//sysclk_enable_module(SYSCLK_PORT_C, SYSCLK_USART1);
	#endif
	
	//Usart ports D0
	#ifdef CONF_BOARD_ENABLE_USARTD0
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTD, 3), IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTD, 2), IOPORT_DIR_INPUT);
	//sysclk_enable_module(SYSCLK_PORT_D, SYSCLK_USART0);
	#endif
	
	//Usart ports D1
	#ifdef CONF_BOARD_ENABLE_USARTD1
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTD, 7), IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTD, 6), IOPORT_DIR_INPUT);
	//sysclk_enable_module(SYSCLK_PORT_D, SYSCLK_USART1);
	#endif
	
	//Usart ports E0
	#ifdef CONF_BOARD_ENABLE_USARTE0
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTE, 3), IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTE, 2), IOPORT_DIR_INPUT);
	//sysclk_enable_module(SYSCLK_PORT_D, SYSCLK_USART1);
	#endif
	
	//Usart ports F0
	#ifdef CONF_BOARD_ENABLE_USARTF0
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTF, 3), IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTF, 2), IOPORT_DIR_INPUT);
	//sysclk_enable_module(SYSCLK_PORT_F, SYSCLK_USART0);
	#endif
	
	//SPI port C initialize
	#ifdef ENABLE_PORT_C_SPI
	ioport_configure_pin(SPI_PORTC_MASTER_SCK, IOPORT_DIR_OUTPUT |
	IOPORT_INIT_HIGH);
	ioport_configure_pin(SPI_PORTC_MASTER_MOSI, IOPORT_DIR_OUTPUT |
	IOPORT_INIT_HIGH);
	ioport_configure_pin(SPI_PORTC_MASTER_MISO, IOPORT_DIR_INPUT);
	ioport_configure_pin(SPI_PORTC_CS, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	#endif

	//WIFI ENABLER
	#ifdef WIFI_ENABLED
	ioport_configure_pin(WIFI_ENABLER, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	ioport_configure_pin(WIFI_RESET, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	#endif
	
	//GPS ENABLER
	#ifdef GPS_ENABLED
	ioport_configure_pin(GPS_ENABLER, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	#endif
	
	//ADS1254 PINS INITIALIZATION
	#ifdef CONF_BOARD_ENABLE_ADS1254
	ioport_configure_pin(DOUT_DRDY, IOPORT_DIR_INPUT);
	//PORTE.PIN4CTRL = (0x30);
	ioport_configure_pin(SCLK, IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
	ioport_configure_pin(CHSEL0, IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
	ioport_configure_pin(CHSEL1, IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
	#endif
	
	#ifdef CONF_BOARD_SD_MMC_SPI
	ioport_configure_pin(SD_MMC_SPI_SCK, IOPORT_DIR_OUTPUT	| IOPORT_INIT_HIGH);
	ioport_configure_pin(SD_MMC_SPI_MOSI, IOPORT_DIR_OUTPUT	| IOPORT_INIT_HIGH);
	ioport_configure_pin(SD_MMC_SPI_MISO, IOPORT_DIR_INPUT);
	ioport_configure_pin(SD_MMC_SPI_0_CS, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	//	Card detection GPIO pin
	ioport_configure_pin(SD_MMC_0_CD_GPIO, IOPORT_DIR_INPUT| IOPORT_LEVEL | IOPORT_PULL_UP);
	#endif
		
	
	
	//External RTC enable communication via I2C on port C (DS3231 chip)
	#ifdef CONF_BOARD_ENABLE_EXT_RTC
	PORTC.PIN0CTRL = PORT_OPC_WIREDANDPULL_gc;
	PORTC.PIN1CTRL = PORT_OPC_WIREDANDPULL_gc;
	#endif
	
	
	
}

/**
 * @}
 */
