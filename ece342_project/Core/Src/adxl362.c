#include "adxl362.h"

#include <stdio.h>
#include <string.h>
#include "spi.h"

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

void adxl362_start(void){
	spi_write(ADXL362_REG_THRESH_ACT_L, 0xFA);
}