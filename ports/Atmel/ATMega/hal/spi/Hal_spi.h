/*
 * Hal_spi.h
 *
 * Created: 16-5-2017 20:50:08
 *  Author: Dorus
 */


#ifndef HAL_SPI_H_
#define HAL_SPI_H_

#include <stdint.h>
#include "../gpio/Hal_gpio.h"

#define SPI_MODE_MASTER 0x01
#define SPI_MODE_SLAVE  0x02

#define SPI_DUMMY_BYTE 0xFF

typedef uint8_t HalSpiBase_t;

struct HalSpiInstance {
    HalSpiBase_t    channel;
    uint8_t         mode;
    uint32_t        baud_rate;
};

void HalSpiInit(struct HalSpiInstance *spi_handle);

void HalSpiWriteByte(struct HalSpiInstance *spi_handle, uint8_t data);

uint8_t HalSpiReadByte(struct HalSpiInstance *spi_handle);

void HalSpiWrite(struct HalSpiInstance *spi_handle, uint8_t *data, uint8_t length);

void HalSpiRead(struct HalSpiInstance *spi_handle, uint8_t *data, uint8_t length);

void HalSpiDeviceSelect(struct HalSpiInstance *spi_handle);

void HalSpiDeviceDeselect(struct HalSpiInstance *spi_handle);


#endif /* HAL_SPI_H_ */