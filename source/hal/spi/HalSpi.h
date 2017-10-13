/*
 * Hal_spi.h
 *
 * Created: 16-5-2017 20:50:08
 *  Author: Dorus
 */


#ifndef HAL_SPI_H_
#define HAL_SPI_H_

#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>
#include <stdbool.h>

#include <spi_master.h>

#define SPI_MODE_MASTER 0x01
#define SPI_MODE_SLAVE  0x02

#define SPI_DUMMY_BYTE 0xFF

typedef volatile avr32_spi_t * HalSpiBase_t;

typedef struct spi_device HalSpiDevice_t;

struct HalSpiInstance {
    HalSpiBase_t    channel;
    HalSpiDevice_t  device;
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

bool HalSpiIsReady(struct HalSpiInstance *spi_handle);

#ifdef __cplusplus
}
#endif
#endif /* HAL_SPI_H_ */