/*
 * Prior_example.c
 *
 * Target MCU: ATMEGA644P
 * Author: D. van de Spijker
 */

/*                 Atmel ATMEGA644P
                    +-----\/-----+
                PB0 |1         40| PA0 ADC0
                PB1 |2         39| PA1 ADC1
                PB2 |3         38| PA2 ADC2
                PB3 |4         37| PA3 ADC3
                PB4 |5         36| PA4 ADC4
           MOSI PB5 |6         35| PA5 ADC5
          MISO  PB6 |7         34| PA6 ADC6
            SCK PB7 |8         33| PA7 ADC7
              RESET |9         32| AREF         FLOATING
                VCC |10        31| GND          GND
                GND |11        30| AVCC         +5V
20Mhz        XTAL2  |12        29| PC7 TOSC2
20Mhz        XTAL1  |13        28| PC6 TOSC1
            RX0 PD0 |14        27| PC5 TDI
            TX0 PD1 |15        26| PC4 TDO
                PD2 |16        25| PC3 TMS
                PD3 |17        24| PC2 TCK
           0C1B PD4 |18        23| PC1 SDA
           0C1A PD5 |19        22| PC0 SCL
           0C2B PD6 |20        21| PD7 OC2A
                    +------------+
*/

#define F_CPU 16000000UL


/*Libraries*/
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdio.h>

/*Prior RTOS header*/
#include "PriorRTOS/Include/Prior_RTOS.h"
#include "PriorRTOS/Include/List.h"
#include "PriorRTOS/Include/Conversion.h"
#include "PriorRTOS/Include/Prior_logger.h"
#include "PriorRTOS/Include/uart.h"
//#include "pwmgen.h"

/*GLCD Drivers*/
//#include "Prior RTOS/Drivers/GLCD_ST7920/st7920.h"
//#include "Prior RTOS/Drivers/GLCD_ST7920/graphics.h"
//#include "arial12.h"
//#include "Prior RTOS/Drivers/GLCD_ST7920/hal.h"

/*Task prototypes*/

void RenderUI(void*);
void UpdateUI(void*);
void BlinkGreen(void*);
void BlinkRed(void*);
void BlinkBlue(void*);
void UnitTest(void*);
void comUART(void*);
void RingbufTest(void*);
void PipeTest(void*);
void MailboxTest(void*);
void ItcTest(void*);


#define TASK_ITC_TEST_OP_MODE_MAILBOX 0x00
#define TASK_ITC_TEST_OP_MODE_PIPE    0x01
#define TASK_ITC_TEST_OP_MODE_RINGBUF 0x02




Id_t timer1,timer2,timer3;
Id_t pool0,pool1;
Id_t mailbox0;

Id_t new_event;

Id_t timers[10];

char rx_buf[20];
U8_t rx_i=0;
extern OsResult_t MmPoolAllocTableCreate(Id_t pool_ID); //Do not use
extern OsResult_t MmPoolAllocTableAdd(void* ptr, Id_t pool_ID); //Do not use
extern  U16_t MmGeneratePoolCRC(Id_t pool_ID); //Do not use
U16_t crc0,crc1,crc2;

