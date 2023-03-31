#include "adxl362.h"
#include <stdio.h>
#include <string.h>
#include "spi.h"

extern SPI_HandleTypeDef hspi1;
void adxl362_init(void){
	
	//adxl345, modify pwr_ctl writing 1, 
	//set the measure bit, reset sleep bit in power_ctl
	//data format register set the range
	uint8_t setting = 0xFA;
	spi_write(ADXL362_REG_THRESH_INACT_L, &setting); //inactivity
	setting = 0x83;
	spi_write(ADXL362_REG_FILTER_CTL, &setting); //general setting (filter ctl)
	setting = 0x02;
	spi_write(ADXL362_REG_POWER_CTL, &setting); //turn on measure
}

/*void adxl362_start(void){
	spi_write(ADXL362_REG_THRESH_ACT_L, 0xFA);
}*/
/*
 #define ADXL362_REG_YDATA_L             0x10
 #define ADXL362_REG_YDATA_H             0x11
*/
void adxl362_read_y(int16_t *pData){
	uint8_t LSB;
	uint8_t MSB;
	uint16_t buff;
	uint8_t regAddr_l = 0x10;
	uint8_t regAddr_h = 0x11;
	HAL_StatusTypeDef ret;
	HAL_GPIO_WritePin(CS_ACC_GPIO_Port, CS_ACC_Pin, GPIO_PIN_RESET); // pull the CS low to enable the slave
	
	//reading the LSB
	uint8_t command_byte = 0x0B;
	
	ret = HAL_SPI_Transmit(&hspi1, &command_byte, 1, 100); //specific to the adxl
	
	ret = HAL_SPI_Transmit(&hspi1, &regAddr_l, 1, 100); //transmit the address where we want to read data
	
	ret = HAL_SPI_Receive(&hspi1, &LSB, 1, 100); //receive the data
	char * msg;
	sprintf(msg, "in read data (%d)\r\n", LSB);
	print_msg(msg);
	buff = LSB << 8;
	//reading the MSB
	
	ret = HAL_SPI_Receive(&hspi1, &MSB, 1, 100) | buff; //receive the data
	sprintf(msg, "in read data (%d)\r\n", MSB);
	print_msg(msg);
	buff = buff | MSB;
	//sign  extension remove first 4 bits
	if(buff>=80){
		buff = convert2stoBinary(buff);
		buff = buff & 0x07ff;
		buff = 0 - buff;
	}else{
		buff = buff & 0x07ff;
	}
	HAL_GPIO_WritePin(CS_ACC_GPIO_Port, CS_ACC_Pin, GPIO_PIN_SET); //pull the CS pin high to disable slave

}

uint16_t convert2stoBinary(uint16_t inp){
	inp = inp ^ 0x07ff; //xor with 1st 12 bits
	inp = inp - 1;
	return inp;
}