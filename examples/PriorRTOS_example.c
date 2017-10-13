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
#include <PriorRTOS.h>
#include <Mailbox.h>
#include <Convert.h>
#include <Logger.h>
#include <Task_def.h>


LOG_INIT_FILE("PriorRTOS_example.c");

//#include "pwmgen.h"

/*GLCD Drivers*/
//#include "Prior RTOS/Drivers/GLCD_ST7920/st7920.h>
//#include "Prior RTOS/Drivers/GLCD_ST7920/graphics.h>
//#include "arial12.h>
//#include "Prior RTOS/Drivers/GLCD_ST7920/hal.h>

/*Task prototypes*/

void RenderUI(void*);
void UpdateUI(void*);
void BlinkGreen(void*);
void BlinkRed(void*);
void BlinkBlue(void*);
void UnitTest(void*);
void RingbufTest(void*);
void PipeTest(void*);
void MailboxTest(void*);
void ItcTest(void*);

#define  LED_BLUE_PIN  0
#define  LED_GREEN_PIN 1
#define  LED_RED_PIN   2

#define TASK_ITC_TEST_OP_MODE_MAILBOX 0x00
#define TASK_ITC_TEST_OP_MODE_PIPE    0x01
#define TASK_ITC_TEST_OP_MODE_RINGBUF 0x02


#define TEST_EVENT (EVENT_TYPE_ACCESS | 0x0000DEAD)

#define TIMEOUT_EVENT (EVENT_TYPE_ACCESS | 0x0000BEEF)

Id_t TaskIdBlinkRed = INVALID_ID, TaskIdBlinkBlue = INVALID_ID, TaskIdUnitTest = INVALID_ID, TaskIdMailboxTest = INVALID_ID,
     TaskIdPipeTest = INVALID_ID, TaskIdRingbufTest = INVALID_ID, TaskIdItcTest = INVALID_ID;

Id_t TimerId1, TimerId2, TimerId3;
Id_t MemPool0, MemPool1;
Id_t TestMailbox = INVALID_ID;
Id_t TestRingbuffer = INVALID_ID;

extern OsResult_t MmPoolAllocTableCreate(Id_t pool_id); //Do not use
extern OsResult_t MmPoolAllocTableAdd(void* ptr, Id_t pool_id); //Do not use
extern  U16_t MmGeneratePoolCRC(Id_t pool_id); //Do not use

U32_t *pSharedMemAlloc = NULL;
Id_t   SharedMemSem;

