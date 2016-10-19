/*
* ads1254.c
*
* Created: 13/2/2013 4:10:00 μμ
*  Author: spilios
*/ 


#include <asf.h>
#include <math.h>
#include "ads1254.h"
#include "hal_interface.h"
#include "led.h"
#include "definitions.h"

#define REMOVE_DC
#define ADS_ISR_PRIORITY PORT_INT0LVL_MED_gc

//static ext_adc_result ads_result_new;
uint32_t temp_result[4];

volatile bool read_ads1254_done = false;

ext_adc_result* result_local; //,*start_local;
uint16_t count_local;

bool conv_done = false;

struct HW_Object* HW_TIMER0 ;

int32_t remove_dc(int32_t input, uint8_t channel);
       
void read_ads1254_result(ext_adc_result* result){
       
       uint8_t temp=0;
       uint8_t i,j;
       irqflags_t flags;
       
       //Mark the start of the reading process
       read_ads1254_done = false;
       
       //Initialize ADS1254 and port E 
       PORTCFG.VPCTRLA=0x04; //Map port E to VP0
	   VPORT0.DIR|=0x20; // set direction pins, 0 is input, 1 is output, so i send 0010 0000  and 0000 1000 for TX of E0 usart= 0x20 SPILIOS CHANGED THIS
              
//       gpio_set_pin_high(SCLK);				//THIS
              
       gpio_set_pin_low(CHSEL0);
       gpio_set_pin_low(CHSEL1);         //change to channel0
              
//       delay_10us_step(2);  //wait 210μs   //THIS

//       gpio_set_pin_low(SCLK);
       
	   //Set to 0 all initial values of ADC result
	   for(i=0;i<9;i++)
	   	   result->data[i]=0;
	          
     
       //Setting GPIO interrupt on pin 4 of E port (DataReady pin)
       //
       //Task: Set up PORTE interrupt 0 as a high level interrupt, triggered by the falling edge of PinE4. 
       //1. Configure input/sense on pin0 to falling edge.
       //2. Write 0x10 to PORTC.INT0MASK to select PE4 as source for interrupt 0.
       //3. Set IN0LVL part of PORTC.INTCTRL to PORT_INT0LVL_MED_gc to enable interrupt 0 at high level.
       //4. Enable medium level interrupts in the PMIC.
       //5. Enable the global interrupt flag.
       
       
       flags = cpu_irq_save();
       PORTE.INT0MASK |=0x10;
       PORTE.PIN4CTRL |=PORT_ISC_FALLING_gc;
       PORTE.INTCTRL |= ADS_ISR_PRIORITY;
       //pmic_enable_level(PMIC_LVL_HIGH);
       cpu_irq_restore(flags);
              
       //Wait for the high level interrupt to take the measurements 

	   //delay_10us_step(200);
		
		//watchdog integer
		uint16_t timeout=10000; //could be less 
       {
		   bool temp_val;
		   do {
			   //delay_10us_step(50);
			   //flags = cpu_irq_save();
			   temp_val = read_ads1254_done;
			   //cpu_irq_restore(flags);
			   timeout--;
			   
		   }while((!temp_val)&&timeout);
	   }
		
//		if(timeout==0)
//			PRINTF("ERROR:ADS");
			
       //make post processing and store
	   for(i=0;i<3;i++){
              
#ifndef REMOVE_DC
              temp_result[i] = temp_result[i] + 0x800000;
#else
              
              temp_result[i] = remove_dc(temp_result[i],i);
              temp_result[i]= temp_result[i]+0x800000;
			//  if((temp_result[i]|0x00FFFFFF)==0)
			//	temp_result[i]=0x800000;
#endif 
     
              for(j=0;j<3;j++){
                     result->data[temp++]= temp_result[i];
                     temp_result[i]>>=8;
              }
       
       }
       
       return;
}

void init_ads1254(void){
#ifdef REMOVE_DC     
       PRINTF("INIT ASD1254 sampler,remove DC enabled\r\n");
#else 
       PRINTF("INIT ASD1254 sampler,remove DC disabled\r\n");
#endif 
       init_10us_step_counter();
}

