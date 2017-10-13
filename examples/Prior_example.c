/****************************************************************
 *  File: Prior_example.c
 *
 *  Description: Prior Implementation example
 
 *	OS Version: V1.3
 *  Date: 13/03/2015  
 *
 *	Author(s)					
 *  -----------------				
 *  D. van de Spijker	
 *	-----------------
 *
 * Copyright (c) D. van de Spijker, 2015
 *
 * This unpublished material is proprietary to D. van de Spijker.
 * All rights reserved. The methods and
 * techniques described herein are considered trade secrets
 * and/or confidential. Reproduction or distribution, in whole
 * or in part, is forbidden except by express written permission
 * of D. van de Spijker.
 *
 * DISCLAIMER: Modification of the Prior Lite kernel is at own risk. 
 * Because the software is licensed free of charge, there is no warranty
 * to the extent permitted by applicable law. The entire risk as to the
 * quality and performance of the Prior kernel is with you. 
 ****************************************************************/


/*
 * Prior_example.c
 *
 * Target MCU: ATMEGA644P
 */

/*				   Atmel ATMEGA644P
					+-----\/-----+
				PB0	|1	  	   40| PA0 ADC0		
				PB1	|2		   39| PA1 ADC1		
				PB2	|3	       38| PA2 ADC2		
				PB3	|4	       37| PA3 ADC3		
			 	PB4	|5		   36| PA4 ADC4		
		   MOSI	PB5	|6		   35| PA5 ADC5		
		  MISO	PB6	|7		   34| PA6 ADC6		
		  	SCK	PB7	|8		   33| PA7 ADC7		
			  RESET	|9		   32| AREF			FLOATING
				VCC	|10		   31| GND			GND
				GND	|11		   30| AVCC			+5V
16Mhz		 XTAL2	|12		   29| PC7 TOSC2	
16Mhz		 XTAL1	|13		   28| PC6 TOSC1	
			RX0	PD0	|14		   27| PC5 TDI		
			TX0	PD1	|15		   26| PC4 TDO		
				PD2	|16		   25| PC3 TMS		
				PD3	|17		   24| PC2 TCK		
		   0C1B PD4	|18		   23| PC1 SDA		
		   0C1A PD5	|19		   22| PC0 SCL		
		   0C2B PD6	|20		   21| PD7 OC2A
				    +------------+	
*/ 

#define F_CPU 20000000UL 


/*Libraries*/
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

/*Prior header*/
#include "Prior RTOS/Kernel/Prior_RTOS.h"
#include "Prior RTOS/Drivers/NRF24L01/nrf24.h"


/*Task prototypes*/
void AllocDemo	(void*);
void ReadTemp	(void*);
void MailReceive(void*);
void MailMaster	(void*);
void RenderUI(void*);
void UpdateUI(void*);
void ledblink1(void*);
void ShittySound(void*);
void NRF24_Transceive(void*);

U32  sin_T;
ID_t timer1,timer2,timer3,timer4;
ID_t mb1;
ID_t mail_event;

U8 rx_mac[5] = {0x6f, 0x6d, 0x6e, 0x69, 0x78}; //omnix
U8 tx_mac[5];
const U8 common_str[16] = {0x6f, 0x6d, 0x6e, 0x69,0x6f, 0x6d, 0x6e, 0x69,0x6f, 0x6d, 0x6e, 0x69,0x6f, 0x6d, 0x6e, 0x69};
U8 secret_str[16] = {};
U8 secret_str2[16] = {};
U8 common_key[16];
U8 common_key2[16];
U8 connection_status;



