#include "spi.h"
//https://github.com/prtzl/Embedded_videos/blob/master/027_HAL-SPI/spi_examples.c
HAL_StatusTypeDef spi_write(uint8_t regAddr, uint8_t *pData){
	HAL_StatusTypeDef ret;
	uint8_t send_data[2] = {regAddr, *pData};
	
	//HAL_GPIO_WritePin();
	
	//going to try it as if it were lab 5
	//ret = HAL_SPI_Transmit(&hspi1, send_data, 2, 100);
	
	return ret;
}