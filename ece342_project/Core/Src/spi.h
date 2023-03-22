#ifndef __SPI_H__
#define __SPI_H__

#include "main.h"
HAL_StatusTypeDef spi_write(uint8_t regAddr, uint8_t *pData);
HAL_StatusTypeDef spi_read(uint8_t regAddr, uint8_t *pData);
#endif