int main(void)
{
    MCUSR = 0;
    /*Prior initiation*/
    OsResult_t opt_status;
    OsResult_t k_status;

    HalGpioPinModeSet(&DDRB, LED_BLUE_PIN, PIN_MODE_OUTPUT);
    HalGpioPinModeSet(&DDRB, LED_GREEN_PIN, PIN_MODE_OUTPUT);
    HalGpioPinModeSet(&DDRB, LED_RED_PIN, PIN_MODE_OUTPUT);
    HalGpioPinModeSet(&DDRA, PA6, PIN_MODE_OUTPUT);

    char buffer[15];
    k_status = OsInit(&opt_status);
    if(k_status != OS_OK) {
        ConvertResultToString(k_status, buffer);
        LOG_ERROR_NEWLINE("Error: %s occurred while initializing essential kernel modules. Awaiting reset..", buffer);
        while(1);
    } else if (opt_status != OS_OK) {
        ConvertResultToString(opt_status, buffer);
        LOG_ERROR_NEWLINE("Error: %s occurred while initializing optional kernel modules. The system will be booted without the uninitialized module(s).", buffer);
    }

    //MemPool0 = MmPoolCreate(20);
    //LOG_DEBUG_NEWLINE("Created memory pool; 0x%04X", MemPool0);
    //crc0 = MmGeneratePoolCRC(pool0);

    //pool1 = MmPoolCreate(20); //Create a second pool with a size of 30 bytes

    /*Event declarations*/

    /*Task declarations - highest priority first - descending*/
    //new_event = EventgroupCreate();


    TaskIdBlinkRed = TaskCreate(BlinkRed, TASK_CAT_MEDIUM, 5, NULL); //TaskGenericNameSet(BlinkRed,"BlinkRed");
    //TaskCreate(RenderUI,cat_high,1);
    //TaskCreate(UpdateUI,cat_high,1);
    //TaskIdBlinkBlue = TaskCreate(BlinkBlue, TASK_CAT_HIGH, 1, NULL); //TaskGenericNameSet(BlinkBlue,"BlinkBlue");
    //TaskCreate(BlinkGreen, TASK_CAT_REALTIME, 5, NULL);// TaskGenericNameSet(BlinkGreen,"BlinkGreen");
    //TaskIdRingbufTest = TaskCreate(RingbufTest, TASK_CAT_HIGH, 5, NULL); //TaskGenericNameSet(RingbufTest,"RingbufTest");
    //TaskCreate(PipeTest, TASK_CAT_HIGH, 5, NULL); //TaskGenericNameSet(PipeTest,"PipeTest");

    //TaskIdMailboxTest   = TaskCreate(MailboxTest, TASK_CAT_HIGH, 5, NULL); //TaskGenericNameSet(MailboxTest,"MailboxTest");
    //TaskIdItcTest       = TaskCreate(ItcTest, TASK_CAT_HIGH, 4, NULL);
    // TaskIdUnitTest      = TaskCreate(UnitTest, TASK_CAT_REALTIME, 2, NULL);  //TaskGenericNameSet(UnitTest,"UnitTest");
//
//
    //mailbox0 = mailbox_Create(10,3); //10 elements, 3 owners max.
    //if(mailbox_OwnerAdd(mailbox0,comUART) == OS_OK) //Add comUART as owner
    //{
    //TaskWake(comUART,NULL);
    //}

    /*Timer declarations*/
    //TimerId1 = TimerCreate(250e3, (TIMER_PARAMETER_P | TIMER_PARAMETER_ON | TIMER_PARAMETER_AR)); //ON, AR, Permanent
    //TimerId2 = TimerCreate(1000e3, (TIMER_PARAMETER_P | TIMER_PARAMETER_ON | TIMER_PARAMETER_AR)); //ON,Permanent
    //TimerId3 = TimerCreate(50000, (TIMER_PARAMETER_P | TIMER_PARAMETER_AR | TIMER_PARAMETER_ON)); //ON, AR, Permanent
    //for (U8_t i = 0; i < 10; i++) {
    //timers[i] = TimerCreate((5e5 + i * 1e5), (TIMER_PARAMETER_P | TIMER_PARAMETER_AR | TIMER_PARAMETER_ON));
    //}

    /*MailBox assignments*/

    //TaskWait(UpdateUI,timer1,0x00);

    //TaskNotify(BlinkGreen, NULL);
    //TaskNotify(TaskIdBlinkRed, NULL);
    //TaskNotify(TaskIdBlinkBlue, NULL);
    //union SemaphoreResource sem_resource;
    // sem_resource.resource_typeless = (void *)pSharedMemAlloc;

    //SharedMemSem = SemaphoreCreate(SEM_TYPE_MUTEX_BINARY, sem_resource);
    //pSharedMemAlloc = MmAllocDynamic(MemPool0, sizeof(U32_t));


    OsStart(TaskIdBlinkRed); /* <= This should be the last function called in main(),
                                function calls or initializations below this point
                                will be ignored.
                                NO USER CODE BELOW THIS POINT
                        */
} //End main(void)


/*-----------------------USER DEFINED TASKS------------------------*/

void BlinkGreen(void* parameter)
{
    LOG_DEBUG_NEWLINE("Blink green");
    HalGpioPinStateSet(&PORTB, LED_GREEN_PIN, PIN_STATE_TOGGLE);
    TaskSleep(100);
}

void BlinkRed(void* parameter)
{
    LOG_DEBUG_NEWLINE("Blink red");
    //HalGpioPinStateSet(&PORTB, LED_RED_PIN, PIN_STATE_TOGGLE);
    HalGpioPinStateSet(&PORTA, PA6, PIN_STATE_TOGGLE);
    TaskSleep(500);
}