int main(void)
{

    /*Prior initiation*/
    OsResult_t opt_status;
    OsResult_t k_status;

    GpioPinModeSet(&DDRB, 0, Output);
    GpioPinModeSet(&DDRB, 1, Output);
    GpioPinModeSet(&DDRB, 2, Output);

    char buffer[15];
    k_status = OsInit(&opt_status);
    if(k_status == e_error) {
        return 0;
    }

    crc0=crc1=crc2=0xFFFF;

    pool0 = MmPoolCreate(40); //Create pool with a size of 20 bytes
    //crc0 = MmGeneratePoolCRC(pool0);

    //pool1 = MmPoolCreate(30); //Create a second pool with a size of 30 bytes

    /*Event declarations*/

    /*Task declarations - highest priority first - descending*/
    //new_event = EventgroupCreate();

    //TaskCreate(comUART,cat_realtime,4,NULL); //TaskGenericNameSet(comUART,"comUART");
    TaskCreate(BlinkRed, TASK_CAT_MEDIUM, 5, NULL); //TaskGenericNameSet(BlinkRed,"BlinkRed");
    //TaskCreate(RenderUI,cat_high,1);
    //TaskCreate(UpdateUI,cat_high,1);
    TaskCreate(BlinkBlue, TASK_CAT_HIGH, 1, NULL); //TaskGenericNameSet(BlinkBlue,"BlinkBlue");
    TaskCreate(BlinkGreen, TASK_CAT_REALTIME, 5, NULL);// TaskGenericNameSet(BlinkGreen,"BlinkGreen");
    TaskCreate(RingbufTest, TASK_CAT_HIGH, 5, NULL); //TaskGenericNameSet(RingbufTest,"RingbufTest");
    //TaskCreate(PipeTest, TASK_CAT_HIGH, 5, NULL); //TaskGenericNameSet(PipeTest,"PipeTest");
    TaskCreate(MailboxTest, TASK_CAT_HIGH, 5, NULL); //TaskGenericNameSet(MailboxTest,"MailboxTest");
    //TaskCreate(UnitTest,cat_high,2,NULL);  //TaskGenericNameSet(UnitTest,"UnitTest");
//
//
    //mailbox0 = mailbox_Create(10,3); //10 elements, 3 owners max.
    //if(mailbox_OwnerAdd(mailbox0,comUART) == e_ok) //Add comUART as owner
    //{
    //TaskWake(comUART,NULL);
    //}

    /*Timer declarations*/
    timer1 = TimerCreate(100e3, (TIMER_PARAMETER_AR | TIMER_PARAMETER_ON | TIMER_PARAMETER_ITR_SET(3))); //ON, AR, Permanent
    timer2 = TimerCreate(250e3, (TIMER_PARAMETER_P | TIMER_PARAMETER_AR | TIMER_PARAMETER_ON)); //ON, AR, Permanent
    timer3 = TimerCreate(50e3, (TIMER_PARAMETER_P | TIMER_PARAMETER_AR | TIMER_PARAMETER_ON)); //ON, AR, Permanent
    //for (U8_t i = 0; i < 10; i++) {
    //timers[i] = TimerCreate((5e5 + i * 1e5), (TIMER_PARAMETER_P | TIMER_PARAMETER_AR | TIMER_PARAMETER_ON));
    //}

    /*MailBox assignments*/

    //TaskWait(UpdateUI,timer1,0x00);

    TaskWake(BlinkGreen, NULL);
    TaskWake(BlinkRed, NULL);
    TaskWake(BlinkBlue, NULL);

    OsStart(RingbufTest); /* <= This should be the last function called in main(),
                                function calls or initializations below this point
                                will be ignored.
                                NO USER CODE BELOW THIS POINT
                        */
} //End main(void)


/*-----------------------USER DEFINED TASKS------------------------*/
void comUART(void* parameter)
{
    //MailboxWidth_t tosend[6];
    //OsResult_t status = mailbox_Pend(tosend,0x00,5);
    //tosend[5] = '\0';
    //if(status == e_ok)
    //{
    //UartSendString(tosend);
    //}
    //
    TaskSleep(50);
}

void BlinkGreen(void* parameter)
{

    //GpioPinStateSet(&PORTB, 1, Toggle);

    LogDebug(LOG_LINE_NEW, "user", "Blink green");
    TaskWait(NULL, timer1, 0x00, 0);
    //TaskSleep(200);
}


void BlinkBlue(void* parameter)
{
    //GpioPinStateSet(&PORTB, 0, Toggle);

    LogDebug(LOG_LINE_NEW, "user", "Blink blue");
    //TaskSleep(100);
    TaskWait(NULL, timer2, 0x00, 0);
    //TaskDelete(NULL);
}


void BlinkRed(void* parameter)
{
    //GpioPinStateSet(&PORTB, 2, Toggle);
    LogDebug(LOG_LINE_NEW, "user", "Blink red");
    //TaskSleep(50);
    TaskWait(NULL, timer3, 0x00, 0);
}

void ItcTest(void *parameter)
{
    U8_t* op_mode = (U8_t*)(parameter);

    switch(*op_mode) {
    case TASK_ITC_TEST_OP_MODE_MAILBOX:

        break;
    }

}

