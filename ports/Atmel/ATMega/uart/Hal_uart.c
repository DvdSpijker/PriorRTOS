

#include "Hal_uart.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>

struct HalUartInstance *uart_handle_0;

void HalUartInit(struct HalUartInstance *uart_handle)
{

    uart_handle->buffer_write_i = uart_handle->buffer_read_i = (UART_RX_BUFSIZE -1);
    uart_handle->buffer_data_avail = 0;
    uart_handle->buffer_status = 0;

    if(uart_handle->channel == UART_CHANNEL_LOGGER) {
        int UBBR_VALUE = (F_CPU/16/uart_handle->baud_rate - 1);
        UBRR0H = (uint8_t)(UBBR_VALUE>>8);
        UBRR0L = (uint8_t)UBBR_VALUE;
        UCSR0B = (1 << RXEN0) | (1 << TXEN0);
        UCSR0C = (1<<USBS0)|(3<<UCSZ00); //8 data bits, 2 stop bits
        UCSR0B |= (1 << RXCIE0); // Enable the USART Receive Complete interrupt (USART_RXC)
        uart_handle_0 = uart_handle;
    } else {
        int UBBR_VALUE = (F_CPU/16/uart_handle->baud_rate - 1);
        UBRR1H = (uint8_t)(UBBR_VALUE>>8);
        UBRR1L = (uint8_t)UBBR_VALUE;
        UCSR1B = (1 << RXEN1) | (1 << TXEN1);
        UCSR1C = (1<<USBS1)|(3<<UCSZ01); //8 data bits, 2 stop bits
        UCSR1B |= (1 << RXCIE1); // Enable the USART Receive Complete interrupt (USART_RXC)
    }

}


void HalUartSendChar(struct HalUartInstance *uart_handle, char character)
{
    if(uart_handle->channel == UART_CHANNEL_LOGGER) {
        //wait while previous byte is completed
        while(!(UCSR0A&(1<<UDRE0)));
        // Transmit data
        UDR0 = character;
    }

}

void HalUartSendString(struct HalUartInstance *uart_handle, const char* str)
{
    int i = 0;
    //wait while previous byte is completed
    while(str[i] != '\0') {
        HalUartSendChar(uart_handle, str[i]);
        i++;
    }

}


uint8_t HalUartReceive(struct HalUartInstance *uart_handle, char* target, uint8_t amount)
{
    if(uart_handle->buffer_status == 0) {
        return 0;    //no data present
    }

    uint8_t i = 0;
    do {


        if (uart_handle->buffer_read_i == (UART_RX_BUFSIZE-1) && uart_handle->buffer_data_avail) {
            uart_handle->buffer_read_i  = 0;
        }

        else if(uart_handle->buffer_data_avail) {
            uart_handle->buffer_read_i++;
        }

        else {
            uart_handle->buffer_status = 0;
            break;
        }

        *(target+i) = uart_handle->rx_buffer[uart_handle->buffer_read_i];
        uart_handle->buffer_data_avail--;
        i++;
    } while(i<(amount) && uart_handle->buffer_data_avail);

    if(!uart_handle->buffer_data_avail) {
        uart_handle->buffer_status = 0;
    } else {
        uart_handle->buffer_status = 1;
    }

    return i;

}

uint8_t HalUartBufferDataAmountGet(struct HalUartInstance *uart_handle)
{
    return uart_handle->buffer_data_avail;
}

uint8_t HalUartBufferStatusGet(struct HalUartInstance *uart_handle)
{
    return uart_handle->buffer_status;
}

uint8_t HalUartBufferContains(struct HalUartInstance *uart_handle, const char check_char)
{
    for (uint8_t i=0; i<uart_handle->buffer_data_avail; i++) {
        if(uart_handle->rx_buffer[i] == check_char) {
            return i;
        }
    }
    return 0;
}

void HalUartBufferFlush(struct HalUartInstance *uart_handle)
{
    uart_handle->buffer_read_i = uart_handle->buffer_write_i = (UART_RX_BUFSIZE -1);
    uart_handle->buffer_status = 0;
    uart_handle->buffer_data_avail = 0;
}





ISR(USART0_RX_vect)
{

    if (uart_handle_0->buffer_status != 2) {

        if (uart_handle_0->buffer_write_i == (UART_RX_BUFSIZE-1) && (UART_RX_BUFSIZE - uart_handle_0->buffer_data_avail)) {
            uart_handle_0->buffer_write_i = 0;
        } else if (UART_RX_BUFSIZE -  uart_handle_0->buffer_data_avail) {
            uart_handle_0->buffer_write_i++;
        }

        uart_handle_0->buffer_status = 1;

        if(uart_handle_0->buffer_data_avail == UART_RX_BUFSIZE) {
            uart_handle_0->buffer_status = 2;
        }

        uart_handle_0->rx_buffer[uart_handle_0->buffer_write_i] = UDR0;
        uart_handle_0->buffer_data_avail++;
    }

}


