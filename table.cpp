/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdlib.h>
#include "LiquidCrystal_I2C.h"
#define ADDRESS 0x4E // Write to LCD
#define COLS 20
#define ROWS 4
LiquidCrystal_LCD lcd0;
I2C_HandleTypeDef hi2c1;

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

RTC_HandleTypeDef hrtc;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_RTC_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM2_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void delay (uint16_t us) {  // microsecond delay (prescaler 720-1 MHz)
	//__HAL_TIM_SET_COUNTER(&htim1, 0);
	int moment = __HAL_TIM_GET_COUNTER(&htim1);
	while (__HAL_TIM_GET_COUNTER(&htim1) - moment < us);
}
void hello () {
	clear(&lcd0);
	home(&lcd0);
	setCursor(&lcd0, 4, 0);
	char* str = "Bauman State";
	printToLCD(&lcd0, str, 12);
	setCursor(&lcd0, 0, 1);
	str = "Technical University";
	printToLCD(&lcd0, str, 20);
	setCursor(&lcd0, 0, 2);
	str = "Gasturbinenkraftwerk";
	printToLCD(&lcd0, str, 20);
	setCursor(&lcd0, 8, 3);
	str = "E3";
	printToLCD(&lcd0, str, 2);
}

void initinftolcd () {
	setCursor(&lcd0, 0, 0);
	char* str = "vmax = ";
	printToLCD(&lcd0, str, 7);
	setCursor(&lcd0, 0, 1);
	str = "eps  = ";
	printToLCD(&lcd0, str, 7);
	setCursor(&lcd0, 0, 2);
	str = "jerk = ";
	printToLCD(&lcd0, str, 7);
	setCursor(&lcd0, 0, 3);
	str = "dir  = ";
	printToLCD(&lcd0, str, 7);
}

