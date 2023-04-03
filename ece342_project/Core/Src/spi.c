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
HAL_StatusTypeDef oled_data_write(uint8_t *data, uint8_t len){
	HAL_StatusTypeDef ret;
	
	HAL_GPIO_WritePin(D_C_GPIO_Port, D_C_Pin, 1);
	//uint8_t command_byte = CMD_DISPLAYOFF;
	ret = HAL_SPI_Transmit(&hspi1, data, len, 100);
	HAL_GPIO_WritePin(LED_display_GPIO_Port, LED_display_Pin, 1);
	return ret;
	
}
void oled_init(){
	
	//bring d/c low
	HAL_GPIO_WritePin(D_C_GPIO_Port, D_C_Pin, 0);
	
	//reset high
	HAL_GPIO_WritePin(OLED_RESET_GPIO_Port , OLED_RESET_Pin, 1);
	
	//vccen low
	HAL_GPIO_WritePin(VCCEN_GPIO_Port , VCCEN_Pin, 0);
	
	//pmoden high, wait
	HAL_GPIO_WritePin(PMOD_EN_GPIO_Port, PMOD_EN_Pin, 1);
	HAL_Delay(50);
	
	//res low, wait, high
	HAL_GPIO_WritePin(OLED_RESET_GPIO_Port , OLED_RESET_Pin, 0);
	HAL_Delay(10);
	HAL_GPIO_WritePin(OLED_RESET_GPIO_Port , OLED_RESET_Pin, 1);
	HAL_Delay(10);
	
	uint8_t cmds[2];
	
	//0xFD, 0x12
	cmds[0] = 0xFD;
	cmds[1] = 0x12;
	oled_write(cmds, 2);
	
	uint8_t cmd = CMD_DISPLAYOFF;
	oled_write(&cmd, 1);
	
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
	oled_clear_screen();
	
	//write 1 to VCCEN
	HAL_GPIO_WritePin(VCCEN_GPIO_Port , VCCEN_Pin, 1);
	
	HAL_Delay(100);
	cmd = CMD_DISPLAYON;
	oled_write(&cmd, 1);
	HAL_Delay(300);
}

void oled_clear_screen(){
	uint8_t cmds[5];
	
	cmds[0] = CMD_CLEARWINDOW;
	cmds[1] = 0x00;
	cmds[2] = 0x00;
	cmds[3] = 0x5F;
	cmds[4] = 0x3F;
	
	oled_write(cmds,5);
}


void drawpixel(uint8_t c, uint8_t r)
{
	uint8_t data[2];
	uint8_t cmds[6];
	//set column start and end
	cmds[0] = CMD_SETCOLUMNADDRESS; 		
	cmds[1] = c;					// Set the starting column coordinates
	cmds[2] = OLEDRGB_WIDTH - 1;					// Set the finishing column coordinates

	//set row start and end
	cmds[3] = CMD_SETROWADDRESS; 		
	cmds[4] = r;					// Set the starting row coordinates
	cmds[5] = OLEDRGB_HEIGHT - 1;					// Set the finishing row coordinates

	data[0] = 0xFF;
	data[1] = 0xFF;
	oled_write(cmds,6);
	oled_data_write (data, 2);
	HAL_Delay(50);
	
}

void drawline(uint8_t c1, uint8_t r1, uint8_t c2, uint8_t r2)
{
	uint8_t cmds[8];
	
	cmds[0] = CMD_DRAWLINE; 	//draw line
	cmds[1] = c1;					// start column
	cmds[2] = r1;					// start row
	cmds[3] = c2;					// end column
	cmds[4] = r2;					//end row
	cmds[5] = 0x3E;	//R					
	cmds[6] = 0x3F;	//G
	cmds[7] = 0x3E;	//b
	
	oled_write(cmds,8);
	HAL_Delay(50);
	
}

void cleardisplay()
{
	uint8_t cmds[5];
	cmds[0] = CMD_CLEARWINDOW; 		// Enter the “clear mode”
	cmds[1] = 0x00;					// Set the starting column coordinates
	cmds[2] = 0x00;					// Set the starting row coordinates
	cmds[3] = OLEDRGB_WIDTH - 1;	// Set the finishing column coordinates;
	cmds[4] = OLEDRGB_HEIGHT - 1;	// Set the finishing row coordinates;
	oled_write(cmds,5);
	HAL_Delay(50);
}