void RingbufTest(void *parameter)
{
    /* Creation and test array declaration. */
    Id_t ringbuffer = RingbufCreate(20);
    if(ringbuffer == INV_ID) {
        LogDebug(LOG_LINE_NEW, "ringbuftest", "Failed to created ringbuffer.");
        goto task_exit;
    } else {
        LogDebug(LOG_LINE_NEW, "ringbuftest", "Ringbuffer created.");
    }
    U8_t test_array[6] = {'t', 'e', 's', 't', 'x', '\0'};


    /* Write test. Write 1 too much. */
    U8_t write_count = 0;
    for (U8_t i = 0; i < 4; i++) {
        test_array[4] = i + 48;
        write_count = RingbufWrite(ringbuffer, test_array, 5);
        if(write_count < 5) {
            LogDebug(LOG_LINE_NEW, "ringbuftest", "Write incomplete: %d", write_count);
            break;
        } else {
            LogDebug(LOG_LINE_NEW, "ringbuftest", "Write complete: %d", write_count);
        }
    }


    /* Read test. Read 1 too much. */
    U8_t read_count = 0;
    for (U8_t i = 0; i < 4; i++) {
        read_count = RingbufRead(ringbuffer, test_array, 5);
        if(read_count < 5) {
            test_array[read_count] = '\0';
            LogDebug(LOG_LINE_NEW, "ringbuftest", "Read incomplete: %d | Read: %s", read_count, test_array);
            break;
        } else {
            LogDebug(LOG_LINE_NEW, "ringbuftest", "Read complete: %d | Read: %s", read_count, test_array);
        }
    }

    write_count = RingbufWrite(ringbuffer, test_array, 5);
    if(write_count < 5) {
        LogDebug(LOG_LINE_NEW, "ringbuftest", "Write incomplete: %d", write_count);
    } else {
        LogDebug(LOG_LINE_NEW, "ringbuftest", "Write complete: %d", write_count);
    }

    /* Cleanup. */
    OsResult_t result = e_error;
    result = RingbufDelete(&ringbuffer);
    if(result != e_ok) {
        LogDebug(LOG_LINE_NEW, "ringbuftest", "Failed to delete ringbuffer.");
    } else {
        LogDebug(LOG_LINE_NEW, "ringbuftest", "Ringbuffer deleted.");
    }

task_exit:
    TaskWake(MailboxTest, NULL);
    TaskDelete(NULL);
}

void PipeTest(void *parameter)
{
    /* Creation and test array declaration. */
    Id_t pipe = PipeCreate(5);
    if(pipe == INV_ID) {
        LogDebug(LOG_LINE_NEW, "pipetest", "Failed to created pipe.");
        goto task_exit;
    } else {
        LogDebug(LOG_LINE_NEW, "pipetest", "Pipe created.");
    }
    U8_t test_array[4] = {'t', 'e', 's', 't'};


    /* Write test. */
    OsResult_t result = e_ok;

    /* Attempt to write with pipe closed. */
    result = PipeWrite(pipe, test_array, 1); /* Expected result = e_locked. */
    char result_buf[10] = {};
    ConvertResultToString(result, result_buf);
    LogDebug(LOG_LINE_NEW, "pipetest", "Write result: %s", result_buf);

    PipeOpen(pipe);
    LogDebug(LOG_LINE_NEW, "pipetest", "Open count: %d", PipeOpenCountGet(pipe));

    for (U8_t i = 0; i < 4; i++) {
        result = PipeWrite(pipe, (test_array+i), 1);
        if(result != e_ok) {
            char result_buf[10] = {};
            ConvertResultToString(result, result_buf);
            LogDebug(LOG_LINE_NEW, "pipetest", "Write failed: %s", result_buf);
            break;
        } else {
            LogDebug(LOG_LINE_NEW, "pipetest", "Write successful.");
        }
    }


    /* Read test. */

    /* Cleanup. */
    PipeClose(pipe);
    PipeDelete(&pipe);

task_exit:
    TaskDelete(NULL);
}