//void BlinkBlue(void* parameter)
//{
//static Id_t timer_event = INVALID_ID;
//TASK_INIT_ENTER();
//TaskWait(TimerId1, TIMER_EVENT_OVERFLOW, EVENT_FLAG_PERMANENT, 0, &timer_event);
//TASK_INIT_EXIT();
//
//TASK_EVENT_HANDLER_ENTER(timer_event);
//LOG_DEBUG_NEWLINE("Blink blue");
//HalGpioPinStateSet(&PORTB, LED_BLUE_PIN, PIN_STATE_TOGGLE);
//TASK_EVENT_HANDLER_EXIT();
//}




//void ItcTest(void *parameter)
//{
////U8_t* op_mode = (U8_t*)(parameter);
//OsResult_t result = OS_OK;
//
//
//static RingbufBase_t test_array[5] = {'t', 'e', 's', 't', '\0'};
//
//
//if(TestMailbox != INVALID_ID) {
///* 1st Post test. */
//result = MailboxPost(TestMailbox, 0x00, test_array, 4);    /* Expected result = OS_OK. */
//char result_buf[15] = {};
//ConvertResultToString(result, result_buf);
//LOG_DEBUG_NEWLINE("Itc Mailbox test:Attempt to post %s at address %u in mailbox %04X: %s", test_array, 0, TestMailbox, result_buf);
//
//LOG_DEBUG_NEWLINE("Itc Mailbox test:Pend counter at address %u: %u", 0x00, MailboxPendCounterGet(TestMailbox, 0x00));
//result_buf[0] = '\0';
//
///* Attempt to overwrite */
//result = MailboxPost(TestMailbox, 0x00, test_array, 4); /* Expected result = OS_LOCKED. */
//ConvertResultToString(result, result_buf);
//LOG_DEBUG_NEWLINE("Itc Mailbox test:Attempt to post %s at address %u in mailbox %04X: %s", test_array, 0, TestMailbox, result_buf);
//
//result_buf[0] = '\0';
//
///* 2nd Post test. */
//result = MailboxPost(TestMailbox, 0x04, test_array, 4); /* Expected result = OS_OK. */
//ConvertResultToString(result, result_buf);
//LOG_DEBUG_NEWLINE("Itc Mailbox test:Attempt to post %s at address %u in mailbox %04X: %s", test_array, 4, TestMailbox, result_buf);
//
//LOG_DEBUG_NEWLINE("Itc Mailbox test:Pend counter at address %u: %u", 0x04, MailboxPendCounterGet(TestMailbox, 0x04));
//result_buf[0] = '\0';
//}
//
//
//static U8_t i = 0;
//if(TestRingbuffer != INVALID_ID) {
///* Write test. Write 1 too much. */
//U8_t write_count = 0;
//test_array[4] = i + 48;
//write_count = RingbufWrite(TestRingbuffer, test_array, 5);
//if(write_count < 5) {
//LOG_DEBUG_NEWLINE("Itc Ringbuffer test:Write incomplete: %d", write_count);
//} else {
//LOG_DEBUG_NEWLINE("Itc Ringbuffer test:Write complete: %d", write_count);
//}
//i++;
//
//}
//
//
//
//}