void get_actual_readings(){
       
       conv_done=false;
       
       if(count_local--){
              //printf("%u\r\n",clock_time());
              read_ads1254_result(result_local++);
       }
       else{
       //     printf("DONE");
              conv_done=true;
       //     count_local=100;
       //     result_local=start_local;
              LED_Toggle(LED_RED);
              hal_stop_hw_obj(HW_TIMER0);
              hal_release_hw_obj(HW_TIMER0);
       }
}

void read_multiple_ads1254_result(ext_adc_result* result,uint16_t count,uint16_t period){

static struct hal_timer_config ADS_TIMER_OPTIONS;
ADS_TIMER_OPTIONS.callback=get_actual_readings;
ADS_TIMER_OPTIONS.period_timeout=period;
ADS_TIMER_OPTIONS.type=periodic;

HW_TIMER0 = hal_get_hw_obj(timer,0); //it returns a pointer to an unused timer, so no need to the int parameter
hal_init_hw_obj( HW_TIMER0);
hal_get_config_hw_obj( HW_TIMER0, &ADS_TIMER_OPTIONS);
hal_start_hw_obj(HW_TIMER0);

//Get ready for the interrupt callback routine
count_local = count;
result_local = result;
//start_local = result;

conv_done=false;

}


bool read_done_ads1254(void){
       return conv_done;
}


//funcion to filter DC out of every channel of the ads1235 
int32_t remove_dc(int32_t input, uint8_t channel){
              
       int32_t Ri = 0;
       //channel 0
       static int32_t C0_Ri_1 = 0;
       static int32_t C0_Ni_1 = 0;
       //channel 1
       static int32_t C1_Ri_1 = 0;
       static int32_t C1_Ni_1 = 0;
       //channel 2
       static int32_t C2_Ri_1 = 0;
       static int32_t C2_Ni_1 = 0;
       
       float Gain = 0.985;

       // Filter to remove DC offset, producing updated value of Real data
       if(channel==0)
       {                                                   
                     Ri = Gain * (float)C0_Ri_1 + (input - C0_Ni_1);
                     C0_Ni_1 = input;
                     C0_Ri_1 = Ri;
       }else if (channel==1){
                     Ri = Gain * (float)C1_Ri_1 + (input - C1_Ni_1);
                     C1_Ni_1 = input;
                     C1_Ri_1 = Ri;
       }else if (channel==2){
                     Ri = Gain * (float)C2_Ri_1 + (input - C2_Ni_1);
                     C2_Ni_1 = input;
                     C2_Ri_1 = Ri;
       }else
              return 0;
       
       return Ri;                  // new value without DC Offset

}

typedef enum  {
       
       READ_CHANNEL1,
       READ_CHANNEL2,
       READ_CHANNEL3,
       } ads1254_state ;
		
	static uint16_t counter=0;
	
