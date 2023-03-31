#include "spi.h"
//https://github.com/prtzl/Embedded_videos/blob/master/027_HAL-SPI/spi_examples.c

extern SPI_HandleTypeDef hspi1;

HAL_StatusTypeDef spi_write(uint8_t regAddr, uint8_t *pData){
	HAL_StatusTypeDef ret;
	uint8_t send_data[2] = {regAddr, *pData};
	
	HAL_GPIO_WritePin(CS_ACC_GPIO_Port, CS_ACC_Pin, 0);
	
	uint8_t command_byte = 0x0A;
	
	ret = HAL_SPI_Transmit(&hspi1, &command_byte, 1, 100);
	if (ret != HAL_OK){
		//print to hercules
		HAL_GPIO_WritePin(CS_ACC_GPIO_Port, CS_ACC_Pin, 1);
		print_msg("Bad send\n");
		Error_Handler();
	}
	
	ret = HAL_SPI_Transmit(&hspi1, send_data, 2, 100);
	if (ret != HAL_OK){
		//print to hercules
		HAL_GPIO_WritePin(CS_ACC_GPIO_Port, CS_ACC_Pin, 1);
		print_msg("Bad send\n");
		Error_Handler();
	}
	//print_msg("write seems ok");
	HAL_GPIO_WritePin(CS_ACC_GPIO_Port, CS_ACC_Pin, 1);
	return ret;
}

HAL_StatusTypeDef spi_read(uint8_t regAddr, uint8_t *pData){
	HAL_StatusTypeDef ret;
	//print_msg("in read\n");
	HAL_GPIO_WritePin(CS_ACC_GPIO_Port, CS_ACC_Pin, GPIO_PIN_RESET); // pull the CS low to enable the slave
	
	uint8_t command_byte = 0x0B;
	
	ret = HAL_SPI_Transmit(&hspi1, &command_byte, 1, 100); //specific to the adxl
	if (ret != HAL_OK){
		//print to hercules
		HAL_GPIO_WritePin(CS_ACC_GPIO_Port, CS_ACC_Pin, 1);
		print_msg("Bad send\n");
		Error_Handler();
	}
	//print_msg("ok transmit\n");
	
	ret = HAL_SPI_Transmit(&hspi1, &regAddr, 1, 100); //transmit the address where we want to read data
	if (ret != HAL_OK){
		//print to hercules
		HAL_GPIO_WritePin(CS_ACC_GPIO_Port, CS_ACC_Pin, 1); //resets thee pin
		print_msg("Bad send\n");
		Error_Handler();
	}
	//print_msg("ok transmit\n");
	
	ret = HAL_SPI_Receive(&hspi1, pData, 1, 100); //receive the data
	if (ret != HAL_OK){
		//print to hercules
		HAL_GPIO_WritePin(CS_ACC_GPIO_Port, CS_ACC_Pin, 1);
		print_msg("Bad read\n");
		Error_Handler();
	}
	//print_msg("ok transmit\n");
	
	HAL_GPIO_WritePin(CS_ACC_GPIO_Port, CS_ACC_Pin, 1); //pull the CS pin high to disable slave
	return ret;
}

HAL_StatusTypeDef spi_read_new(uint8_t regAddr, uint8_t *pData, uint8_t len){
	HAL_StatusTypeDef ret;
	//print_msg("in read\n");
	HAL_GPIO_WritePin(CS_ACC_GPIO_Port, CS_ACC_Pin, 0);
	
	uint8_t command_byte = 0x0B;
	
	ret = HAL_SPI_Transmit(&hspi1, &command_byte, 1, 100);
	if (ret != HAL_OK){
		//print to hercules
		HAL_GPIO_WritePin(CS_ACC_GPIO_Port, CS_ACC_Pin, 1);
		print_msg("Bad send\n");
		Error_Handler();
	}
	//print_msg("ok transmit\n");
	
	ret = HAL_SPI_Transmit(&hspi1, &regAddr, 1, 100);
	if (ret != HAL_OK){
		//print to hercules
		HAL_GPIO_WritePin(CS_ACC_GPIO_Port, CS_ACC_Pin, 1);
		print_msg("Bad send\n");
		Error_Handler();
	}
	//print_msg("ok transmit\n");
	
	ret = HAL_SPI_Receive(&hspi1, pData, len, 100);
	if (ret != HAL_OK){
		//print to hercules
		HAL_GPIO_WritePin(CS_ACC_GPIO_Port, CS_ACC_Pin, 1);
		print_msg("Bad read\n");
		Error_Handler();
	}
	//print_msg("ok transmit\n");
	
	HAL_GPIO_WritePin(CS_ACC_GPIO_Port, CS_ACC_Pin, 1);
	return ret;
}

