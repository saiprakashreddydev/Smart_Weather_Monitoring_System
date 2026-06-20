/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd.h"

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
ADC_HandleTypeDef hadc1;

I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart6;

osThreadId Task1Handle;
osThreadId Task4Handle;
osThreadId Task3Handle;
osThreadId Task2Handle;
osSemaphoreId myBinarySem01Handle;
/* USER CODE BEGIN PV */
extern int _5sec_flag;
extern int _1sec_flag;
extern int enter;
unsigned char rtc_data[7]={0};
int Flag=0;
int i=0;
int count=0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART6_UART_Init(void);
void Task1_ReadRTC_WriteLCD_1Sec(void const * argument);
void Task4_SWMS_Config(void const * argument);
void Task3_Check_InternetStatus(void const * argument);
void Task2_ReadLM35_WriteLCD_5Sec(void const * argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
unsigned char rtc_write[]={0x00,0x13,0x11,0x05,0x06,0x12,0x24};
unsigned char rtc_read[7];
unsigned int lm35_data,b;
unsigned int lm35_flag=0;
unsigned char Fetch[100];
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
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_USART6_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_LCD_Init();
  wifi_init();
 //HAL_I2C_Mem_Write(&hi2c1,0x68<<1,0x00,0x01,&rtc_write,7,100);
  /* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* definition and creation of myBinarySem01 */
  osSemaphoreDef(myBinarySem01);
  myBinarySem01Handle = osSemaphoreCreate(osSemaphore(myBinarySem01), 1);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of Task1 */
  osThreadDef(Task1, Task1_ReadRTC_WriteLCD_1Sec, osPriorityIdle, 0, 128);
  Task1Handle = osThreadCreate(osThread(Task1), NULL);

  /* definition and creation of Task4 */
  osThreadDef(Task4, Task4_SWMS_Config, osPriorityIdle, 0, 128);
  Task4Handle = osThreadCreate(osThread(Task4), NULL);

  /* definition and creation of Task3 */
  osThreadDef(Task3, Task3_Check_InternetStatus, osPriorityIdle, 0, 128);
  Task3Handle = osThreadCreate(osThread(Task3), NULL);

  /* definition and creation of Task2 */
  osThreadDef(Task2, Task2_ReadLM35_WriteLCD_5Sec, osPriorityNormal, 0, 128);
  Task2Handle = osThreadCreate(osThread(Task2), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Start scheduler */
  osKernelStart();
  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_10;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

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
  * @brief USART6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART6_UART_Init(void)
{

  /* USER CODE BEGIN USART6_Init 0 */

  /* USER CODE END USART6_Init 0 */

  /* USER CODE BEGIN USART6_Init 1 */

  /* USER CODE END USART6_Init 1 */
  huart6.Instance = USART6;
  huart6.Init.BaudRate = 115200;
  huart6.Init.WordLength = UART_WORDLENGTH_8B;
  huart6.Init.StopBits = UART_STOPBITS_1;
  huart6.Init.Parity = UART_PARITY_NONE;
  huart6.Init.Mode = UART_MODE_TX_RX;
  huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart6.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart6) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART6_Init 2 */

  /* USER CODE END USART6_Init 2 */

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
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LCD_D0_Pin|LCD_D1_Pin|LCD_D2_Pin|LCD_D3_Pin
                          |RS_Pin|RW_Pin|EN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : LCD_D0_Pin LCD_D1_Pin LCD_D2_Pin LCD_D3_Pin
                           RS_Pin RW_Pin EN_Pin */
  GPIO_InitStruct.Pin = LCD_D0_Pin|LCD_D1_Pin|LCD_D2_Pin|LCD_D3_Pin
                          |RS_Pin|RW_Pin|EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : SW_UP_Pin SW_DW_Pin */
  GPIO_InitStruct.Pin = SW_UP_Pin|SW_DW_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : SW_ENT_Pin */
  GPIO_InitStruct.Pin = SW_ENT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(SW_ENT_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void wifi_init(void)
{
			HAL_UART_Transmit(&huart6,"AT\r\n",5,100);
	  	    HAL_Delay(100);
	  	    HAL_UART_Transmit(&huart6,"AT+RST\r\n",9,100);
	  	    HAL_Delay(100);
	  	    HAL_UART_Transmit(&huart6,"AT+CWMODE=3\r\n",14,100);
	  	    HAL_Delay(100);
	  	    HAL_UART_Transmit(&huart6,"AT+CWJAP=\"SAI\",\"12345678\"\r\n",strlen("AT+CWJAP=\"SAI\",\"12345678\"\r\n"),100);
	  	    HAL_Delay(100);
	  	    HAL_UART_Transmit(&huart6,"AT+CIFSR\r\n",11,100);
	  	    HAL_Delay(100);
	  	    HAL_UART_Transmit(&huart6,"AT+CIPMUX=0\r\n",14,100);
	  	    HAL_Delay(100);
	  	    HAL_UART_Transmit(&huart6,"AT+CWLAP\r\n",11,100);
	  	    HAL_Delay(100);
	  	    HAL_UART_Transmit(&huart6,"AT+CIPMODE=0\r\n",15,100);
}
void SWMS_Config1(void)
{

  while((HAL_GPIO_ReadPin(GPIOC,SW_ENT_Pin))!=0)
		{
			if((HAL_GPIO_ReadPin(GPIOC,SW_UP_Pin))==0)
			{
				if(rtc_data[i]%10==9)
				{
					count++;
					rtc_data[i]++;
				}
				else
				rtc_data[i]++;
				HAL_LCD_WRITE_DATA((rtc_data[i]/10)+48);
				HAL_LCD_WRITE_DATA((rtc_data[i]%10)+48);
				  while((HAL_GPIO_ReadPin(GPIOC,SW_UP_Pin))==0);
				HAL_LCD_WRITE_COMMAND(0x10);
				HAL_LCD_WRITE_COMMAND(0x10);
			}
			if((HAL_GPIO_ReadPin(GPIOC,SW_DW_Pin))==0)
			{
				if(rtc_data[i]%10==9)
				{
					count--;
					rtc_data[i]--;
				}
				else
				rtc_data[i]--;
				HAL_LCD_WRITE_DATA((rtc_data[i]/10)+48);
				HAL_LCD_WRITE_DATA((rtc_data[i]%10)+48);
				  while((HAL_GPIO_ReadPin(GPIOC,SW_DW_Pin))==0);
				HAL_LCD_WRITE_COMMAND(0x10);
				HAL_LCD_WRITE_COMMAND(0x10);
			}
		}
		HAL_Delay(500);
		HAL_Delay(500);
		Flag++;
		rtc_data[i]-=count;
		while(count)
		{
			rtc_data[i]+=7;
			count--;
		}
		i++;
}
void SW_Enter(void)
{
	switch(Flag)
		{
			case 0:HAL_LCD_WRITE_COMMAND(0x81);
						HAL_LCD_STRING("SS:MM:HH");
						HAL_LCD_WRITE_COMMAND(0xC1);
						HAL_LCD_STRING("00");
						HAL_LCD_WRITE_COMMAND(0xC1);
							break;
			case 1:HAL_LCD_WRITE_COMMAND(0xC4);
						HAL_LCD_STRING("00");
						HAL_LCD_WRITE_COMMAND(0xC4);
							break;
			case 2:HAL_LCD_WRITE_COMMAND(0xC7);
						HAL_LCD_STRING("00");
						HAL_LCD_WRITE_COMMAND(0xC7);
							break;
			case 3:HAL_LCD_WRITE_COMMAND(0x01);
						HAL_LCD_WRITE_COMMAND(0x81);
						HAL_LCD_STRING("DY:DD:MM:YY");
						HAL_LCD_WRITE_COMMAND(0xC1);
						HAL_LCD_STRING("00");
						HAL_LCD_WRITE_COMMAND(0xC1);
						  break;
			case 4:HAL_LCD_WRITE_COMMAND(0xC4);
						HAL_LCD_STRING("00");
						HAL_LCD_WRITE_COMMAND(0xC4);
						  break;
			case 5:HAL_LCD_WRITE_COMMAND(0xC7);
						HAL_LCD_STRING("00");
						HAL_LCD_WRITE_COMMAND(0xC7);
							break;
			case 6:HAL_LCD_WRITE_COMMAND(0xCA);
						HAL_LCD_STRING("00");
						HAL_LCD_WRITE_COMMAND(0xCA);
							break;
			case 7:HAL_LCD_WRITE_COMMAND(0x01);
						HAL_LCD_WRITE_COMMAND(0x82);
						HAL_LCD_WRITE_COMMAND(0x01);
						HAL_I2C_Mem_Write(&hi2c1,0x68<<1,0x00,0x01,rtc_data,7,100);
						HAL_LCD_STRING("Data Written");
						HAL_LCD_WRITE_COMMAND(0x01);
						break;
		}
}

/* USER CODE END 4 */

/* USER CODE BEGIN Header_Task1_ReadRTC_WriteLCD_1Sec */
/**
  * @brief  Function implementing the Task1 thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_Task1_ReadRTC_WriteLCD_1Sec */
void Task1_ReadRTC_WriteLCD_1Sec(void const * argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
	  if(_1sec_flag==1)
	  {
	  	  osSemaphoreWait(myBinarySem01Handle,osWaitForever);
	  	  	  	HAL_LCD_WRITE_COMMAND(0x80);
	  	  	  	HAL_I2C_Mem_Read(&hi2c1,0x68<<1,0x00,0x01,rtc_read,7,100);
	  	 	   	   	   // HH:MM:SS
	  	  	  	HAL_LCD_WRITE_COMMAND(0x81);
	  	  	  	      // hours
	  	  	  	HAL_LCD_WRITE_DATA((rtc_read[2] >> 4) + '0');
	  	  	  	HAL_LCD_WRITE_DATA((rtc_read[2] & 0x0F) + '0');
	  	  	  	HAL_LCD_WRITE_DATA(':');
	  	  	  	       // minutes
	  	  	  	HAL_LCD_WRITE_DATA((rtc_read[1] >> 4) + '0');
	  	  	  	HAL_LCD_WRITE_DATA((rtc_read[1] & 0x0F) + '0');
	  	  	  	HAL_LCD_WRITE_DATA(':');
	  	  	  	       // seconds
	  	  	  	HAL_LCD_WRITE_DATA((rtc_read[0] >> 4) + '0');
	  	  	  	HAL_LCD_WRITE_DATA((rtc_read[0] & 0x0F) + '0');
	  	  	  	       // Date: DD/MM/YY
	  	  	  	HAL_LCD_WRITE_COMMAND(0xC1);
	  	  	  	      //date
	  	  	  	HAL_LCD_WRITE_DATA((rtc_read[4] >> 4) + '0');
	  	  	  	HAL_LCD_WRITE_DATA((rtc_read[4] & 0x0F) + '0');
	  	  	  	HAL_LCD_WRITE_DATA('/');
	  	  	  	       // month
	  	  	  	HAL_LCD_WRITE_DATA((rtc_read[5] >> 4) + '0');
	  	  	  	HAL_LCD_WRITE_DATA((rtc_read[5] & 0x0F) + '0');
	  	  	  	HAL_LCD_WRITE_DATA('/');
	  	  	  	      //year
	  	  	  	HAL_LCD_WRITE_DATA((rtc_read[6] >> 4) + '0');
	  	  	  	HAL_LCD_WRITE_DATA((rtc_read[6] & 0x0F) + '0');
                _1sec_flag=0;
	   	osSemaphoreRelease(myBinarySem01Handle);
                 osDelay(1);
	  }
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_Task4_SWMS_Config */
/**
* @brief Function implementing the Task4 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Task4_SWMS_Config */
void Task4_SWMS_Config(void const * argument)
{
  /* USER CODE BEGIN Task4_SWMS_Config */
  /* Infinite loop */
  for(;;)
  {
	  if(enter==1)
	  {
		  osSemaphoreWait(myBinarySem01Handle,osWaitForever);
		  HAL_LCD_WRITE_COMMAND(0x01);
		  	while(1)
		  	{
		  		SW_Enter();
		  		if(Flag==7)
		  		{
		  			Flag=0;
		  			break ;
		  		}
		  		SWMS_Config1();
		  	}
		 osSemaphoreRelease(myBinarySem01Handle);
		 enter=0;
		 osDelay(1);
	  }
  }
  /* USER CODE END Task4_SWMS_Config */
}

/* USER CODE BEGIN Header_Task3_Check_InternetStatus */
/**
* @brief Function implementing the Task3 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Task3_Check_InternetStatus */
void Task3_Check_InternetStatus(void const * argument)
{
  /* USER CODE BEGIN Task3_Check_InternetStatus */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END Task3_Check_InternetStatus */
}

/* USER CODE BEGIN Header_Task2_ReadLM35_WriteLCD_5Sec */
/**
* @brief Function implementing the Task2 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Task2_ReadLM35_WriteLCD_5Sec */
void Task2_ReadLM35_WriteLCD_5Sec(void const * argument)
{
  /* USER CODE BEGIN Task2_ReadLM35_WriteLCD_5Sec */
  /* Infinite loop */
  for(;;)
  {
	  if(_5sec_flag==1)
	  {
	  osSemaphoreWait(myBinarySem01Handle,osWaitForever);
		HAL_ADC_Start(&hadc1);
	  	HAL_ADC_PollForConversion(&hadc1,20);
	  	lm35_data=HAL_ADC_GetValue(&hadc1);
	  	lm35_data=((lm35_data*3000)/40960);
	  	HAL_LCD_WRITE_COMMAND(0x8C);
	  	b=lm35_data/10;
	  	HAL_LCD_WRITE_DATA(b+48);
	  	b=lm35_data%10;
	  	HAL_LCD_WRITE_DATA(b+48);
	  	HAL_LCD_WRITE_DATA(0xDF);
	  	HAL_LCD_WRITE_DATA('C');
		HAL_UART_Transmit(&huart6,"AT+CIPSTATUS\r\n",15,100);
	    HAL_Delay(100);
	    HAL_UART_Transmit(&huart6,"AT+CIPSTART=\"TCP\",\"142.93.218.33\",80\r\n",39,100);
	    HAL_Delay(100);
	    HAL_UART_Transmit(&huart6,"AT+CIPSEND=40\r\n",16,100);
	    sprintf(Fetch,"GET /page.php?temp=%d&hum=30&dev=36\r\n",lm35_data);
	    HAL_Delay(100);
	    HAL_UART_Transmit(&huart6,Fetch,sizeof(Fetch),100);
	    HAL_Delay(100);
	    HAL_UART_Transmit(&huart6,"AT+CIPCLOSE\r\n",15,100);
		  _5sec_flag=0;
	  	osSemaphoreRelease(myBinarySem01Handle);
	  }
	  	osDelay(1);
  }
  /* USER CODE END Task2_ReadLM35_WriteLCD_5Sec */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM10 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM10) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

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
