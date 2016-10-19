/*
 * hal_usart.c
 *
 * Created: 13/6/2013 4:43:04 μμ
 *  Author: spilios
 */ 

#include "hal_usart.h"
//#include "hal_interface.h"
#define USART_INTERRUPT_PRIORITY_BM PMIC_LVL_MEDIUM  //if changed must change accordingly the below define
#define USART_INTERRUPT_PRIORITY USART_INT_LVL_MED 

//#define USART_INTERRUPT_PRIORITY_BM PMIC_LVL_HIGH  //if changed must change accordingly the below define
//#define USART_INTERRUPT_PRIORITY USART_INT_LVL_HI


bool hal_usart_init(struct HW_Object* hw_dev){
	
	if(hw_dev==NULL)
		return false;
		
	//Must disable interrupts to properly initialize the usarts
	if(cpu_irq_is_enabled())
		cpu_irq_disable();
	
	//default usart attributes
	usart_config USART_SERIAL_OPTIONS = {
		.baudrate = 115200,
		.charlength = USART_CHSIZE_8BIT_gc,
		.paritytype = USART_PMODE_DISABLED_gc,
		.stopbits = false
	};	
	usart_init_rs232( (USART_t *)hw_dev->hw_pointer,&USART_SERIAL_OPTIONS);
	
	usart_set_rx_interrupt_level( (USART_t*) hw_dev->hw_pointer, USART_INTERRUPT_PRIORITY);
	USART_InterruptDriver_Initialize((USART_data_t*) &( ( (struct usart_hal_config*) hw_dev->config_struct)->com_data), (USART_t*)hw_dev->hw_pointer, USART_INTERRUPT_PRIORITY);
		
	//usart_init_rs232(&ETHERNET_USART, &USART_SERIAL_OPTIONS);
	//usart_set_rx_interrupt_level(WIFI_data.usart, USART_INT_LVL_LO);
	//USART_InterruptDriver_Initialize(&WIFI_data, &WIFI_USART_0, USART_DREINTLVL_LO_gc);			
	
	if(!cpu_irq_is_enabled())
	cpu_irq_enable();
	
	return true;
}

bool hal_usart_set_config(struct HW_Object* hw_dev, usart_config* config){
	
	if(hw_dev==NULL)
		return false;
		
	//Must disable interrupts to properly initialize the usarts
//	if(cpu_irq_is_enabled())
//		cpu_irq_disable();
	
	((struct usart_hal_config*) hw_dev->config_struct)->baudrate = config->baudrate;
	((struct usart_hal_config*) hw_dev->config_struct)->charlength = config->charlength;
	((struct usart_hal_config*) hw_dev->config_struct)->paritytype = config->paritytype;
	((struct usart_hal_config*) hw_dev->config_struct)->stopbits = config->stopbits;
	
//	if(!cpu_irq_is_enabled())
//	cpu_irq_enable();
	
	return usart_init_rs232( (USART_t *)hw_dev->hw_pointer,(usart_config*) hw_dev->config_struct);
}

bool hal_usart_get_config(struct HW_Object* hw_dev, usart_config* config){
		
	if(hw_dev==NULL || config==NULL)
		return false;
		
	config->baudrate = ((struct usart_hal_config*) hw_dev->config_struct)->baudrate ;
	config->charlength = ((struct usart_hal_config*) hw_dev->config_struct)->charlength ;
	config->paritytype = ((struct usart_hal_config*) hw_dev->config_struct)->paritytype ;
	config->stopbits = ((struct usart_hal_config*) hw_dev->config_struct)->stopbits ;
	
	return true;
}


inline bool hal_usart_send_byte(struct HW_Object* hw_dev, uint8_t byte_data){
	
/*	if(USART_TXBuffer_FreeSpace( (USART_data_t *) &( ( (struct usart_hal_config*) hw_dev->config_struct)->com_data) ) ){
			USART_TXBuffer_PutByte((USART_data_t *) &( ((struct usart_hal_config*) hw_dev->config_struct)->com_data), byte_data);
			return true;
	}			
	else 
		return false;
		*/
	
	//if device not started, return false
	if(!(hw_dev->started))
		return false;
	
	while(!USART_TXBuffer_FreeSpace( (USART_data_t *) &( ( (struct usart_hal_config*) hw_dev->config_struct)->com_data) ) );
	
	if(USART_TXBuffer_FreeSpace( (USART_data_t *) &( ( (struct usart_hal_config*) hw_dev->config_struct)->com_data) ) ){
		USART_TXBuffer_PutByte((USART_data_t *) &( ((struct usart_hal_config*) hw_dev->config_struct)->com_data), byte_data);
		return true;
	}
	else
		return false;

}