//void RingbufTest(void *parameter)
//{
//
//static Id_t TestRingbufDataInEvent = INVALID_ID;
//static U8_t TestRingbufferCounter = 0;
//
//TASK_INIT_ENTER();
///* Creation of test mailbox. */
//TestRingbuffer = RingbufCreate(5);
//
//if(TestRingbuffer == INVALID_ID) {
//LOG_DEBUG_NEWLINE("Ringbuffer test:Failed to created ringbuffer.");
//goto task_cleanup;
//} else {
//LOG_DEBUG_NEWLINE("Ringbuffer test:Ringbuffer  created (%x).", TestRingbuffer);
//if(TaskWait(TestRingbuffer, RINGBUF_EVENT_DATA_IN, EVENT_FLAG_PERMANENT, 0, &TestRingbufDataInEvent) == OS_OK) {
//LOG_DEBUG_NEWLINE("Ringbuffer test:Waiting for ringbuffer data in event (%04X).", TestRingbufDataInEvent);
//} else {
//LOG_DEBUG_NEWLINE("Ringbuffer test:Failed to subscribe to ringbuffer data in event.");
//}
//}
//TASK_INIT_EXIT();
//
//static RingbufBase_t test_array[6] = {};
//TASK_EVENT_HANDLER_ENTER(TestRingbufDataInEvent)
///* Read test. Read 1 too much. */
//if(RingbufDataCountGet(TestRingbuffer)) {
//U8_t read_count = 0;
//read_count = RingbufRead(TestRingbuffer, test_array, 5);
//test_array[read_count] = '\0';
//if(read_count < 5) {
//LOG_DEBUG_NEWLINE("Ringbuffer test:Read incomplete: %d | Read: %s", read_count, test_array);
//} else {
//LOG_DEBUG_NEWLINE("Ringbuffer test:Read complete: %d | Read: %s", read_count, test_array);
//}
//
//}
//TASK_EVENT_HANDLER_EXIT();
//
//if(TestRingbufferCounter < 3) {
//TestRingbufferCounter++;
//TaskNotify(TaskIdItcTest, NULL);
//TASK_YIELD();
//}
//
//OsResult_t result;
//result = RingbufDelete(&TestRingbuffer);
//if(result == OS_OK) {
//LOG_DEBUG_NEWLINE("Ringbuffer test:Ringbuffer deleted.");
//} else {
//LOG_DEBUG_NEWLINE("Ringbuffer test:Failed to delete ringbuffer.");
//}
//
//
//task_cleanup:
//result = TaskDelete(&TaskIdRingbufTest);
//if(result == OS_OK) {
//LOG_DEBUG_NEWLINE("Ringbuffer test:RingbufferTest task deleted.");
//} else {
//LOG_DEBUG_NEWLINE("Ringbuffer test:Failed to delete RingbufferTest task.");
//}
//}

