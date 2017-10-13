#include "Hal_spi.h"
#include <sysclk.h>

void HalSpiInit(struct HalSpiInstance *spi_handle)
{
    
    spi_master_init(spi_handle->channel);
    spi_master_setup_device(spi_handle->channel, &spi_handle->device, SPI_MODE_0, spi_handle->baud_rate, 0);   
    spi_enable(spi_handle->channel);
    sysclk_enable_peripheral_clock(spi_handle->channel);
}

void HalSpiWriteByte(struct HalSpiInstance *spi_handle, uint8_t data)
{
    //spi_write_single(spi_handle->channel, data);
    spi_write_packet(spi_handle->channel, &data, 1);
}

uint8_t HalSpiReadByte(struct HalSpiInstance *spi_handle)
{
    return 0;
}

void HalSpiWrite(struct HalSpiInstance *spi_handle, uint8_t *data, uint8_t length)
{
    for (uint8_t i = 0; i < length; i++) {
        spi_write_single(spi_handle->channel, data[i]);
    }
}

void HalSpiRead(struct HalSpiInstance *spi_handle, uint8_t *data, uint8_t length)
{
    
}

void HalSpiDeviceSelect(struct HalSpiInstance *spi_handle)
{
    spi_select_device(spi_handle->channel, &spi_handle->device);
}

void HalSpiDeviceDeselect(struct HalSpiInstance *spi_handle)
{
    spi_deselect_device(spi_handle->channel, &spi_handle->device);
}

bool HalSpiIsReady(struct HalSpiInstance *spi_handle)
{
    return (spi_is_tx_ready(spi_handle->channel));
}