HAL_StatusTypeDef oled_write(uint8_t *cmd, uint8_t len){
	HAL_StatusTypeDef ret;
	
	HAL_GPIO_WritePin(LED_display_GPIO_Port, LED_display_Pin, 0);
	//uint8_t command_byte = CMD_DISPLAYOFF;
	ret = HAL_SPI_Transmit(&hspi1, cmd, len, 100);
	HAL_GPIO_WritePin(LED_display_GPIO_Port, LED_display_Pin, 1);
	return ret;
}

void oled_init(){
	uint8_t cmd = CMD_DISPLAYOFF;
	oled_write(&cmd, 1);
	uint8_t cmds[2];
	cmds[0] = CMD_SETREMAP;
	cmds[1] = 0x72;
	oled_write(cmds, 2);
	
	cmds[0] = CMD_SETDISPLAYSTARTLINE;
	cmds[1] = 0x00;
	oled_write(cmds, 2);
	
	cmds[0] = CMD_SETDISPLAYOFFSET;
	cmds[1] = 0x00;
	oled_write(cmds, 2);
	
	cmd = CMD_NORMALDISPLAY;
	oled_write(&cmd, 1);
	
	cmds[0]=CMD_SETMULTIPLEXRATIO;
	cmds[1]=0x3F;
	oled_write(cmds, 2);
	
	cmds[0]=CMD_SETMASTERCONFIGURE;
	cmds[1]=0x8E;
	oled_write(cmds, 2);
	
	cmds[0]=CMD_POWERSAVEMODE;
	cmds[1]=0x0B;
	oled_write(cmds, 2);
	
	cmds[0]=CMD_PHASEPERIODADJUSTMENT;
	cmds[1]=0x31;
	oled_write(cmds, 2);
	
	cmds[0]=CMD_DISPLAYCLOCKDIV;
	cmds[1]=0xF0;
	oled_write(cmds, 2);
	
	cmds[0]=CMD_SETPRECHARGESPEEDA;
	cmds[1]=0x64;
	oled_write(cmds, 2);
	
	cmds[0]=CMD_SETPRECHARGESPEEDB;
	cmds[1]=0x78;
	oled_write(cmds, 2);
	
	cmds[0]=CMD_SETPRECHARGESPEEDC;
	cmds[1]=0x64;
	oled_write(cmds, 2);
	
	cmds[0]=CMD_SETPRECHARGEVOLTAGE;
	cmds[1]=0x3A;
	oled_write(cmds, 2);
	
	cmds[0]=CMD_SETVVOLTAGE;
	cmds[1]=0x3E;
	oled_write(cmds, 2);
	
	cmds[0]=CMD_MASTERCURRENTCONTROL;
	cmds[1]=0x06;
	oled_write(cmds, 2);
	
	cmds[0]=CMD_SETCONTRASTA;
	cmds[1]=0x91;
	oled_write(cmds, 2);
	
	cmds[0]=CMD_SETCONTRASTB;
	cmds[1]=0x50;
	oled_write(cmds, 2);
	
	cmds[0]=CMD_SETCONTRASTC;
	cmds[1]=0x7D;
	oled_write(cmds, 2);
	
	cmd = CMD_DEACTIVESCROLLING;
	oled_write(&cmd, 1);
	
	//clear screen
	//Clear();
	
	//write 1 to VCCEN
	
	HAL_Delay(100);
	cmd = CMD_DISPLAYON;
	oled_write(&cmd, 1);
	HAL_Delay(300);
}