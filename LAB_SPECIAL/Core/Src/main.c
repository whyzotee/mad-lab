/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "string.h"
#include "stdbool.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define UART_TIMEOUT 1000
#define PACKET_SIZE 64
#define TX_MODE 0 // 1 == tx, 0 == rx
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
char tx_header[] = "\n\rMan from U.A.R.T.1!\n\r";
char rx_header[] = "\n\rMan from U.A.R.T.2!\n\r";

char press_q[] = "Quit PRESS q\n\r";
char name_title[] = "	Name: ";

uint8_t state = 0;
bool is_end = false;

char tx_name_buffer[PACKET_SIZE];
char rx_name_buffer[PACKET_SIZE];

char tx_input_buffer;
char tx_buffer[PACKET_SIZE];
char rx_buffer[PACKET_SIZE];

uint8_t test_count = 0;
char test_word[PACKET_SIZE];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
/* USER CODE BEGIN PFP */
void NextState(void);
void PreviousState(void);
void GetUserInput(void);
void Receive_From_UART(void);
void EndState(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

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
  MX_USART1_UART_Init();
  MX_USART3_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
#if TX_MODE
  HAL_UART_Transmit(&huart3, (uint8_t*) tx_header, strlen(tx_header), UART_TIMEOUT);
  HAL_UART_Transmit(&huart3, (uint8_t*) press_q, strlen(press_q), UART_TIMEOUT);
  HAL_UART_Transmit(&huart3, (uint8_t*) name_title, strlen(name_title), UART_TIMEOUT);

  HAL_UART_Receive_IT(&huart1, (uint8_t*) rx_buffer, PACKET_SIZE);
#else
  HAL_UART_Transmit(&huart3, (uint8_t*) rx_header, strlen(rx_header), UART_TIMEOUT);
  HAL_UART_Transmit(&huart3, (uint8_t*) press_q, strlen(press_q), UART_TIMEOUT);
  state = 1;

  HAL_UART_Receive_IT(&huart2, (uint8_t*) rx_buffer, PACKET_SIZE);
#endif

  memset(tx_buffer, 0, PACKET_SIZE);
  memset(rx_buffer, 0, PACKET_SIZE);
  memset(test_word, 0, PACKET_SIZE);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (!is_end)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	switch (state) {
	  case 0:
		  GetUserInput();
		  break;
	  case 1:
		  // Receive from interrupt by whyzotee
		  break;
	  default:
		  char error[] = "Bye";
		  HAL_UART_Transmit(&huart3, (uint8_t*) error, strlen(error), 1000);
		  is_end = true;
		  break;
	}

	HAL_Delay(50);
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 216;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == GPIO_PIN_13)
	{
		if (state == 0) return;

		memset(test_word, 0, PACKET_SIZE);
		UART_HandleTypeDef *target_uart = (!TX_MODE) ? &huart1 : &huart2;

		if (test_count == 0)
		{
			strcpy(test_word, "whyzotee");
			test_count++;
		}
		else if (test_count == 1)
		{
			strcpy(test_word, "Message 1");
		    test_count++;
		}
		else
		{
			strcpy(test_word, "Message 2");
			test_count = 1;
		}

		HAL_UART_Transmit(target_uart, (uint8_t*)test_word, PACKET_SIZE, UART_TIMEOUT);
	}
}

void NextState(void) { state +=1 ;}
void PreviousState(void) { state -=1 ;}
void EndState(void) { state = -1 ;}

void GetUserInput(void)
{
	while(__HAL_UART_GET_FLAG(&huart3, UART_FLAG_RXNE) == 0) {}
	HAL_UART_Receive(&huart3, (uint8_t*) &tx_input_buffer, 1, UART_TIMEOUT);

	while (__HAL_UART_GET_FLAG(&huart3, UART_FLAG_TC) == 0) {}
	if (tx_input_buffer == 0x08 || tx_input_buffer == 0x7F){
		if (strlen(tx_buffer) > 0) {
			tx_buffer[strlen(tx_buffer)-1] = 0;
			HAL_UART_Transmit(&huart3, (uint8_t*) "\0\b \b", 4, UART_TIMEOUT);
		}
	} else if (tx_input_buffer == '\r') {
		HAL_UART_Transmit(&huart3, (uint8_t*) "\r\n", 2, UART_TIMEOUT);

		if (!strlen(tx_name_buffer)) {
			strcpy(tx_name_buffer, tx_buffer);

			HAL_UART_Transmit((TX_MODE) ? &huart1: &huart2, (uint8_t*) tx_name_buffer, PACKET_SIZE, UART_TIMEOUT);
		} else {
			HAL_UART_Transmit((TX_MODE) ? &huart1: &huart2, (uint8_t*) tx_buffer, PACKET_SIZE, UART_TIMEOUT);
		}

		if (strcmp(tx_buffer, "q") == 0) {
			EndState();
			return;
		}

		memset(tx_buffer, 0, PACKET_SIZE);

		NextState();
	} else {
		HAL_UART_Transmit(&huart3, (uint8_t*) &tx_input_buffer, 1, UART_TIMEOUT);
		tx_buffer[strlen(tx_buffer)] = tx_input_buffer;
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

#if TX_MODE
	HAL_UART_Receive_IT(&huart1, (uint8_t*) rx_buffer, PACKET_SIZE);
#else
	HAL_UART_Receive_IT(&huart2, (uint8_t*) rx_buffer, PACKET_SIZE);
#endif

	if (strcmp(rx_buffer, "q") == 0) {
		EndState();
		return;
	}

	while (__HAL_UART_GET_FLAG(&huart3, UART_FLAG_TC) == 0) {}
	HAL_UART_Transmit(&huart3, (uint8_t*) "	", 1, UART_TIMEOUT);

	if (!strlen(rx_name_buffer)) {
		strcpy(rx_name_buffer, (char*)rx_buffer);

		HAL_UART_Transmit(&huart3, (uint8_t*) rx_name_buffer, strlen(rx_name_buffer), UART_TIMEOUT);
		HAL_UART_Transmit(&huart3, (uint8_t*) " is ready", 9, UART_TIMEOUT);
	} else {
		HAL_UART_Transmit(&huart3, (uint8_t*) rx_name_buffer, strlen(rx_name_buffer), UART_TIMEOUT);
		HAL_UART_Transmit(&huart3, (uint8_t*) " : ", 3, UART_TIMEOUT);
		HAL_UART_Transmit(&huart3, (uint8_t*) rx_buffer, PACKET_SIZE, UART_TIMEOUT);
	}

	HAL_UART_Transmit(&huart3, (uint8_t*) "\r\n", 2, UART_TIMEOUT);

	if (!strlen(tx_name_buffer)) {
		HAL_UART_Transmit(&huart3, (uint8_t*) name_title, strlen(name_title), UART_TIMEOUT);
	} else {
		HAL_UART_Transmit(&huart3, (uint8_t*) "	", 1, UART_TIMEOUT);
		HAL_UART_Transmit(&huart3, (uint8_t*) tx_name_buffer, PACKET_SIZE, UART_TIMEOUT);
		HAL_UART_Transmit(&huart3, (uint8_t*) " => ", 4, UART_TIMEOUT);
	}

	memset(rx_buffer, 0, PACKET_SIZE);

	PreviousState();
}
/* USER CODE END 4 */

 /* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x0;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
  MPU_InitStruct.SubRegionDisable = 0x87;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

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
#ifdef USE_FULL_ASSERT
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
