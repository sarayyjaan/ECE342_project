#ifndef __ADXL362_H__
#define __ADXL362_H__
#include <stdint.h>
#include "main.h"
/*GARBAGE FILE
void adxl362_init(void); 
void adxl362_start(void);
void adxl362_read_x(int16_t *pData);
void adxl362_read_y(int16_t *pData);
uint16_t convert2stoBinary(uint16_t inp);
*/
void start_zero();
 /* ADXL362 communication commands */
 #define ADXL362_WRITE_REG               0x0A
 #define ADXL362_READ_REG                0x0B
 #define ADXL362_WRITE_FIFO              0x0D
 
 /* Registers */
 #define ADXL362_REG_DEVID_AD            0x00
 #define ADXL362_REG_DEVID_MST           0x01
 #define ADXL362_REG_PARTID              0x02
 #define ADXL362_REG_REVID               0x03
 #define ADXL362_REG_XDATA               0x08
 #define ADXL362_REG_YDATA               0x09
 #define ADXL362_REG_ZDATA               0x0A
 #define ADXL362_REG_STATUS              0x0B
 #define ADXL362_REG_FIFO_L              0x0C
 #define ADXL362_REG_FIFO_H              0x0D
 #define ADXL362_REG_XDATA_L             0x0E
 #define ADXL362_REG_XDATA_H             0x0F
 #define ADXL362_REG_YDATA_L             0x10
 #define ADXL362_REG_YDATA_H             0x11
 #define ADXL362_REG_ZDATA_L             0x12
 #define ADXL362_REG_ZDATA_H             0x13
 #define ADXL362_REG_TEMP_L              0x14
 #define ADXL362_REG_TEMP_H              0x15
 #define ADXL362_REG_SOFT_RESET          0x1F
 #define ADXL362_REG_THRESH_ACT_L        0x20
 #define ADXL362_REG_THRESH_ACT_H        0x21
 #define ADXL362_REG_TIME_ACT            0x22
 #define ADXL362_REG_THRESH_INACT_L      0x23
 #define ADXL362_REG_THRESH_INACT_H      0x24
 #define ADXL362_REG_TIME_INACT_L        0x25
 #define ADXL362_REG_TIME_INACT_H        0x26
 #define ADXL362_REG_ACT_INACT_CTL       0x27
 #define ADXL362_REG_FIFO_CTL            0x28
 #define ADXL362_REG_FIFO_SAMPLES        0x29
 #define ADXL362_REG_INTMAP1             0x2A
 #define ADXL362_REG_INTMAP2             0x2B
 #define ADXL362_REG_FILTER_CTL          0x2C
 #define ADXL362_REG_POWER_CTL           0x2D
 #define ADXL362_REG_SELF_TEST           0x2E
 /*	Initialization Parameters
*/
const uint8_t FREE_FALL_THRESH = 0x96;  	//Sets free-fall detection threshold to 600mg
const uint8_t INACT_STILL_THRESH = 0xFA;	//Sets still motion reference threshold to 1g
const uint8_t FREE_FALL_TIME = 0x03;    	//Sets free-fall detection time to 30ms
const uint8_t INACT_STILL_TIME_L = 0x10;	//lower half of 10 second still time
const uint8_t INACT_STILL_TIME_H = 0x27;	//upper half of 10 second still time
const uint8_t REF_INACT_ENABLE = 0x0C; 		//Enable referenced inactivity detection
const uint8_t SET_INACT_INTERUPT = 0x20;	//Sets the inactivity interrupt to the interrupt 1 pin
const uint8_t SENSOR_RANGE_8 = 0x83;      	//Sets sensor range to 8g with 100Hz ODR
const uint8_t SENSOR_RANGE_4 = 0x43;     	//Sets sensor range to 4g with 100Hz ODR
const uint8_t SENSOR_RANGE_2 = 0x03;      	//Sets sensor range to 2g with 100Hz ODR
const uint8_t BEGIN_MEASURE = 0x02;     		//Begins measurement


 #endif