void MailboxTest(void *parameter)
{
    /* Creation and test array declaration. */
    Task_t owner_task = MailboxTest;
    Id_t mailbox = MailboxCreate(10, &owner_task, 1);

    if(mailbox == INV_ID) {
        LogDebug(LOG_LINE_NEW, "mailboxtest", "Failed to created mailbox.");
        goto task_exit;
    } else {
        LogDebug(LOG_LINE_NEW, "mailboxtest", "Mailbox created.");
    }
    U8_t test_array[4] = {'t', 'e', 's', 't'};


    /* 1st Post test. */
    OsResult_t result = e_ok;

    result = MailboxPost(mailbox, 0x00, test_array, 4); /* Expected result = e_ok. */
    char result_buf[15] = {};
    ConvertResultToString(result, result_buf);
    LogDebug(LOG_LINE_NEW, "mailboxtest", "Post result: %s", result_buf);

    LogDebug(LOG_LINE_NEW, "mailboxtest", "Pend counter at address %u: %u", 0x00, MailboxPendCounterGet(mailbox, 0x00));

    /* Attempt to overwrite */
    result = MailboxPost(mailbox, 0x00, test_array, 4); /* Expected result = e_locked. */
    ConvertResultToString(result, result_buf);
    LogDebug(LOG_LINE_NEW, "mailboxtest", "Post result: %s", result_buf);

    /* 2nd Post test. */
    result = MailboxPost(mailbox, 0x04, test_array, 4); /* Expected result = e_ok. */
    ConvertResultToString(result, result_buf);
    LogDebug(LOG_LINE_NEW, "mailboxtest", "Post result: %s", result_buf);

    LogDebug(LOG_LINE_NEW, "mailboxtest", "Pend counter at address %u: %u", 0x04, MailboxPendCounterGet(mailbox, 0x00));

    /* Read test. */
    result = MailboxPend(mailbox, test_array, 0x00, 4); /* Expected result = e_ok. */
    ConvertResultToString(result, result_buf);
    LogDebug(LOG_LINE_NEW, "mailboxtest", "Pend result: %s", result_buf);

    LogDebug(LOG_LINE_NEW, "mailboxtest", "Pend counter at address %u: %u", 0x00, MailboxPendCounterGet(mailbox, 0x00));

    /* Cleanup. */
    result = MailboxDelete(&mailbox);
    if(result != e_ok) {
        LogDebug(LOG_LINE_NEW, "mailboxtest", "Failed to delete mailbox.");
    } else {
        LogDebug(LOG_LINE_NEW, "mailboxtest", "Mailbox deleted.");
    }
task_exit:
    //TaskYield;
    TaskDelete(NULL);
}


void UnitTest(void* parameter)
{
    OsResult_t result = TaskWaitNew(NULL, timer2, TIMER_EVENT_OVERFLOW, 3000);
}

//void RenderUI(void* parameter)
//{
//PORTD &= ~(1 << EN);
//DDRD |= (1 << RS) | (1 << RW) | (1 << EN) | (1 << PSB) | (1 << RST);
//PORTD |= (1 << PSB);
///* data bus pins are outputs */
///* pullups enabled on all pins */
//
//DDRB=  0xFF;
//
//st7920_init();
//_delay_ms(100);
//st7920_set_mode(invert);
//st7920_clear();
//
////Borders
//gfx_hline(0,0,127);
//gfx_hline(0,63,127);
//gfx_vline(0,0,63);
//gfx_vline(127,0,63);
////Info bar
//gfx_hline(1,5,126);
//
////st7920_putchar('b',20,20);
//
////Security System
//gfx_rect(50,10,20,20);
//
//gfx_fillrect(56,20,66,24);
//gfx_fillrect(55,21,55,23);
//gfx_line(61,19,57,15);
//gfx_line(62,19,58,15);
//gfx_line(57,14,61,12);
//gfx_line(58,14,62,12);
//
//
////Next button
//gfx_rect(100,10,20,20);
////triangle
//gfx_hline(105,20,115);
//gfx_line(110,15,115,20);
//gfx_line(110,25,115,20);
//
////Network button
//gfx_rect(75,10,20,20);
//gfx_hline(81,20,89);
//gfx_vline(85,17,19);
//gfx_vline(81,21,23);
//gfx_vline(89,21,23);
//gfx_fillrect(80,24,82,26);
//gfx_fillrect(88,24,90,26);
//gfx_fillrect(84,14,86,16);
//
//
////Power button
//gfx_rect(100,36,20,20);
//gfx_circle(104,40,12,12,6);
//gfx_fillrect(108,41,112,41);
//gfx_vline(110,103,110);
//
////Settings button
//gfx_rect(75,36,20,20);
//gfx_vline(80,40,52);
//gfx_vline(85,40,52);
//gfx_vline(90,40,52);
//gfx_rect(79,49,2,2);
//gfx_rect(84,43,2,2);
//gfx_rect(89,47,2,2);
//
//TimerStart(timer1);
//}
//
//void UpdateUI(void* parameter)
//{
//gfx_fillrect(5,10,10,15);
//
//}
