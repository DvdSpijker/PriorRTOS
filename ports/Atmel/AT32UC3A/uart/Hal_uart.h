#ifndef HAL_UART_H
#define HAL_UART_H

#include <stdint.h>
#include <Types.h>

#include <serial.h>

#define UART_RX_BUFSIZE 20 /* Buffer size per channel. */

#define UART_STOP_BITS_1 0
#define UART_STOP_BITS_2 1

#define UART_CHANNEL_LOGGER &AVR32_USART1

typedef void (*UartInstanceCallback_t)(void);

typedef volatile avr32_usart_t * HalUartBase_t;

struct  HalUartInstance {
    HalUartBase_t channel;
    uint32_t baud_rate;
    uint8_t n_stop_bits;
    UartInstanceCallback_t rx_callback;

    /* Do not access these fields directly. */
    uint8_t rx_buffer[UART_RX_BUFSIZE];
    volatile uint8_t buffer_read_i;
    volatile uint8_t buffer_write_i;
    volatile uint8_t buffer_data_avail;
    volatile uint8_t buffer_status; //0 = empty, 1 = data present, 2 = full
};

void HalUartInit(struct HalUartInstance *uart_handle);

void HalUartSendChar(struct HalUartInstance *uart_handle, char character);

void HalUartSendString(struct HalUartInstance *uart_handle, const char* str);

uint8_t HalUartReceive(struct HalUartInstance *uart_handle, char* target, uint8_t amount);

uint8_t HalUartBufferDataAmountGet(struct HalUartInstance *uart_handle);

uint8_t HalUartBufferStatusGet(struct HalUartInstance *uart_handle);

uint8_t HalUartBufferContains(struct HalUartInstance *uart_handle, const char check_char);

void HalUartBufferFlush(struct HalUartInstance *uart_handle);


#endif