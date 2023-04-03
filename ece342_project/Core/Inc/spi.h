#ifndef __SPI_H__
#define __SPI_H__

#include "main.h"
HAL_StatusTypeDef spi_write(uint8_t regAddr, uint8_t *pData);
HAL_StatusTypeDef spi_read_old(uint8_t regAddr, uint8_t *pData);
HAL_StatusTypeDef spi_read(uint8_t regAddr, uint8_t *pData, uint8_t len);
HAL_StatusTypeDef oled_data_write(uint8_t *data, uint8_t len);
void drawpixel(uint8_t c, uint8_t r);
void cleardisplay();
#endif