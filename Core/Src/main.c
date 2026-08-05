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
#include "adc.h"
#include "dma.h"
#include "fdcan.h"
#include "opamp.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include "multi_button.h"
#include "mybutton.h"
#include "fdcan.h"
#include "VFmode.h"
#include "FOC_Model.h"
#include "hall_driev.h"
#include "mycan.h"
#include "myuart.h"

extern HALL_Handle_t HALL_Handle;
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
extern float CurrlValue[3];
extern uint8_t ADC_OffSet;
extern uint8_t Hal_State;
extern uint8_t open_can;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

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
  MX_TIM1_Init();
  MX_USART3_UART_Init();
  MX_ADC1_Init();
  MX_ADC2_Init();
  MX_OPAMP1_Init();
  MX_OPAMP2_Init();
  MX_OPAMP3_Init();
  MX_FDCAN1_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
  
	// 启动定时器的 CNT 计数
	HAL_TIM_Base_Start(&htim1);                              // TIM1 计数
	
	// 启动定时器对应通道的 PWM 输出
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);                // U+ 上桥
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);                // V+ 上桥
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);                // W+ 上桥
	
	// 启动定时器对应互补通道的 PWM 输出
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);             // U- 下桥（互补）
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);             // V- 下桥（互补）
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3);             // W- 下桥（互补）
	
	// 启动通道 4 的 PWM 输出用于触发 ADC 采样
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);                // 触发 ADC 采样
	
	/* 初始化 DMA 接收 */
	BSP_Vofa_Init();
	
	// ADC 初始化
	BSP_ADC_Init();
		
		
	// 按键初始化
	my_key_init();
	
	// FDCAN 初始化
	MX_FDCAN1_Init();
	// 配置滤波器：接收 ID 0x000~0x1FF 的所有报文
	Can_Message_Init();
	
	// VF 初始化
	VFmode_initialize();
	
	// 启动 TIM4 霍尔传感器捕获（中断模式）
	HAL_TIMEx_HallSensor_Start_IT(&htim4);

	// 读取霍尔初始电角度
	HALL_Init_Electrical_Angle();

	// 初始化 FOC 模型（有感+无感一体）
	FOC_Model_initialize();
	Hal_State = 1;              // 1=有感(Hall)，0=无感(Flux)
	rtU.Motor_OnOff = 1.0f;     // 电机使能
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	
	if (open_can) {
    // 上位机模式：LED 闪烁（每 500ms 翻转）
	USART3_Anylze(); 
    static uint32_t last_led_tick = 0;
    if (HAL_GetTick() - last_led_tick >= 500) {
        HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
        last_led_tick = HAL_GetTick();
    }
	} else {
		// 电位器模式：LED 常亮
		HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
		Potentiometer_SpeedSet();
	}  
	
	FDCAN_Control();
	  
	// 发送数据
	send_UVWV();
	
	// 每 100ms 发一次 CAN 消息
	static uint32_t last_can_tick = 0;
	if (HAL_GetTick() - last_can_tick >= 100) {
		last_can_tick = HAL_GetTick();
		HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, FDCan_RxData);
	}
	
		
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
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV2;
  RCC_OscInitStruct.PLL.PLLN = 80;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART3) {
        /* DMA 发送完成，不需要额外操作，HAL 内部会自动清 BUSY */
    }
}

void send_UVWV(void)
{
	
	  
	if (ADC_OffSet == 1)    // 偏移校准完成后才开始发
	{
		float load_data[6];
		//load_data[0] = CurrlValue[0];        // U 相电流
		load_data[0] = state;
		load_data[1] = CurrlValue[1];        // V 相电流
		load_data[2] = CurrlValue[2];        // W 相电流
		load_data[3] = (float)TIM1->CCR1;    // CCR1（W 相占空比）
		load_data[4] = (float)TIM1->CCR2;    // CCR2（V 相占空比）
		load_data[5] = (float)TIM1->CCR3;    // CCR3（U 相占空比）
		

		static uint8_t tempData[28];
		memcpy(tempData, (uint8_t *)&load_data, sizeof(load_data));
		tempData[24] = 0x00; tempData[25] = 0x00;
		tempData[26] = 0x80; tempData[27] = 0x7F;

		HAL_UART_Transmit(&huart3, tempData, 28, 1000);
		HAL_Delay(1);
	}
}

/* 电位器调速（PB12 / ADC1_IN11），10 次采样平均，范围 600~2300 RPM */
void Potentiometer_SpeedSet(void)
{
    static uint16_t vres[10] = {0};
    uint16_t Vres = 0;
    static uint8_t pot_cnt = 0;

    if(pot_cnt < 10)
    {
		uint32_t timeout = 10000;
		while ((ADC1->CR & ADC_CR_ADSTART) && --timeout);  // 等 ADC 空闲

		ADC1->CR |= ADC_CR_ADSTART;          // 启动规则组转换

		timeout = 10000;
		while (!(ADC1->ISR & ADC_ISR_EOC) && --timeout);  // 等 EOC

		if (timeout == 0) return;            // 超时了放弃本次
		vres[pot_cnt] = ADC1->DR;            // 读结果
        pot_cnt++;
    }
    else
    {
        pot_cnt = 0;
        for(uint8_t i = 0; i < 10; i++) Vres += vres[i];
        Vres /= 10;

        if(Vres > 2048)
            rtU.SpeedRef = (float)((int)((Vres - 2048) * 0.83) + 600);
        else
            rtU.SpeedRef = -(float)((int)((2048 - Vres) * 0.83) + 600);
    }
}


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