ISR( PORTE_INT0_vect ) // Note that this vector name is a define mapped to the correct interrupt vector
{                     // See "board.h"
       
       static ads1254_state ads1254_state_t=READ_CHANNEL1;
       static uint8_t i=0;
       irqflags_t flags;
	   
	   uint8_t isr_temp_result[4];
       
       flags = cpu_irq_save();
	   
	   counter++;
	   //check the first bit to see if the result is positive or negative
/*       if(VPORT0.IN&0x10)   temp_result[i] = 0xffffffff;     
              else temp_result[i] = 0x00000000;
       
	   for(uint8_t j=0;j<24;j++){
              
              temp_result[i] <<= 1;                                            // << --> shift LEFT, ara diavazei apo MSBit --->  LSBit
              
              VPORT0.OUT |=0x20; //Sclock = 1
              
              if(VPORT0.IN&0x10)   temp_result[i] |= 0x00000001;  
                     else temp_result[i] &= 0xfffffffe;
                     
              VPORT0.OUT &=0xdf; //Sclock = 0  //new data comes out every falling edge 
       }
*/
	          if(VPORT0.IN&0x10)   isr_temp_result[0] = 0xff;
			  else isr_temp_result[0] = 0x00;
	          
	          for(uint8_t j=1;j<4;j++){
		          for(uint8_t k=0;k<8;k++){
					  isr_temp_result[j] <<= 1;                                            // << --> shift LEFT, ara diavazei apo MSBit --->  LSBit
		          
					  VPORT0.OUT |=0x20; //Sclock = 1
		          
					  if(VPORT0.IN&0x10)   isr_temp_result[j] |= 01;
					  else isr_temp_result[j] &= 0xfe;
		          
					  VPORT0.OUT &=0xdf; //Sclock = 0  //new data comes out every falling edge
				  }
			  }
			  

	   
	   cpu_irq_restore(flags);
       
	    for(uint8_t j=0;j<4;j++){
			temp_result[i]<<=8;
			temp_result[i]= temp_result[i]|isr_temp_result[j];
		}
	   
       VPORT0.OUT &=0xdf; //set low Sclock


       switch(ads1254_state_t) {
              case READ_CHANNEL1:
              ads1254_state_t=READ_CHANNEL2;
              //set multiplexer to channel2
              gpio_set_pin_high(CHSEL0);
              gpio_set_pin_low(CHSEL1);
              break;
              
              case READ_CHANNEL2:
              ads1254_state_t=READ_CHANNEL3;
              //set multiplexer to channel3
              gpio_set_pin_low(CHSEL0);
              gpio_set_pin_high(CHSEL1);
              break;
              
              case READ_CHANNEL3:
              ads1254_state_t=READ_CHANNEL1;
              //set multiplexer to channel4
              gpio_set_pin_high(CHSEL0);
              gpio_set_pin_high(CHSEL1);
			  
			  //Clear the enabled interrupt on DataReady Pin
			  flags = cpu_irq_save();
			  
			  //mark the end of the reading on all 3 channels
			  read_ads1254_done=true;
			  
			  PORTE.INT0MASK &=0xef;
			  PORTE.PIN4CTRL &=~PORT_ISC_FALLING_gc;
			  PORTE.INTCTRL &= ~ADS_ISR_PRIORITY; //0xfc;
			  //pmic_enable_level(PMIC_LVL_MEDIUM);
			  cpu_irq_restore(flags);
	          break;
              
              default:
              break;
       
       }
       //Increase i up to max 3 using modulo
       i= (++i)%3;
       
       
       //clear a possible interrupt flag on PORTE interrupt 0 due to data read, writing 1 clears the flag (strange but true according to manual)
       PORTE.INTFLAGS = 0x01;
       
}

uint16_t return_count(void){
	
	return counter;
}

uint16_t reset_count(void){
	
	counter=0;
}

