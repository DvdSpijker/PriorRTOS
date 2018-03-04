

#include "HalUart.h"
#include <stdint.h>
#include <intc.h>



void HalUartInit(struct HalUartInstance *uart_handle)
{
    uart_handle->packet_write_i = uart_handle->packet_read_i = (UART_RX_packetSIZE -1);
    uart_handle->packet_data_avail = 0;
    uart_handle->packet_status = 0;

    usart_serial_options_t uart_opt;
    uart_opt.baudrate = uart_handle->baud_rate;
    uart_opt.paritytype = USART_NO_PARITY;
    uart_opt.stopbits = USART_2_STOPBITS;
    uart_opt.charlength = 8;


    usart_serial_init(uart_handle->channel, &uart_opt);

}


void HalUartSendChar(struct HalUartInstance *uart_handle, char character)
{
    usart_serial_putchar(uart_handle->channel, character);
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
    if(uart_handle->packet_status == 0) {
        return 0;    //no data present
    }

    uint8_t i = 0;
    do {


        if (uart_handle->packet_read_i == (UART_RX_packetSIZE-1) && uart_handle->packet_data_avail) {
            uart_handle->packet_read_i  = 0;
        }

        else if(uart_handle->packet_data_avail) {
            uart_handle->packet_read_i++;
        }

        else {
            uart_handle->packet_status = 0;
            break;
        }

        *(target+i) = uart_handle->rx_packet[uart_handle->packet_read_i];
        uart_handle->packet_data_avail--;
        i++;
    } while(i<(amount) && uart_handle->packet_data_avail);

    if(!uart_handle->packet_data_avail) {
        uart_handle->packet_status = 0;
    } else {
        uart_handle->packet_status = 1;
    }

    return i;

}

uint8_t HalUartpacketDataAmountGet(struct HalUartInstance *uart_handle)
{
    return uart_handle->packet_data_avail;
}

uint8_t HalUartpacketStatusGet(struct HalUartInstance *uart_handle)
{
    return uart_handle->packet_status;
}

uint8_t HalUartpacketContains(struct HalUartInstance *uart_handle, const char check_char)
{
    for (uint8_t i=0; i<uart_handle->packet_data_avail; i++) {
        if(uart_handle->rx_packet[i] == check_char) {
            return i;
        }
    }
    return 0;
}

void HalUartpacketFlush(struct HalUartInstance *uart_handle)
{
    uart_handle->packet_read_i = uart_handle->packet_write_i = (UART_RX_packetSIZE -1);
    uart_handle->packet_status = 0;
}




//
//ISR(USART0_RX_vect)
//{
//
//if (uart_handle_0->packet_status != 2) {
//
//if (uart_handle_0->packet_write_i == (UART_RX_packetSIZE-1) && (UART_RX_packetSIZE - uart_handle_0->packet_data_avail)) {
//uart_handle_0->packet_write_i = 0;
//} else if (UART_RX_packetSIZE -  uart_handle_0->packet_data_avail) {
//uart_handle_0->packet_write_i++;
//}
//
//uart_handle_0->packet_status = 1;
//
//if(uart_handle_0->packet_data_avail == UART_RX_packetSIZE) {
//uart_handle_0->packet_status = 2;
//}
//
//uart_handle_0->rx_packet[uart_handle_0->packet_write_i] = UDR0;
//uart_handle_0->packet_data_avail++;
//}
//
//}


