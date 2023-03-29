#include "spi.h"
//https://github.com/prtzl/Embedded_videos/blob/master/027_HAL-SPI/spi_examples.c

extern SPI_HandleTypeDef hspi1;

HAL_StatusTypeDef spi_write(uint8_t regAddr, uint8_t *pData){
	HAL_StatusTypeDef ret;
	uint8_t send_data[2] = {regAddr, *pData};
	
	//HAL_GPIO_WritePin();
	
	//going to try it as if it were lab 5
	ret = HAL_SPI_Transmit(&hspi1, send_data, 1, 100);
	if (ret != HAL_OK){
		//print to hercules
		print_msg("Bad write\n");
		Error_Handler();
	}
	
	
	return ret;
}

HAL_StatusTypeDef spi_read(uint8_t regAddr, uint8_t *pData){
	HAL_StatusTypeDef ret;
	print_msg("in read\n");
	HAL_GPIO_WritePin(CS_ACC_GPIO_Port, CS_ACC_Pin, 0);
	
	uint8_t command_byte = 0x0B;
	
	ret = HAL_SPI_Transmit(&hspi1, &command_byte, 1, 100);
	if (ret != HAL_OK){
		//print to hercules
		HAL_GPIO_WritePin(CS_ACC_GPIO_Port, CS_ACC_Pin, 1);
		print_msg("Bad send\n");
		Error_Handler();
	}
	print_msg("ok transmit\n");
	
	ret = HAL_SPI_Transmit(&hspi1, &regAddr, 1, 100);
	if (ret != HAL_OK){
		//print to hercules
		HAL_GPIO_WritePin(CS_ACC_GPIO_Port, CS_ACC_Pin, 1);
		print_msg("Bad send\n");
		Error_Handler();
	}
	print_msg("ok transmit\n");
	
	ret = HAL_SPI_Receive(&hspi1, pData, 1, 100);
	if (ret != HAL_OK){
		//print to hercules
		HAL_GPIO_WritePin(CS_ACC_GPIO_Port, CS_ACC_Pin, 1);
		print_msg("Bad read\n");
		Error_Handler();
	}
	print_msg("ok transmit\n");
	
	HAL_GPIO_WritePin(CS_ACC_GPIO_Port, CS_ACC_Pin, 1);
	return ret;
}