inline bool hal_usart_receive_byte(struct HW_Object* hw_dev,uint8_t* data){
	
	//if device not started, return false
	if(!hw_dev->started)
		return false;
			
	if( USART_RXBufferData_Available( (USART_data_t *) &( ( (struct usart_hal_config*) hw_dev->config_struct)->com_data)) ){
		*data =  USART_RXBuffer_GetByte( (USART_data_t *) &( ( (struct usart_hal_config*) hw_dev->config_struct)->com_data) );
		return true;
		}
	else 
		return false;
}

bool hal_usart_start(struct HW_Object* hw_dev){
	
	if(hw_dev->started)
		return true;	
	
	hw_dev->started=true;
	
	usart_set_rx_interrupt_level( (USART_t*) hw_dev->hw_pointer, USART_INTERRUPT_PRIORITY);
	usart_set_dre_interrupt_level((USART_t*) hw_dev->hw_pointer, USART_INTERRUPT_PRIORITY);
			
		if(!pmic_level_is_enabled(USART_INTERRUPT_PRIORITY_BM))
			pmic_enable_level(USART_INTERRUPT_PRIORITY_BM);
		if(!cpu_irq_is_enabled())
			cpu_irq_enable();
	
	
	return true;
}

bool hal_usart_stop(struct HW_Object* hw_dev){
	
	hw_dev->started=false;
	usart_set_rx_interrupt_level( (USART_t*) hw_dev->hw_pointer, USART_INT_LVL_OFF);
	usart_set_dre_interrupt_level((USART_t*) hw_dev->hw_pointer, USART_INT_LVL_OFF);
	
	return true;
}


bool hal_usart_send_string(struct HW_Object* hw_device, uint8_t* string, uint16_t size){
	
	bool res=true;
	for(int i=0;i<size;i++)
		res&=hal_usart_send_byte(hw_device,string[i]);
	
	return res;
}

uint8_t hal_usart_receive_string(struct HW_Object* hw_device, uint8_t* buffer, uint16_t size){
	
	uint8_t temp=1;
	uint8_t i=0;
	bool res;
	
	while(i<size&&temp!=0){
		res = hal_usart_receive_byte(hw_device,&temp);
		if(res==false)
			break;
		buffer[i++] = temp;
	}
	
	return i;

/*	for(int i=0;i<size;i++){
			temp=hal_usart_receive_byte(hw_device);
	
		buffer[i] = temp;
		temp=0;
	}
	return true;*/
}



//! Interrupt Service Routines for USARTE0

//  Receive complete interrupt service routine.
//  Calls the common receive complete handler with pointer to the correct USART
//  as argument.
ISR( USARTE0_RXC_vect ) // Note that this vector name is a define mapped to the correct interrupt vector
{                     // See "board.h"
	
	if(global_hw_registry.COM4.started)
		USART_RXComplete( (USART_data_t *) &( ( (struct usart_hal_config*) global_hw_registry.COM4.config_struct)->com_data) );
	
	//USART_RXComplete( &WIFI_data );
}

//  Data register empty  interrupt service routine.
//  Calls the common data register empty complete handler with pointer to the
//  correct USART as argument.

ISR( USARTE0_DRE_vect ) // Note that this vector name is a define mapped to the correct interrupt vector
{                     // See "board.h"
	if(global_hw_registry.COM4.started)
		USART_DataRegEmpty((USART_data_t *) &( ( (struct usart_hal_config*) global_hw_registry.COM4.config_struct)->com_data) );

}



//! Interrupt Service Routines for USARTC0

//  Receive complete interrupt service routine.
//  Calls the common receive complete handler with pointer to the correct USART
//  as argument.
ISR( USARTC0_RXC_vect ) // Note that this vector name is a define mapped to the correct interrupt vector
{                     // See "board.h"
	if(global_hw_registry.COM0.started)
		USART_RXComplete((USART_data_t *) &( ( (struct usart_hal_config*) global_hw_registry.COM0.config_struct)->com_data) );
		
}

