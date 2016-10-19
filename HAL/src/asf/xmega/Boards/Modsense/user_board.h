/*
 * IncFile1.h
 *
 * Created: 3/10/2012 3:01:57 μμ
 *  Author: spilios
 */ 


#ifndef USER_BOARD_H_
#define USER_BOARD_H_

//Enablers of devices
#define GPS_ENABLER                     IOPORT_CREATE_PIN(PORTA, 4)
#define WIFI_ENABLER                    IOPORT_CREATE_PIN(PORTA, 5)

//Reset of Devices
#define WIFI_RESET						IOPORT_CREATE_PIN(PORTF, 7)
                    
//GPIO to receive the RTC Square wave , 1Hz, high-to-low enabled
#define RTC_SECONDS						IOPORT_CREATE_PIN(PORTB, 2)				

//GPIO to receive the GPS interrupt
#define GPS_SECOND						IOPORT_CREATE_PIN(PORTF, 0)

//Nice names for our Peripherals and the ports they use
#define LED_GREEN                        IOPORT_CREATE_PIN(PORTF, 4)
#define LED_RED                          IOPORT_CREATE_PIN(PORTB, 1)

//#define POWER_SENSOR					 IOPORT_CREATE_PIN(PORTA, 2)

#define GPS_USART						 USARTC0
#define WIFI_USART_1					 USARTC1
#define TO_PC_USART						 USARTD1
#define WIFI_USART_0					 USARTE0
#define ETHERNET_USART					 USARTF0

/**
 * \name External oscillator
 *
 * \todo Need to measure the actual startup time on the hardware.
 */
//@{
#define BOARD_XOSC_HZ                   11059200
//#define BOARD_XOSC_TYPE                 XOSC_TYPE_32KHZ
#define BOARD_XOSC_TYPE					XOSC_TYPE_XTAL  //spilios changed for our board
#define BOARD_XOSC_STARTUP_US           10
//@}

/*! \name SPI on Port C pins mapping
 */
//! @{
#define SPI_PORTC				&SPIC
//#define SPI_PORTC_CS            IOPORT_CREATE_PIN(PORTC,1)  // CS as output
#define SPI_PORTC_MASTER_SS     IOPORT_CREATE_PIN(PORTC,4)  // SS as output
#define SPI_PORTC_MASTER_SCK    IOPORT_CREATE_PIN(PORTC,7)  // SCK as output
#define SPI_PORTC_MASTER_MOSI   IOPORT_CREATE_PIN(PORTC,5)  // MOSI as output
#define SPI_PORTC_MASTER_MISO   IOPORT_CREATE_PIN(PORTC,6)  // MISO as input
#define SPI_PORTC_SPI_CLK_MASK  SYSCLK_PORT_C

//! @}

//! \name of SPI pins, for microSD on USART D0 acting in software spi mode
//@{
#define SD_MMC_SPI_MEM_CNT				1
#define SD_MMC_0_CD_GPIO                IOPORT_CREATE_PIN(PORTD, 5)
#define SD_MMC_0_CD_DETECT_VALUE        0
#define SD_MMC_SPI_USES_USART_SPI_SERVICE // To signal that is a USART in SPI mode
#define SD_MMC_SPI                       &USARTD0

#define SD_MMC_SPI_SCK                   IOPORT_CREATE_PIN(PORTD, 1)
#define SD_MMC_SPI_MISO                  IOPORT_CREATE_PIN(PORTD, 2)
#define SD_MMC_SPI_MOSI                  IOPORT_CREATE_PIN(PORTD, 3)
#define SD_MMC_SPI_0_CS                  IOPORT_CREATE_PIN(PORTD, 0)

//@}


/*! \ADS 1254 pin connections 
 *
 * ο ADC που έχουμε στον digitazer είναι ο ADS1254.
 * Επικοινωνεί μέσω SPI μέσω των pins 13, 14, 15, 16.
 * Οι λειτουργίες των pins αναλυτικά είναι οι παρακάτω.
 * 13 -> DOUT/notDRDY :  αρχικά notDRDY ότι υπάρχουν έτοιμα δεδομένα και μετά τα διαβάζουμε DOUT.
 * 14 -> SCLK : ρολόι
 * 15 -> CHSEL1 : επιλέγει 1 ένα από τα 4 αναλογικά κανάλια μαζί με το CHSEL0
 * 16 -> CHSEL0 : επιλέγει 1 ένα από τα 4 αναλογικά κανάλια μαζί με το CHSEL1.
 * 
 * Στον επεξεργαστή αυτά τα pins συνδέονται ως εξής:
 * το DOUT/notDRDY στο PE4 του επεξεργαστή 
 * το SCLK στο PE5 του επεξεργαστή 
 * το CHSEL1 στο PΑ1 του επεξεργαστή 
 * το CHSEL0 στο PΑ0 του επεξεργαστή
 */
//! @{
	
#define DOUT_DRDY                     IOPORT_CREATE_PIN(PORTE,4)	
#define SCLK                          IOPORT_CREATE_PIN(PORTE,5)
#define CHSEL0                        IOPORT_CREATE_PIN(PORTA,0)
#define CHSEL1                        IOPORT_CREATE_PIN(PORTA,1)	
	
//! @}


/*! \name I2C PORTC Connections of the external RTC chip
 */
//! @{
#define TWI_C_MASTER           &TWIC
#define TWI_C_MASTER_PORT      PORTC
#define I2C_SCL      IOPORT_CREATE_PIN(PORTC,0)  // SCL AS output
#define I2C_SDA      IOPORT_CREATE_PIN(PORTC,1)  // SDA for data communication

//! @}

/** Spilios Added 
 * \def CONF_BOARD_ENABLE_USARTC0
 * \brief Initialize IO pins for USART 0 on port C
 */
/*
#  if !defined(CONF_BOARD_ENABLE_USARTC0)
#    define CONF_BOARD_ENABLE_USARTC0
#  endif
*/


#endif /* USER_BOARD_H_ */