void inftolcd (float vlim, float eps, float v0, int dir) {
	char str1[10];
	sprintf(str1, "%d", (int)vlim);
	setCursor(&lcd0, 7, 0);
	printToLCD(&lcd0, str1, 4);
	sprintf(str1, "%d", (int)eps);
	setCursor(&lcd0, 7, 1);
	printToLCD(&lcd0, str1, 4);
	sprintf(str1, "%d", (int)v0);
	setCursor(&lcd0, 7, 2);
	printToLCD(&lcd0, str1, 3);
	setCursor(&lcd0, 7, 3);
	printToLCD(&lcd0, dir, 1);
	//if (dir){printToLCD(&lcd0, "left ", 5);}
	//else    {printToLCD(&lcd0, "right", 5);}
}
/* USER CODE END 0 */
/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

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
  MX_RTC_Init();
  MX_USART1_UART_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start(&htim1);
  HAL_TIM_Base_Start(&htim2);
  /* USER CODE END 2 */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  int32_t StepsLeft = 0;
  int dir = 0;
  char buffr[2] = "z\n";
  float vlim = 6400.0; // 6400 st/sec = 2 rev/sec = 16 mm/sec
  //float v0 = 100.0;
  float v0 = 6400.0;
  int eps = 3200.0; // 6400 st/sec^2 =
  int bSteps = (int)(1000000.0/(vlim));
  int tictok = (int)bSteps/2;
  int start = 0;
  int32_t TOC;
  uint32_t t0;
  uint32_t t;
  uint32_t dt = 1;
  float v;
  uint32_t globaltimer = 0;
  LiquidCrystal_I2C(&lcd0, &hi2c1, ADDRESS, COLS, ROWS);
  init(&lcd0);
  hello();
  HAL_Delay(1000);
  clear(&lcd0);
  initinftolcd();
  inftolcd (vlim, eps, v0, dir);

  while (1)
  {
	  /*
	  rotation speed - 2 rev/sec
	  full rev       - 3200 st/rev
	  screw          - 8 mm/rev
	  */

	  HAL_UART_Receive_IT(&huart1, (uint8_t*)buffr, 1);
	  if (buffr[0] != 'z') {
		  HAL_UART_Transmit_IT(&huart1, (uint8_t*)buffr, 2);
		  if (buffr[0] == 'l') {HAL_GPIO_WritePin(GPIOB, DIR1_Pin, RESET); dir = 0;}
		  else if (buffr[0] == 'r') {HAL_GPIO_WritePin(GPIOB, DIR1_Pin, SET); dir = 1;}
		  else if (buffr[0] == 'f') {HAL_GPIO_WritePin(GPIOB, DIR2_Pin, RESET); dir = 2;}
		  else if (buffr[0] == 'b') {HAL_GPIO_WritePin(GPIOB, DIR2_Pin, SET); dir = 3;}
		  //else if (buffr[0] == '1') {StepsLeft = 100;start = 1;}
		  //else if (buffr[0] == '2') {StepsLeft = 200;start = 1;}
		  //else if (buffr[0] == '3') {StepsLeft = 300;start = 1;}
		  else if (buffr[0] == '4') {StepsLeft = 400;start = 1;}
		  //else if (buffr[0] == '5') {StepsLeft = 500;start = 1;}
		  //else if (buffr[0] == '6') {StepsLeft = 600;start = 1;}
		  //else if (buffr[0] == '7') {StepsLeft = 700;start = 1;}
		  //else if (buffr[0] == '8') {StepsLeft = 800;start = 1;}
		  //else if (buffr[0] == '9') {StepsLeft = 900;start = 1;}
		  //else if (buffr[0] == '0') {StepsLeft = 96000;start = 1;} //30 rev
		  //else if (buffr[0] == 'h') {StepsLeft = 50;start = 1;}
		  //else if (buffr[0] == 'q') {StepsLeft = 25;start = 1;}
		  //else if (buffr[0] == 'd') {StepsLeft = 10;start = 1;}
		  //else if (buffr[0] == 'f') {StepsLeft = 5;start = 1;}
		  //else if (buffr[0] == 'o') {StepsLeft = 1;start = 1;}
		  else if (buffr[0] == 'a') {vlim += 100.0;}
		  else if (buffr[0] == 'b') {vlim -= 100.0;}
		  buffr[0] = 'z';
		  inftolcd (vlim, eps, v0, dir);
	  }
	  //StepsLeft = 400;
	  if (start == 1) {
		  start = 0;
          v = v0;
          t0 = __HAL_TIM_GET_COUNTER(&htim2) + globaltimer-1;
          TOC = 0;
	  }
	  t = __HAL_TIM_GET_COUNTER(&htim2) + globaltimer;
	  dt = t-t0;
	  if (v < vlim && StepsLeft > TOC) {
		  v += (float)(0.001*eps*dt);
		  TOC += 1;
	  }
	  if (StepsLeft < TOC) {
		  v -= (float)(0.001*eps*dt);
	  }
	  if (v < v0) {
	  	  v = v0;
	  }
	  if (v > vlim) {
	  	  v = vlim;
	  }
	  bSteps = 1000000/v;
	  tictok = bSteps/2;
	  t0 = t;
	  if (StepsLeft > 0) {
		  StepsLeft -= 1;
		  if (StepsLeft == 0) {
			  char q[2] = "z\n";
			  HAL_UART_Transmit_IT(&huart1, (uint8_t*)q, 2);
		  }
		  //
		  if (dir < 2) {
			  HAL_GPIO_TogglePin(GPIOB, ST1_Pin);
			  delay(tictok);
			  HAL_GPIO_TogglePin(GPIOB, ST1_Pin);
			  delay(tictok);
		  }
		  else {
			  HAL_GPIO_TogglePin(GPIOB, ST2_Pin);
			  delay(tictok);
			  HAL_GPIO_TogglePin(GPIOB, ST2_Pin);
			  delay(tictok);
		  }
	  }
	  globaltimer += __HAL_TIM_GET_COUNTER(&htim2);
	  __HAL_TIM_SET_COUNTER(&htim2, 0);
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */
  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
  hrtc.Init.OutPut = RTC_OUTPUTSOURCE_ALARM;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

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

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 72-1;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 0xffff-1;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 36000-1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 0xffff;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV2;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, ST1_Pin|DIR1_Pin|ST2_Pin|DIR2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : PA1 */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : ST1_Pin DIR1_Pin ST2_Pin DIR2_Pin */
  GPIO_InitStruct.Pin = ST1_Pin|DIR1_Pin|ST2_Pin|DIR2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
