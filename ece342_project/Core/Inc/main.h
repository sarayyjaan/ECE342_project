/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include "adxl362.h"
#include "spi.h"
#include <stdio.h>
#include "fonts.h"
#include "ssd1306.h"
#include <stdbool.h>
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
uint16_t swap_bytes(uint16_t x);
uint16_t return_value(uint16_t inp);
void print_msg(char * msg);
void testing_read(void);
void adxl362_read_y(int16_t *pData);
HAL_StatusTypeDef spi_read(uint8_t regAddr, uint8_t *pData, uint8_t len);
HAL_StatusTypeDef oled_write(uint8_t *cmd, uint8_t len);
void oled_init();
void oled_clear_screen();
void drawline(uint8_t c1, uint8_t r1, uint8_t c2, uint8_t r2);
void drawRectangle(uint8_t c1, uint8_t r1, uint8_t c2, uint8_t r2);
void draw0(uint8_t c1, uint8_t r1);
void draw1(uint8_t c1, uint8_t r1);
void draw2(uint8_t c1, uint8_t r1);
void draw3(uint8_t c1, uint8_t r1);
void draw4(uint8_t c1, uint8_t r1);
void draw5(uint8_t c1, uint8_t r1);
void draw6(uint8_t c1, uint8_t r1);
void draw7(uint8_t c1, uint8_t r1);
void draw8(uint8_t c1, uint8_t r1);
void draw9(uint8_t c1, uint8_t r1);
void drawNumber(int num);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define USER_Btn_Pin GPIO_PIN_13
#define USER_Btn_GPIO_Port GPIOC
#define MCO_Pin GPIO_PIN_0
#define MCO_GPIO_Port GPIOH
#define LED_display_Pin GPIO_PIN_3
#define LED_display_GPIO_Port GPIOA
#define CS_ACC_Pin GPIO_PIN_4
#define CS_ACC_GPIO_Port GPIOA
#define LD1_Pin GPIO_PIN_0
#define LD1_GPIO_Port GPIOB
#define INTMAP1_Pin GPIO_PIN_1
#define INTMAP1_GPIO_Port GPIOB
#define PMOD_EN_Pin GPIO_PIN_13
#define PMOD_EN_GPIO_Port GPIOB
#define LD3_Pin GPIO_PIN_14
#define LD3_GPIO_Port GPIOB
#define OLED_RESET_Pin GPIO_PIN_15
#define OLED_RESET_GPIO_Port GPIOB
#define STLK_RX_Pin GPIO_PIN_8
#define STLK_RX_GPIO_Port GPIOD
#define STLK_TX_Pin GPIO_PIN_9
#define STLK_TX_GPIO_Port GPIOD
#define USB_PowerSwitchOn_Pin GPIO_PIN_6
#define USB_PowerSwitchOn_GPIO_Port GPIOG
#define USB_OverCurrent_Pin GPIO_PIN_7
#define USB_OverCurrent_GPIO_Port GPIOG
#define USB_SOF_Pin GPIO_PIN_8
#define USB_SOF_GPIO_Port GPIOA
#define USB_VBUS_Pin GPIO_PIN_9
#define USB_VBUS_GPIO_Port GPIOA
#define USB_ID_Pin GPIO_PIN_10
#define USB_ID_GPIO_Port GPIOA
#define USB_DM_Pin GPIO_PIN_11
#define USB_DM_GPIO_Port GPIOA
#define USB_DP_Pin GPIO_PIN_12
#define USB_DP_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define LD2_Pin GPIO_PIN_7
#define LD2_GPIO_Port GPIOB
#define VCCEN_Pin GPIO_PIN_8
#define VCCEN_GPIO_Port GPIOB
#define D_C_Pin GPIO_PIN_9
#define D_C_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
//defining registers' addresses
#define SELF_TEST 0x2E

#define OLEDRGB_WIDTH 96
#define OLEDRGB_HEIGHT 64

#define CMD_DRAWLINE                       0x21
#define CMD_DRAWRECTANGLE                  0x22
#define CMD_SETCOLUMNADDRESS               0x15
#define CMD_SETROWADDRESS                  0x75

#define CMD_COPYWINDOW                     0x23
#define CMD_DIMWINDOW                      0x24
#define CMD_CLEARWINDOW                    0x25
#define CMD_FILLWINDOW                     0x26
    #define DISABLE_FILL    0x00
    #define ENABLE_FILL     0x01

#define CMD_CONTINUOUSSCROLLINGSETUP      0x27
#define CMD_DEACTIVESCROLLING              0x2E
#define CMD_ACTIVESCROLLING                0x2F

#define CMD_SETCOLUMNADDRESS              	0x15
#define CMD_SETROWADDRESS                 	0x75
#define CMD_SETCONTRASTA                  	0x81
#define CMD_SETCONTRASTB                  	0x82
#define CMD_SETCONTRASTC                  	0x83
#define CMD_MASTERCURRENTCONTROL          	0x87
#define CMD_SETPRECHARGESPEEDA           	0x8A
#define CMD_SETPRECHARGESPEEDB           	0x8B
#define CMD_SETPRECHARGESPEEDC           	0x8C
#define CMD_SETREMAP                       	0xA0
#define CMD_SETDISPLAYSTARTLINE          	0xA1
#define CMD_SETDISPLAYOFFSET              	0xA2
#define CMD_NORMALDISPLAY                  	0xA4
#define CMD_ENTIREDISPLAYON              	0xA5
#define CMD_ENTIREDISPLAYOFF              	0xA6
#define CMD_INVERSEDISPLAY                 	0xA7
#define CMD_SETMULTIPLEXRATIO             	0xA8
#define CMD_DIMMODESETTING                	0xAB
#define CMD_SETMASTERCONFIGURE            	0xAD
#define CMD_DIMMODEDISPLAYON             	0xAC
#define CMD_DISPLAYOFF                     	0xAE
#define CMD_DISPLAYON    					0xAF
#define CMD_POWERSAVEMODE                 	0xB0
#define CMD_PHASEPERIODADJUSTMENT         	0xB1
#define CMD_DISPLAYCLOCKDIV               	0xB3
#define CMD_SETGRAySCALETABLE            	0xB8
#define CMD_ENABLELINEARGRAYSCALETABLE  	0xB9
#define CMD_SETPRECHARGEVOLTAGE           	0xBB
#define CMD_SETVVOLTAGE                   	0xBE
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
