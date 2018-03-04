#ifndef HAL_UART_H
#define HAL_UART_H

#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>
#include <Types.h>

#include <serial.h>

#define UART_RX_packetSIZE 20 /* packet size per channel. */

#define HAL_UART_STOP_BITS_1 0
#define HAL_UART_STOP_BITS_2 1

#define UART_CHANNEL_LOGGER &AVR32_USART1

typedef void (*UartInstanceCallback_t)(void);

typedef volatile avr32_usart_t * HalUartBase_t;

struct  HalUartInstance {
    HalUartBase_t channel;
    uint32_t baud_rate;
    uint8_t n_stop_bits;
    UartInstanceCallback_t rx_callback;

    /* Do not access these fields directly. */
    uint8_t rx_packet[UART_RX_packetSIZE];
    volatile uint8_t packet_read_i;
    volatile uint8_t packet_write_i;
    volatile uint8_t packet_data_avail;
    volatile uint8_t packet_status; //0 = empty, 1 = data present, 2 = full
};

void HalUartInit(struct HalUartInstance *uart_handle);

void HalUartSendChar(struct HalUartInstance *uart_handle, char character);

void HalUartSendString(struct HalUartInstance *uart_handle, const char* str);

uint8_t HalUartReceive(struct HalUartInstance *uart_handle, char* target, uint8_t amount);

uint8_t HalUartpacketDataAmountGet(struct HalUartInstance *uart_handle);

uint8_t HalUartpacketStatusGet(struct HalUartInstance *uart_handle);

uint8_t HalUartpacketContains(struct HalUartInstance *uart_handle, const char check_char);

void HalUartpacketFlush(struct HalUartInstance *uart_handle);

#ifdef __cplusplus
}
#endif
#endif