//  Data register empty  interrupt service routine.
//  Calls the common data register empty complete handler with pointer to the
//  correct USART as argument.

ISR( USARTC0_DRE_vect ) // Note that this vector name is a define mapped to the correct interrupt vector
{                     // See "board.h"
	if(global_hw_registry.COM0.started)
		USART_DataRegEmpty( (USART_data_t *) &( ( (struct usart_hal_config*) global_hw_registry.COM0.config_struct)->com_data) );
}


//! Interrupt Service Routines for USARTF0

//  Receive complete interrupt service routine.
//  Calls the common receive complete handler with pointer to the correct USART
//  as argument.
ISR( USARTF0_RXC_vect ) // Note that this vector name is a define mapped to the correct interrupt vector
{                     // See "board.h"
	if(global_hw_registry.COM5.started)
		USART_RXComplete((USART_data_t *) &( ( (struct usart_hal_config*) global_hw_registry.COM5.config_struct)->com_data) );
}

//  Data register empty  interrupt service routine.
//  Calls the common data register empty complete handler with pointer to the
//  correct USART as argument.

ISR( USARTF0_DRE_vect ) // Note that this vector name is a define mapped to the correct interrupt vector
{                     // See "board.h"
	if(global_hw_registry.COM5.started)
		USART_DataRegEmpty( (USART_data_t *) &( ( (struct usart_hal_config*) global_hw_registry.COM5.config_struct)->com_data) );
}

//! Interrupt Service Routines for USARTC1

//  Receive complete interrupt service routine.
//  Calls the common receive complete handler with pointer to the correct USART
//  as argument.
ISR( USARTC1_RXC_vect ) // Note that this vector name is a define mapped to the correct interrupt vector
{                     // See "board.h"
	if(global_hw_registry.COM1.started)
		USART_RXComplete((USART_data_t *) &( ( (struct usart_hal_config*) global_hw_registry.COM1.config_struct)->com_data) );
}
//  Data register empty  interrupt service routine.
//  Calls the common data register empty complete handler with pointer to the
//  correct USART as argument.

ISR( USARTC1_DRE_vect ) // Note that this vector name is a define mapped to the correct interrupt vector
{                     // See "board.h"
	if(global_hw_registry.COM1.started)
		USART_DataRegEmpty( (USART_data_t *) &( ( (struct usart_hal_config*) global_hw_registry.COM1.config_struct)->com_data) );
}


//! Interrupt Service Routines for USARTD0
//  Receive complete interrupt service routine.
//  Calls the common receive complete handler with pointer to the correct USART
//  as argument.
ISR( USARTD0_RXC_vect ) // Note that this vector name is a define mapped to the correct interrupt vector
{                     // See "board.h"
	if(global_hw_registry.COM2.started)
		USART_RXComplete((USART_data_t *) &( ( (struct usart_hal_config*) global_hw_registry.COM2.config_struct)->com_data) );
}

//  Data register empty  interrupt service routine.
//  Calls the common data register empty complete handler with pointer to the
//  correct USART as argument.

ISR( USARTD0_DRE_vect ) // Note that this vector name is a define mapped to the correct interrupt vector
{                     // See "board.h"
	if(global_hw_registry.COM2.started)
		USART_DataRegEmpty( (USART_data_t *) &( ( (struct usart_hal_config*) global_hw_registry.COM2.config_struct)->com_data) );
}



//! Interrupt Service Routines for USARTD1
//  Receive complete interrupt service routine.
//  Calls the common receive complete handler with pointer to the correct USART
//  as argument.
ISR( USARTD1_RXC_vect ) // Note that this vector name is a define mapped to the correct interrupt vector
{                     // See "board.h"
	if(global_hw_registry.COM3.started)
		USART_RXComplete((USART_data_t *) &( ( (struct usart_hal_config*) global_hw_registry.COM3.config_struct)->com_data) );
}
//  Data register empty  interrupt service routine.
//  Calls the common data register empty complete handler with pointer to the
//  correct USART as argument.

ISR( USARTD1_DRE_vect ) // Note that this vector name is a define mapped to the correct interrupt vector
{                     // See "board.h"
	if(global_hw_registry.COM3.started)
		USART_DataRegEmpty( (USART_data_t *) &( ( (struct usart_hal_config*) global_hw_registry.COM3.config_struct)->com_data) );
}

