/**
 * \file
 *
 * \brief RTC32 configuration
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
#ifndef CONF_RTC32_H
#define CONF_RTC32_H

#define RTC32_COMPARE_INT_LEVEL RTC32_COMPINTLVL_LO_gc

#define CONFIG_RTC_CLOCK_SOURCE       CLK_RTCSRC_TOSC32_gc

/************************************************************************/
/*    CLK_RTCSRC_ULP_gc = (0x00<<1),  /* 1.024 kHz from internal 32kHz ULP 
CLK_RTCSRC_TOSC_gc = (0x01<<1),  /* 1.024 kHz from 32.768 kHz crystal oscillator on TOSC 
CLK_RTCSRC_RCOSC_gc = (0x02<<1),  /* 1.024 kHz from internal 32.768 kHz RC oscillator 
CLK_RTCSRC_TOSC32_gc = (0x05<<1),  /* 32.768 kHz from 32.768 kHz crystal oscillator on TOSC <-------------------------------------------
CLK_RTCSRC_RCOSC32_gc = (0x06<<1),  /* 32.768 kHz from internal 32.768 kHz RC oscillator 
CLK_RTCSRC_EXTCLK_gc = (0x07<<1),  /* External Clock from TOSC1 
                                                                     
/************************************************************************/

#define CONFIG_RTC32_CLOCK_1024HZ

#endif /* CONF_RTC32_H */