void PipeTest(void *parameter)
{
    /* Creation and test array declaration. */
    Id_t pipe = PipeCreate(5);
    if(pipe == INVALID_ID) {
        LOG_DEBUG_NEWLINE("pipetest:Failed to created pipe.");
        goto task_exit;
    } else {
        LOG_DEBUG_NEWLINE("pipetest:Pipe created.");
    }
    U8_t test_array[4] = {'t', 'e', 's', 't'};


    /* Write test. */
    OsResult_t result = OS_OK;

    /* Attempt to write with pipe closed. */
    result = PipeWrite(pipe, test_array, 1); /* Expected result = e_locked. */
    char result_buf[10] = {};
    ConvertResultToString(result, result_buf);
    LOG_DEBUG_NEWLINE("pipetest:Write result: %s", result_buf);

    PipeOpen(pipe);
    LOG_DEBUG_NEWLINE("pipetest:Open count: %d", PipeOpenCountGet(pipe));

    for (U8_t i = 0; i < 4; i++) {
        result = PipeWrite(pipe, (test_array+i), 1);
        if(result != OS_OK) {
            char result_buf[10] = {};
            ConvertResultToString(result, result_buf);
            LOG_DEBUG_NEWLINE("pipetest:Write failed: %s", result_buf);
            break;
        } else {
            LOG_DEBUG_NEWLINE("pipetest:Write successful.");
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
    static Id_t TestMailboxPostEvent = INVALID_ID;
    static U8_t TestMailboxCounter = 0;

    TASK_INIT_ENTER();
    /* Creation of test mailbox. */
    TestMailbox = MailboxCreate(10, &TaskIdMailboxTest, 1);

    if(TestMailbox == INVALID_ID) {
        LOG_DEBUG_NEWLINE("Mailbox test:Failed to created mailbox.");
        goto task_cleanup;
    } else {
        LOG_DEBUG_NEWLINE("Mailbox test:Mailbox created (%x).", TestMailbox);
        if(TaskWait(TestMailbox, MAILBOX_EVENT_POST(0x00), EVENT_FLAG_PERMANENT, 0, &TestMailboxPostEvent) == OS_OK) {
            LOG_DEBUG_NEWLINE("Mailbox test:Waiting for mailbox post event (%04X) at address %u", TestMailboxPostEvent, 0);
        } else {
            LOG_DEBUG_NEWLINE("Mailbox test:Failed to subscribe to mailbox post event.");
        }
    }

    TASK_INIT_EXIT();

    OsResult_t result = OS_OK;
    U8_t test_array[5] = {};
    char result_buf[15] = {};

    TASK_EVENT_HANDLER_ENTER(TestMailboxPostEvent);
    /* Pend test. */
    result = MailboxPend(TestMailbox, 0x00, test_array, 4); /* Expected result = OS_OK. */
    test_array[4] = '\0';
    ConvertResultToString(result, result_buf);
    LOG_DEBUG_NEWLINE("Mailbox test:Pend result: %s, mailbox data at address %u: %s", result_buf, 0, test_array);

    LOG_DEBUG_NEWLINE("Mailbox test:Pend counter at address %u: %u", 0x00, MailboxPendCounterGet(TestMailbox, 0x00));
    result_buf[0] = '\0';
    TestMailboxCounter++;

    TASK_EVENT_HANDLER_EXIT();


    if(TestMailboxCounter < 3) {
        TestMailboxCounter++;
        TaskNotify(TaskIdItcTest, NULL);
        TASK_YIELD();
    }

    result = MailboxDelete(&TestMailbox);
    if(result == OS_OK) {
        LOG_DEBUG_NEWLINE("Mailbox test:Mailbox deleted.");
    } else {
        LOG_DEBUG_NEWLINE("Mailbox test:Failed to delete mailbox.");
    }

task_cleanup:
    result = TaskDelete(&TaskIdMailboxTest);
    if(result == OS_OK) {
        LOG_DEBUG_NEWLINE("Mailbox test:MailboxTest task deleted.");
    } else {
        LOG_DEBUG_NEWLINE("Mailbox test:Failed to delete MailboxTest task.");
    }

    result = TaskNotify(TaskIdUnitTest, NULL);
    result = TaskNotify(TaskIdBlinkRed, NULL);
    result = TaskNotify(TaskIdBlinkBlue, NULL);
    result = OS_ERROR;
}


void UnitTest(void *task_args)
{
    OsResult_t result;
    static Id_t timeout_event_id = INVALID_ID;

    TASK_INIT_ENTER();
    LOG_DEBUG_NEWLINE("Eventtest:Initializing task.");
    /* Subscribed to TIMEOUT_EVENT, which will not be published therefore
    * a timeout should occur after 1000ms. */
    result = TaskWait(0xFEEF, TIMEOUT_EVENT, EVENT_FLAG_PERMANENT, 1000, &timeout_event_id);
    if(result == OS_OK) {
        LOG_DEBUG_NEWLINE("Eventtest:Subscribed to TIMEOUT_EVENT with timeout = 1000ms.");
    } else {
        LOG_ERROR_NEWLINE("Eventtest:Error while subscribing to TIMEOUT_EVENT.");
    }
    TASK_INIT_EXIT();


    static U8_t alloc_size = 1;
    static U8_t *test_alloc = NULL;

    if(test_alloc != NULL) {
        result = MmFree((void **)&test_alloc);
        if(result == OS_OK) {
            LOG_DEBUG_NEWLINE("Freed memory.");
        }
    }
    test_alloc = (U8_t *)MmAllocDynamic(MemPool0, (sizeof(U8_t) * alloc_size));
    if(test_alloc != NULL) {
        LOG_DEBUG_NEWLINE("Allocated memory of size %u.", (sizeof(U8_t) * alloc_size));
        for (U8_t i = 0; i < alloc_size; i++) {
            test_alloc[i] = 0xDE;
        }
    }
    if(alloc_size < 5) {
        alloc_size++;
    } else {
        alloc_size = 1;
    }


    TASK_EVENT_HANDLER_ENTER(timeout_event_id);
    TASK_EVENT_HANDLER_TIMEOUT_ENTER();
    LOG_DEBUG_NEWLINE("Eventtest:Handling event %p timeout (%u ms)", occ_event, occ_event->life_time_ms);
    TASK_EVENT_HANDLER_TIMEOUT_EXIT();
    TASK_EVENT_HANDLER_EXIT();
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
