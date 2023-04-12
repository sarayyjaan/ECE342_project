/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <string.h>
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <math.h>
#include "adxl362.h"
#include "ssd1306.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define SAMPLE_LENGTH 1000
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c2;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim6;

UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart3_tx;

PCD_HandleTypeDef hpcd_USB_OTG_FS;

/* USER CODE BEGIN PV */
I2C_HandleTypeDef hi2c1;
int x_buff[20];
int y_buff[20];
int z_buff[20];
int8_t x_accl[20], y_accl[20], z_accl[20];
int totave[20], totvect[20];
int step_count = 0;
int threshhold = 10;
int flag = 0;
int x_avg, y_avg, z_avg;
int xZero, yZero, zZero;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_I2C2_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM6_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_USB_OTG_FS_PCD_Init(void);
/* USER CODE BEGIN PFP */
float mean(float data[], int len);
void thresholding(int8_t y[], int lag, float threshold, float influence);
void print_msg(char * msg);
void testing_read(void);
uint16_t twosToBin(uint16_t input);
uint16_t return_value(uint16_t inp);
uint16_t swap_bytes(uint16_t x);
float stddev(float data[], int len);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
char msg[100];
void thresholding(int8_t y[], int lag, float threshold, float influence) {
    //memset(signals, 0, sizeof(int) * SAMPLE_LENGTH);
    float filteredY[20];
    memcpy(filteredY, y, sizeof(float) * SAMPLE_LENGTH);
    float avgFilter[20];
    float stdFilter[20];

    avgFilter[lag - 1] = mean(y, lag);
    stdFilter[lag - 1] = stddev(y, lag);

    for (int i = lag; i < SAMPLE_LENGTH; i++) {
        if (fabsf(y[i] - avgFilter[i-1]) > threshold * stdFilter[i-1]) {
            if (y[i] > avgFilter[i-1]) {
              //  step_count++;
							//sprintf(msg, "%d", step_count);
							//print_msg(msg);
            } else {
                //signals[i] = -1;
            }
            filteredY[i] = influence * y[i] + (1 - influence) * filteredY[i-1];
        } else {
            //signals[i] = 0;
        }
        avgFilter[i] = mean(filteredY + i-lag, lag);
        stdFilter[i] = stddev(filteredY + i-lag, lag);
    }
}
float mean(float data[], int len) {
    float sum = 0.0, mean = 0.0;
    int i;
    for(i=0; i<len; ++i) {
        sum += data[i];
    }
    mean = sum/len;
    return mean;
}

float stddev(float data[], int len) {
    float the_mean = mean(data, len);
    float standardDeviation = 0.0;
    int i;
    for(i=0; i<len; ++i) {
        standardDeviation += pow(data[i] - the_mean, 2);
    }
    return sqrt(standardDeviation/len);
}

void print_msg(char * msg) {
  HAL_UART_Transmit(&huart3, (uint8_t *)msg, strlen(msg), 100);
}

void testing_read(void){
	print_msg("Test\n");
	uint8_t value=1;
	spi_read_old(0x00, &value);
	char msg[100];
	
	if(value == 0xAD){
		print_msg("seems ok\n");
		sprintf(msg, "tis this (0x%x)\r\n", value);
		print_msg(msg);
	}
	else{
		sprintf(msg, "Wrong product id (0x%x)\r\n", value);
	}
}

uint16_t twosToBin(uint16_t input){	
	//flip all 11 bits
	input = input ^ 0x07ff;
	
	//subtract 1 to get binary
	input = input - 1;
	
	return input;	
}
uint16_t return_value(uint16_t inp){
	int sign = 0; 
	int result = 0;
	if(inp >= 0x8000){// If negative
		sign = 1;
		inp = twosToBin(inp);
	}
	//Get rid of first 4 bits
	result = inp & 0x07ff;

	if(sign == 1){
		result = 0 - result;
	}
	
	return result;	
};

