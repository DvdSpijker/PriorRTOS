/**********************************************************************************************************************************************
*  File: Prior_periph.c
*
*  Description: Prior Peripheral HAL
*
*  OS Version: V0.4
*  Date: 13/03/2015
*
*  Author(s)
*  -----------------
*  D. van de Spijker
*  -----------------
*
*
*  Copyright© 2017    D. van de Spijker
*
*  Permission is hereby granted, free of charge, to any person obtaining a copy
*  of this software AND associated documentation files (the "Software"), to deal
*  in the Software without restriction, including without limitation the rights to use,
*  copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
*  and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
*
*  1. Redistributions of source code must retain the above copyright notice,
*  this list of conditions and the following disclaimer.
*
*  2. The name of Prior RTOS may not be used to endorse or promote products derived
*    from this Software without specific prior written permission.
*
*  3. This Software may only be redistributed and used in connection with a
*    product in which Prior RTOS is integrated. Prior RTOS shall not be
*    distributed, under a different name or otherwise, as a standalone product.
*
*
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
*  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
*  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
*  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**********************************************************************************************************************************************/

#include <Peripheral.h>

/************OS Peripherals***********/


void HalInit(void)
{

}

//void adc_Init()
//{
//ADMUX = (1<<REFS0); // AREF = AVcc
//ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0); //ADC Enable and prescaler 128
//
////ID ADCResult_table = CreateMemPartition(2,mem_volatile);
//
//} //end InitADC
//
//
//
//#if ENABLE_SPI>0
//void hspi_Init(void)
//{
//DDRB |= (1 << PINB7) | (1 << PINB6);
//DDRB &= ~(1 << PINB5);
//SPCR0 |= (1 << SPE0) | (1 << MSTR0);
//}
//
//#endif
//
//void periph_Init (void)
//{
//
//}
//
//
///*Peripherals user API*/
//
////Analog-to-Digital Converter
//#if ENABLE_ADC>0
//
//U16_t ADCResult_table[8];
//
//void adc_ReadChannel(U8_t adc_channel)
//{
//// select the corresponding channel 0~7
//adc_channel &= 0b00000111;  // AND operation with 7
//ADMUX = (ADMUX & 0xF8)|adc_channel; //Select the channel with a safety mask
//ADCSRA |= (1<<ADSC); //Start single conversion, ADSC => 1
//while (ADCSRA & (1<<ADSC));
//ADCResult_table[adc_channel] = ADC;
//}
//
//U16_t adc_ResultGet(U8_t adc_channel)
//{
//return ADCResult_table[adc_channel];
//}
//
//#endif
//
//
////UART
//#if ENABLE_UART>0
//pRingbuf_t hUartRXBuf;
////pRingbuf_t hUartTXBuf;
//
//void hUartInit(unsigned int baud_rate)
//{
//UART_event =EventgroupCreate();
//hUartRXBuf = RingbufCreate(20);
//
//int UBBR_VALUE = (F_CPU/16/baud_rate - 1);
//UBRR0H = (U8_t)(UBBR_VALUE>>8);
//UBRR0L = (U8_t)UBBR_VALUE;
//UCSR0B = (1 << RXEN0) | (1 << TXEN0);
//UCSR0C = (1<<USBS0)|(3<<UCSZ00); //8 data bits, 2 stop bits
//UCSR0B |= (1 << RXCIE0); // Enable the USART Receive Complete interrupt (USART_RXC)
//}
//
//void hUartByteSend(U8_t u8Data)
//{
////wait while previous byte is completed
//while(!(UCSR0A&(1<<UDRE0)));
//// Transmit data
//UDR0 = u8Data;
//}
//
//void hUartStringSend(char* string)
//{
//int i = 0;
////wait while previous byte is completed
//while(string[i] != '\0') {
//hUartByteSend(string[i]);
//i++;
//}
//}
//
//unsigned char hUartByteReceive()
//{
//unsigned char byte;
//// Return received data
//if(!EventgroupFlagsGet(UART_event,0x01)) {
//return 0x00;
//} else {
//U16_t z = RingbufRead(hUartRXBuf,&byte,1);
//if(z) {
//EventgroupFlagsClear(UART_event,0x01);
//}
//}
//return byte;
//}
//
//ISR(USART0_RX_vect)
//{
//U16_t z = RingbufWrite(hUartRXBuf,&UDR0,1);
//if(!z) {
//EventgroupFlagsSet(UART_event,0x01);
//} else {
//EventgroupFlagsSet(UART_event,0x02);
//}
//}
//#endif
//
////SPI
//#if ENABLE_SPI >0
//U8_t hspi_ByteWrite(U8_t data_byte)
//{
//SPDR0 = data_byte;
//while(!(SPSR0 & (1 <<SPIF0)));
//return SPDR0;
//}
//#endif
//
//
////I2C
//
////TODO

/*******************************/