int main(void)
{
	
	DDRB |= (1 << PINB0) | (1 << PINB1) | (1 <<PINB2) | (1 << PINB3) | (1 << PINB4) | (1 << PINB5);

	
	/*Prior initiation*/
	os_Init();
	
	//nrf24_init();
	//nrf24_config(1,16); //channel 1, 128bit payload (16 bytes)
	
	/*Partition creation*/
	mb1 = mm_PoolCreate(20);	
	
	/*Event declarations*/
	mail_event = event_CreateGroup();
	
	/*Task declarations*/
	task_Create(NRF24_Transceive,realtime,2);
	task_Create(ShittySound,realtime,4);
	//task_Create(ReadTemp,realtime,5);
	task_Create(MailMaster,high,2);
	task_Create(MailReceive,high,3);
	task_Create(ledblink1,medium,1);
	task_Create(AllocDemo,medium,2);


	
	
	sin_T = 1500;

	
	/*SubTimer declarations*/
	timer1 = timer_Create(20000,0x03); //2000ms, Permanent, ON
	timer3 = timer_Create(10000,0x07); //
	timer2 = timer_Create(2000,0x07);
	timer4 = timer_Create(sin_T,0x07);
	
	/*Task registrations*/
	//task_Wait(MailReceive,mail_event,0x01);
	task_Wait(AllocDemo, timer1,0x00);
	task_Wait(NRF24_Transceive,timer3,0x00);
	//task_Wait(ShittySound,timer2,0x00);
	
	//PORTB |= (1 <<PINB1);
	/*MailBox assignments*/
	//mailbox_Assign(MailMaster,5);
	
	os_Start(ledblink1); // <= This should be the last function called in your main, 
							   // function calls or initializations following the StartPrior_RTOS function
							   // will be ignored and in fact never executed.			   
	/*NO USER CODE BELOW THIS POINT IN int main(void)*/
	
} //End main(void)


/*-----------------------USER DEFINED TASKS------------------------*/

void AllocDemo(void* parameter) 
{
	
	
	//Dynamic memory allocation 
	U32*	data32 = (U32*)mm_Alloc(mb1,4);
	U16*	data16 = (U16*)mm_Alloc(mb1,2);
	U8*		data8  = (U8*) mm_Alloc(mb1,1);
	
	*data32 = 0xBEEFBEEF;
	*data16 = 0xDEAD;
	*data8	= 0x18;
	
	mm_Free(mb1,data32);
	mm_Free(mb1,data16);
	
	U64* data64 = (U64*)mm_ReAlloc(mb1,data8,8);
	
	*data64 = 0x12345678DEADBEEF;
		
	//ID_t* pipe = (ID_t*)mm_Alloc(mb1,sizeof(ID_t)); 
//	*pipe = pipe_Open(8);
	//pipe_Write(*pipe,data64);
	mm_Free(mb1,data64);
	timer_Reset(timer1);
	//task_Wake(MailMaster,pipe);	
	
}

void MailMaster(void* parameter)
{
	ID_t* pipe = (ID_t*) parameter;			//Get pipe ID from task parameter
	mailbox_Post(0x00,*pipe);				//Post ID
	mm_Free(mb1,pipe);							//Free allocated memory for ID pointer
	event_GroupFlagsSet(mail_event,0x01);   //Set event flag
	event_Broadcast(mail_event);
}

void MailReceive(void* parameter)
{
	event_GroupFlagsClear(mail_event,0x01); //Clear event flag
	
	ID_t ID = mailbox_Pend(MailMaster,0x00); //Pend mail from MailMaster
	U64* data64 = (U64*)pipe_Read(ID); //Read from Pipe with received ID
	
	//Send over UART
	char buffer[20];
	itoa(data64,buffer,10);
	huart_StringSend(buffer);
	huart_StringSend("\n");
	

}



void ReadTemp(void* parameter) //Read temperature
{

	U32 raw = 0;
	
	for (U8 i = 0; i<10;i++)
	{
		adc_ReadChannel(0);
		raw += adc_ResultGet(0); //Read ADC channel
		_delay_ms(1);
	}
	
	raw = raw * 48.8;
	double Temp = raw / (double)102.3;
	U16 rTemp = Temp;
	
	//Send data over UART
	char buffer[10];
	itoa(rTemp,buffer,10);
	huart_StringSend(buffer);
	huart_StringSend("\n");
}

void ledblink1(void* parameter) //TOGGLE RED
{
	PORTB ^= (1 << PINB2); 	
	if (sin_T < 2000)
		sin_T+=100;
	else
		sin_T = 0;
		
	task_Sleep(1000);
}


void ShittySound(void* parameter)
{
	if ((PORTB & PINB1))
	{
		//task_Wait(NULL,timer2,0x00);
		PORTB &= ~(1 << PINB1); 
	}
	else
	{
		//task_Wait(NULL,timer4,0x00);
		timer_IntervalSet(timer4,sin_T);
		PORTB |= (1 << PINB1); 
	}
			
}

void NRF24_Transceive(void* parameter)
{
	connection_status = 0;
	if (connection_status == 0)
	{
		huart_StringSend("Key1\tKey2\n");
		U8 public_key[16] = {};
		U8 public_key2[16] = {};	
		
		timer_Reset(timer3);
		//nrf24_send(public_str);
	}
}