void calibrate(){
	//print_msg("Calibrating ..\n");
	int data = 0;
	for (int i = 0;i<20;i++){
		spi_read_old(ADXL362_REG_XDATA, &data);
		x_buff[i] = (int8_t)data;
			/*spi_read(0x0E, &data, 2); //xdata
			data = swap_bytes(data);
			x_buff[i] = return_value(data);
			*/
		x_avg = x_buff[i] + x_avg;
	}
	x_avg = x_avg / 20;
	for (int i = 0;i<20;i++){
		spi_read_old(ADXL362_REG_YDATA, &data);
		y_buff[i] = (int8_t)data;
			/*spi_read(0x10, &data, 2); //xdata
			data = swap_bytes(data);
			y_buff[i] = return_value(data*/
		y_avg = y_buff[i] + y_avg;
	}
	y_avg = y_avg / 20;
	for (int i = 0;i<20;i++){
		spi_read_old(ADXL362_REG_ZDATA, &data);
			z_buff[i] = (int8_t)data;
			/*spi_read(0x12, &data, 2); //xdata
			data = swap_bytes(data);
			z_buff[i] = return_value(data);*/
			z_avg = z_buff[i] + z_avg;
		}
	z_avg = z_avg / 20;
}


uint16_t swap_bytes(uint16_t x) {
	
  uint16_t bitmask = 0x00ff;
  uint16_t temp = x & bitmask; //lsbs
  x = x >> 8;
  temp = temp << 8;
  x = x | temp;
  return x;
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	int16_t val;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2C2_Init();
  MX_SPI1_Init();
  MX_TIM1_Init();
  MX_TIM6_Init();
  MX_USART3_UART_Init();
  MX_USB_OTG_FS_PCD_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	testing_read(); //tests that device reg of accelerometer can be read
	uint8_t value=1;
	char msg[100];
	char display[100];
	//configure?
	/*uint8_t setting = 0x05;
	spi_write(0x20, &setting); //threshold active L
	setting = 0x00;
	spi_write(0x21, &setting); //threshold active H	
	setting = 0x02;
	spi_write(0x22, &setting); //time act
	setting = 0xFA;
	spi_write(0x23, &setting); //inactivity
	setting = 0x02;
	spi_write(0x25, &setting); //inactivity time
	setting = 0x03;
	spi_write(0x27, &setting); //act/inactivity control reg
	setting = 0x03;
	setting = 0x83; //SENSOR_RANGE_2 = 0x03, SENSOR_RANGE_8 = 0x83
	spi_write(0x2c, &setting); //general setting (filter ctl)
	setting = 0x02;
	spi_write(0x2D, &setting); //turn on measure*/
	adxl362_init();
	adxl362_activity_config();
	//self test mode
	print_msg("call self test\n");
	uint8_t self_test=0x01;
	spi_write(SELF_TEST,&self_test);
	//print_msg("reading x\n");
	
	spi_read_old(ADXL362_REG_XDATA, &value); //xdata
	//sprintf(msg, "x data (%d)(0x%x)\r\n", (int) value, value);
	//print_msg(msg);
	//29,109, 169
	spi_read_old(ADXL362_REG_YDATA, &value); //ydata
	//sprintf(msg, "y data (%d)\r\n", (int) value);
	//print_msg(msg);
	
	spi_read_old(ADXL362_REG_ZDATA, &value); //zdata
	//sprintf(msg, "z data (%d)\r\n", (int) value);
	//print_msg(msg);
	
	//deassert st
	self_test=0x00;
	spi_write(0x2E,&self_test);
	uint16_t data=0;
	
	oled_init();
	HAL_Delay(500);
	cleardisplay();
	//drawpixel(2, 2);
	HAL_Delay(500);
	//oled_clear_screen();
	
	extern FontDef_t Font_7x10;
	SSD1306_COLOR_t white = SSD1306_COLOR_WHITE;
	
	SSD1306_Puts("Steps:", &Font_7x10, white);
	HAL_Delay(1000);
	//calibrating to get avg values
	start_zero();
	sprintf(msg, "zero x: %d, y: %d, z:%d\n",(int)xZero, yZero, zZero);
	print_msg(msg);
	calibrate_msb();
	sprintf(msg, "zero x: %d, y: %d, z:%d\n",(int)xZero, yZero, zZero);
	print_msg(msg);
	calibrate();
	sprintf(msg, "avg x: %d, y: %d, z:%d\n",(int)x_avg, y_avg, z_avg);
	print_msg(msg);
	print_msg("Done calibrating\n");
	
	int a=0;
	int num = 0;
	int lag = 20;
		float threshold = 5;
    float influence = 0;
    uint8_t z[10];
	print_msg("time,x_axis,y_axis,z_axis\n");
  while (1)
  {
		start_zero();
		calibrate_msb();
		calibrate();
		sprintf(msg, "zero x: %d, y: %d, z:%d\n",(int)xZero, yZero, zZero);
	print_msg(msg);
		sprintf(msg, "avg x: %d, y: %d, z:%d\n",(int)x_avg, y_avg, z_avg);
	print_msg(msg);

		HAL_Delay(1000);
		for (int i = 0; i < 10; i++)
		{
			HAL_Delay(100);
			//reading 12 bits
			/*
			x_accl[i] = adxl362_get_x();
			y_accl[i] = adxl362_get_y();
			z_accl[i] = adxl362_get_z();
			sprintf(msg, "%d,%d,%d,%d\n", num, x_accl[i],y_accl[i],z_accl[i]);
			print_msg(msg);
			*/
			//reading from 8 bit registers
			spi_read_old(ADXL362_REG_XDATA, &value); //xdata                       
			x_accl[i] = (int8_t)value;
			spi_read_old(ADXL362_REG_YDATA, &value); //ydata
			y_accl[i] = (int8_t)value;
			spi_read_old(ADXL362_REG_ZDATA, &value); //zdata
			z_accl[i] = (int8_t)value;
			sprintf(msg, "%d,%d,%d,%d\n", num, x_accl[i],y_accl[i],z_accl[i]);
			print_msg(msg);
			//computing the dot products
			totvect[i] = sqrt(((x_accl[i] - x_avg)*(x_accl[i] - xZero)) 
			+ ((y_accl[i] - y_avg) * (y_accl[i] - yZero)) 
			+ ((z_accl[i] - z_avg) * (z_accl[i] - zZero))); //should not be moving in the z direction
			num++;
			totave[i] = (totvect[i] + totvect[i - 1]) / 2 ;
			sprintf(msg, "totave: %d\n",(int)totave[i]);
			print_msg(msg);
			
    //HAL_Delay(100);
    if((totave[i]>threshhold && flag==0))
    {
			print_msg("Step taken\n");
      step_count++;
			cleardisplay();
			SSD1306_GotoXY(0,0);
			SSD1306_Puts("Steps:", &Font_7x10, white);
			sprintf(display, "%d",a);
			SSD1306_Puts(display, &Font_7x10, white);
			drawNumber(step_count);
      flag=1;
    }
    else if (totave[i] > threshhold && flag==1)
    {
        //do nothing; already considered this count
    }
    if (totave[i] <threshhold  && flag==1)
    {
      flag=0; //deassert
    }
	//	thresholding(z_accl, lag, threshold, influence);
		spi_read_old(ADXL362_REG_STATUS, &value);
		value = value & 0x10;

		if(value == 0x10 && (abs(x_accl[i])>0x09 || abs(y_accl[i]>0x09))){

			//print_msg("ACT Bit = 1\n");
			//HAL_Delay(500);
			a++;
			
			cleardisplay();
			SSD1306_GotoXY(0,0);
			SSD1306_Puts("Steps:", &Font_7x10, white);
			sprintf(display, "%d",a);
			SSD1306_Puts(display, &Font_7x10, white);
			drawNumber(step_count);
		}
		else{
			//print_msg("ACT Bit=0\n");
		}
		//HAL_Delay(500);
	}
		sprintf(msg, "step_count: %d\n",(int)step_count);
		//print_msg(msg);
	if( 0 == 1){
		spi_read(0x0E, &data, 2); //xdata
		data = return_value(data);
		sprintf(msg, "\nx data (%d)(0x%x)\r\n", (int) data, data);
		print_msg(msg);
		//x_data = x_data>>4;
		/*x_data=(x_data*1000)/(2000/8);
		sprintf(msg, "processing x data (%d)\r\n", x_data);
		print_msg(msg);*/
	
	spi_read(0x10, &data, 2); //ydata
	data = return_value(data);
	sprintf(msg, "y data (%d)(0x%x)\r\n", (int) data, data);
	print_msg(msg);
	
	//adxl362_read_y(&val);
	//sprintf(msg, "y data (%d)\r\n", val);
	//print_msg(msg);
	spi_read(0x12, &data, 2); //zdata
	data = return_value(data);
	sprintf(msg, "z data (%d)\r\n", (int) data);
	print_msg(msg);
	}
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 100000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 20;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 10;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 0;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 65535;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief USB_OTG_FS Initialization Function
  * @param None
  * @retval None
  */
static void MX_USB_OTG_FS_PCD_Init(void)
{

  /* USER CODE BEGIN USB_OTG_FS_Init 0 */

  /* USER CODE END USB_OTG_FS_Init 0 */

  /* USER CODE BEGIN USB_OTG_FS_Init 1 */

  /* USER CODE END USB_OTG_FS_Init 1 */
  hpcd_USB_OTG_FS.Instance = USB_OTG_FS;
  hpcd_USB_OTG_FS.Init.dev_endpoints = 6;
  hpcd_USB_OTG_FS.Init.speed = PCD_SPEED_FULL;
  hpcd_USB_OTG_FS.Init.dma_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.phy_itface = PCD_PHY_EMBEDDED;
  hpcd_USB_OTG_FS.Init.Sof_enable = ENABLE;
  hpcd_USB_OTG_FS.Init.low_power_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.lpm_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.vbus_sensing_enable = ENABLE;
  hpcd_USB_OTG_FS.Init.use_dedicated_ep1 = DISABLE;
  if (HAL_PCD_Init(&hpcd_USB_OTG_FS) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USB_OTG_FS_Init 2 */

  /* USER CODE END USB_OTG_FS_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LED_display_Pin|CS_ACC_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LD1_Pin|INTMAP1_Pin|PMOD_EN_Pin|LD3_Pin
                          |OLED_RESET_Pin|LD2_Pin|VCCEN_Pin|D_C_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(USB_PowerSwitchOn_GPIO_Port, USB_PowerSwitchOn_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : USER_Btn_Pin */
  GPIO_InitStruct.Pin = USER_Btn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USER_Btn_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LED_display_Pin CS_ACC_Pin */
  GPIO_InitStruct.Pin = LED_display_Pin|CS_ACC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LD1_Pin INTMAP1_Pin PMOD_EN_Pin LD3_Pin
                           OLED_RESET_Pin LD2_Pin VCCEN_Pin D_C_Pin */
  GPIO_InitStruct.Pin = LD1_Pin|INTMAP1_Pin|PMOD_EN_Pin|LD3_Pin
                          |OLED_RESET_Pin|LD2_Pin|VCCEN_Pin|D_C_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_PowerSwitchOn_Pin */
  GPIO_InitStruct.Pin = USB_PowerSwitchOn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(USB_PowerSwitchOn_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_OverCurrent_Pin */
  GPIO_InitStruct.Pin = USB_OverCurrent_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USB_OverCurrent_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