/*

#include <asf.h>
#include <math.h>
#include "ads1254.h"
#include "hal_interface.h"
#include "led.h"
#include "definitions.h"

//#define REMOVE_DC

static ext_adc_result ads_result_new;
static bool read_ads1254_done = false;

ext_adc_result* result_local; //,*start_local;
uint16_t count_local;

bool conv_done = false;

struct HW_Object* HW_TIMER0 ;

int32_t remove_dc(int32_t input, uint8_t channel);
	
void read_ads1254_result(ext_adc_result* result){
	
	uint8_t temp=0;
	uint32_t temp_result[4];
	irqflags_t flags;
	
	PORTCFG.VPCTRLA=0x04; //Map port E to VP0
	VPORT0.DIR=0x20; // set direction pins, 0 is input, 1 is output, so i send 0010 0000 = 0x20
	
	gpio_set_pin_high(SCLK);
	
	gpio_set_pin_low(CHSEL0);
	gpio_set_pin_low(CHSEL1);		//change to channel0
	
	delay_10us_step(30);  //wait 210μs
	
	gpio_set_pin_low(SCLK);
	delay_10us_step(27);	//wait 270μs
	
	flags = cpu_irq_save();

	if((VPORT0.IN&0x10)==0x10)	temp_result[0] = 0xff;     // An DIN=1  =>  set ADC_data[i]=1.
		else temp_result[0] = 0x00;
	
	VPORT0.OUT |= 0x20; //Sclock = 1 
		
	for(uint8_t j=1;j<4;j++){
		temp_result[j]=0;
		for(uint8_t i=0;i<8;i++){
			
			temp_result[j] <<= 1;						   // << --> shift LEFT, diavazei apo MSBit --->  LSBit
			VPORT0.OUT &=0xdf;   //Sclock = 0 , new data to arrive after t9=60 ns
					
			if(VPORT0.IN&0x10)	temp_result[j] |= 0x01;
				else temp_result[j] &= 0xfe;
				
			VPORT0.OUT |=0x20; //Sclock = 1
		}
	}
	
	cpu_irq_restore(flags);
	
	VPORT0.OUT &=0xdf; //set low Sclock

	{

#ifndef REMOVE_DC		
		uint32_t adder = (temp_result[0]<<24 ^ temp_result[1]<<16 ^ temp_result[2]<<8 ^ temp_result[3]) + 0x800000;
#else 		
		int32_t adder1 = (temp_result[0]<<24 ^ temp_result[1]<<16 ^ temp_result[2]<<8 ^ temp_result[3]);
		adder1 = remove_dc(adder1,0);
		uint32_t adder= adder1+0x800000;
#endif	
		temp_result[3] = adder;
		temp_result[2] = adder>>8;
		temp_result[1] = adder>>16;
	}

	for(uint8_t j=3;j>0;j--){
		result->data[temp++]= temp_result[j];
	}

	gpio_set_pin_high(CHSEL0);		//change to channel 1
	delay_10us_step(28);	        //wait 278ns
	
	flags = cpu_irq_save();
	
	if((VPORT0.IN&0x10)==0x10)	temp_result[0] = 0xff;     // An DIN=1  =>  set ADC_data[i]=1.
	else temp_result[0] = 0x00;
	
//	VPORT0.OUT |= 0x20;   //Sclock = 1
	
	for(uint8_t j=1;j<4;j++){
		temp_result[j]=0;
		for(uint8_t i=0;i<8;i++){
			
			temp_result[j] <<= 1;						   // << --> shift LEFT, ara diavazei apo MSBit --->  LSBit
//			VPORT0.OUT &=0xdf;   //Sclock = 0
			VPORT0.OUT |= 0x20;
			
			if(VPORT0.IN&0x10)	temp_result[j] |= 0x01;
				else temp_result[j] &= 0xfe;
			
			VPORT0.OUT &=0xdf;   //Sclock = 0 //new data after 60ns
			
//			VPORT0.OUT |=0x20;  //Sclock = 1
			
		}
	}
	
	cpu_irq_restore(flags);
	
	VPORT0.OUT &=0xdf; //set low Sclock
	
	{

#ifndef REMOVE_DC
		uint32_t adder = (temp_result[0]<<24 ^ temp_result[1]<<16 ^ temp_result[2]<<8 ^ temp_result[3]) + 0x800000;
#else
		int32_t adder1 = (temp_result[0]<<24 ^ temp_result[1]<<16 ^ temp_result[2]<<8 ^ temp_result[3]);
		adder1 = remove_dc(adder1,1);
		uint32_t adder= adder1+0x800000;
#endif	
	
		temp_result[3] = adder;
		temp_result[2] = adder>>8;
		temp_result[1] = adder>>16;
	}
	
	for(uint8_t j=3;j>0;j--){
		result->data[temp++]= temp_result[j];
	}
	
	gpio_set_pin_low(CHSEL0);
	gpio_set_pin_high(CHSEL1);		//change to channel 2
	delay_10us_step(28);	        //wait 280ns
	
//	cpu_irq_disable();
    flags = cpu_irq_save();
	
		if((VPORT0.IN&0x10)==0x10)	temp_result[0] = 0xff;     // An DIN=1  =>  set ADC_data[i]=1.
		else temp_result[0] = 0x00;
		
		VPORT0.OUT |= 0x20;
		
		for(uint8_t j=1;j<4;j++){
			temp_result[j]=0;
			for(uint8_t i=0;i<8;i++){
				
				temp_result[j] <<= 1;						   // << --> shift LEFT, ara diavazei apo MSBit --->  LSBit
				VPORT0.OUT &=0xdf;
				
				if(VPORT0.IN&0x10)	temp_result[j] |= 0x01;
				else temp_result[j] &= 0xfe;
				
				VPORT0.OUT |=0x20;
				
			}
		}
		
    	cpu_irq_restore(flags);
		VPORT0.OUT &=0xdf; //set low Sclock
		
	{
#ifndef REMOVE_DC
		uint32_t adder = (temp_result[0]<<24 ^ temp_result[1]<<16 ^ temp_result[2]<<8 ^ temp_result[3]) + 0x800000;
#else
		int32_t adder1 = (temp_result[0]<<24 ^ temp_result[1]<<16 ^ temp_result[2]<<8 ^ temp_result[3]);
		adder1 = remove_dc(adder1,2);
		uint32_t adder= adder1+0x800000;
#endif	
		
		temp_result[3] = adder;
		temp_result[2] = adder>>8;
		temp_result[1] = adder>>16;
	}
	
	for(uint8_t j=3;j>0;j--){
		result->data[temp++]= temp_result[j];
	}
	
	return;
}

void init_ads1254(void){
#ifdef REMOVE_DC	
	PRINTF("INIT ASD1254 sampler,remove DC enabled\r\n");
#else 	
	PRINTF("INIT ASD1254 sampler,remove DC disabled\r\n");
#endif	
	init_10us_step_counter();
}

void get_actual_readings(){
	
	conv_done=false;
	
	if(count_local--){
		//printf("%u\r\n",clock_time());
		read_ads1254_result(result_local++);
	}
	else{
	//	printf("DONE");
		conv_done=true;
	//	count_local=100;
	//	result_local=start_local;
		LED_Toggle(LED_RED);
		hal_stop_hw_obj(HW_TIMER0);
		hal_release_hw_obj(HW_TIMER0);
	}
}

void read_multiple_ads1254_result(ext_adc_result* result,uint16_t count,uint16_t period){

struct hal_timer_config HW_TIMER_OPTIONS;
HW_TIMER_OPTIONS.callback=get_actual_readings;
HW_TIMER_OPTIONS.period_timeout=period;
HW_TIMER_OPTIONS.type=periodic;

HW_TIMER0 = hal_get_hw_obj(timer,0); //it returns a pointer to an unused timer, so no need to the int parameter
hal_init_hw_obj( HW_TIMER0);
hal_set_config_hw_obj( HW_TIMER0, &HW_TIMER_OPTIONS);
hal_start_hw_obj(HW_TIMER0);

//Get ready for the interrupt callback routine
count_local = count;
result_local = result;
//start_local = result;

conv_done=false;

}


bool read_done_ads1254(void){
	return conv_done;
}


//funcion to filter DC out of every channel of the ads1235 
int32_t remove_dc(int32_t input, uint8_t channel){
		
	int32_t Ri = 0;
	//channel 0
	static int32_t C0_Ri_1 = 0;
	static int32_t C0_Ni_1 = 0;
	//channel 1
	static int32_t C1_Ri_1 = 0;
	static int32_t C1_Ni_1 = 0;
	//channel 2
	static int32_t C2_Ri_1 = 0;
	static int32_t C2_Ni_1 = 0;
	
	float Gain = 0.985;

	// Filter to remove DC offset, producing updated value of Real data
	if(channel==0)
	{	                                             
			Ri = Gain * (float)C0_Ri_1 + (input - C0_Ni_1);
			C0_Ni_1 = input;
			C0_Ri_1 = Ri;
	}else if (channel==1){
			Ri = Gain * (float)C1_Ri_1 + (input - C1_Ni_1);
			C1_Ni_1 = input;
			C1_Ri_1 = Ri;
	}else if (channel==2){
			Ri = Gain * (float)C2_Ri_1 + (input - C2_Ni_1);
			C2_Ni_1 = input;
			C2_Ri_1 = Ri;
	}else
		 return 0;
	
	return Ri;                  // new value without DC Offset

}
*/



