/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
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
#include "usart.h"
#include "gpio.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */
uint8_t can2_tx_aData[8]={0,0};
uint8_t can2_rx_aData[8]={0,0};

CAN_RxHeaderTypeDef can2_pHeader;



/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
	/******************************************************************************************************
	 Noted:
	 1、 APM32 CAN2 needs to be set separately at offset address 0x200 to 0x31c,ST only set CAN1.
	 2、 APM32 CAN2 interrupt vector needs to be reset.
	 ******************************************************************************************************/

  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_CAN1_Init();

  MX_CAN2_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  USART_TxData(USART2,2);

  CAN_Filter_Config(&hcan2);

  #if CAN2_RX_HANDLE_ENABLE//enable  CAN2 rx enable
  CAN_EnableInterrupt(CAN2,CAN_INT_F0MP);
  #endif
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
      #if !CAN2_RX_HANDLE_ENABLE      //if not enable can2 rx interrupt,excute scan mode.
	  if(HAL_CAN_GetRxMessage(&hcan2, CAN_RX_FIFO0, &can2_pHeader,can2_rx_aData)==HAL_OK)
	  {
		  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_RESET);
		  CAN_Send(&hcan2,can2_rx_aData,size(can2_rx_aData));
		  HAL_Delay(100);
		  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_SET);
	  }
      #endif

	  if(HAL_GPIO_ReadPin(GPIOA, KEY1_Pin)==GPIO_PIN_RESET)
	  {
		  HAL_Delay(10);
		  if(HAL_GPIO_ReadPin(GPIOA, KEY1_Pin)==GPIO_PIN_RESET)
		  {
			  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, GPIO_PIN_RESET);
			  #if CAN2_TX_HANDLE_ENABLE//enable  CAN2 tx enable
			  CAN_EnableInterrupt(CAN2,CAN_INT_TXME);
			  #endif

			  can2_tx_aData[0]=0;
			  can2_tx_aData[1]++;
			  CAN_Send(&hcan2,can2_tx_aData,size(can2_tx_aData));
			  while(HAL_GPIO_ReadPin(GPIOA, KEY1_Pin)!=GPIO_PIN_SET);
			  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, GPIO_PIN_SET);
		  }
	  }

      #if CAN2_TX_HANDLE_ENABLE    //if enable can2 tx interrupt,key exit interrupt.
	  if(HAL_GPIO_ReadPin(GPIOA, KEY2_Pin)==GPIO_PIN_RESET)
	  {
	  		  HAL_Delay(10);
	  		  if(HAL_GPIO_ReadPin(GPIOA, KEY2_Pin)==GPIO_PIN_RESET)
	  		  {
	  			  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_RESET);
	  			  CAN_DisableInterrupt(CAN2,CAN_INT_TXME);
	  			  while(HAL_GPIO_ReadPin(GPIOA, KEY1_Pin)!=GPIO_PIN_SET);
	  			  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_SET);
	  		  }
	   }
      #endif
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.Prediv1Source = RCC_PREDIV1_SOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  RCC_OscInitStruct.PLL2.PLL2State = RCC_PLL_NONE;
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
  /** Configure the Systick interrupt time
  */
  __HAL_RCC_PLLI2S_ENABLE();
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
