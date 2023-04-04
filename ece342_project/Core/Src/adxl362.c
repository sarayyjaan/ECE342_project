#include "adxl362.h"
#include <stdio.h>
#include <string.h>
#include "spi.h"

extern SPI_HandleTypeDef hspi1;
extern int xZero, yZero, zZero;
uint16_t convert2stoBinary(uint16_t inp){
	inp = inp ^ 0x07ff; //xor with 1st 12 bits
	inp = inp - 1;
	return inp;
}
void adxl362_init(void){
	
	uint8_t setting = INACT_STILL_THRESH;
	spi_write(ADXL362_REG_THRESH_INACT_L, &setting); //sets stationary threshold to 1g
	setting = 0x03; //sets range to 2g
	spi_write(ADXL362_REG_FILTER_CTL, &setting); //sets the sensor range to 2g
	setting = BEGIN_MEASURE;
	spi_write(ADXL362_REG_POWER_CTL, &setting); //turn on measure
}
int16_t adxl362_get_x(){
	int16_t pData;
	spi_read(0x0E, &pData, 2); //xdata
	pData = swap_bytes(pData);
	pData = return_value(pData);
	//process to achieve desired reading
	
	pData = pData + xZero;
	return pData;
}
int16_t adxl362_get_y(){
	int16_t pData;
	spi_read(0x10, &pData, 2); //xdata
	pData = swap_bytes(pData);
	pData = return_value(pData);
	pData = pData + yZero;
	return pData;
}
int16_t adxl362_get_z(){
	int16_t pData;
	spi_read(0x12, &pData, 2); //xdata
	pData = swap_bytes(pData);
	pData = return_value(pData);
	pData = pData + zZero;
	return pData;
}
void start_zero(){
	//math sets the current acceleration to read zero
	int xAvg = 0;
	int yAvg = 0;
	int zAvg = 0;
	for(int i = 0; i < 100; i ++){
			xAvg =  xAvg + adxl362_get_x();
			yAvg = yAvg + adxl362_get_y();
			zAvg = zAvg + adxl362_get_z();
	}
	xAvg = xAvg / 100;
	yAvg = yAvg / 100;
	zAvg = zAvg / 100;
	
	xZero = 0 - (xAvg - xZero);
	yZero = 0 - (yAvg - yZero);
	zZero = 1000 - (zAvg - zZero);	
}


int16_t adxl362_read_x(int16_t *pData){
	spi_read(0x0E, &pData, 2); //xdata
	*pData = swap_bytes(*pData);
	*pData = return_value(*pData);
	return *pData;
}

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
