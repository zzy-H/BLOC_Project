/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    adc.c
  * @brief   This file provides code for the configuration
  *          of the ADC instances.
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
#include "adc.h"

/* USER CODE BEGIN 0 */

#include <math.h>
#include "multi_button.h"
#include "main.h"
#include "VFmode.h"
#include "FOC_mode0.h"
#include "hall_driev.h"

uint8_t ADC_OffSet = 0;
static uint8_t cnt = 0;
static float offset[3] = {0};
static uint32_t ADC_Data[3] = {0};
float CurrlValue[3] = {0};   // 三相电流值，可能以后要用，不加static

extern HALL_Handle_t HALL_Handle;

//static float last_val[3] = {0};


/* USER CODE END 0 */

ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;

/* ADC1 init function */
void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_MultiModeTypeDef multimode = {0};
  ADC_InjectionConfTypeDef sConfigInjected = {0};
  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.GainCompensation = 0;
  hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc1.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the ADC multi-mode
  */
  multimode.Mode = ADC_MODE_INDEPENDENT;
  if (HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Injected Channel
  */
  sConfigInjected.InjectedChannel = ADC_CHANNEL_3;
  sConfigInjected.InjectedRank = ADC_INJECTED_RANK_1;
  sConfigInjected.InjectedSamplingTime = ADC_SAMPLETIME_6CYCLES_5;
  sConfigInjected.InjectedSingleDiff = ADC_SINGLE_ENDED;
  sConfigInjected.InjectedOffsetNumber = ADC_OFFSET_NONE;
  sConfigInjected.InjectedOffset = 0;
  sConfigInjected.InjectedNbrOfConversion = 2;
  sConfigInjected.InjectedDiscontinuousConvMode = DISABLE;
  sConfigInjected.AutoInjectedConv = DISABLE;
  sConfigInjected.QueueInjectedContext = DISABLE;
  sConfigInjected.ExternalTrigInjecConv = ADC_EXTERNALTRIGINJEC_T1_CC4;
  sConfigInjected.ExternalTrigInjecConvEdge = ADC_EXTERNALTRIGINJECCONV_EDGE_RISING;
  sConfigInjected.InjecOversamplingMode = DISABLE;
  if (HAL_ADCEx_InjectedConfigChannel(&hadc1, &sConfigInjected) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Injected Channel
  */
  sConfigInjected.InjectedChannel = ADC_CHANNEL_12;
  sConfigInjected.InjectedRank = ADC_INJECTED_RANK_2;
  if (HAL_ADCEx_InjectedConfigChannel(&hadc1, &sConfigInjected) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}
/* ADC2 init function */
void MX_ADC2_Init(void)
{

  /* USER CODE BEGIN ADC2_Init 0 */

  /* USER CODE END ADC2_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};
  ADC_InjectionConfTypeDef sConfigInjected = {0};

  /* USER CODE BEGIN ADC2_Init 1 */

  /* USER CODE END ADC2_Init 1 */

  /** Common config
  */
  hadc2.Instance = ADC2;
  hadc2.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc2.Init.Resolution = ADC_RESOLUTION_12B;
  hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc2.Init.GainCompensation = 0;
  hadc2.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc2.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc2.Init.LowPowerAutoWait = DISABLE;
  hadc2.Init.ContinuousConvMode = DISABLE;
  hadc2.Init.NbrOfConversion = 1;
  hadc2.Init.DiscontinuousConvMode = DISABLE;
  hadc2.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc2.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc2.Init.DMAContinuousRequests = DISABLE;
  hadc2.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc2.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&hadc2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_3;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Injected Channel
  */
  sConfigInjected.InjectedChannel = ADC_CHANNEL_3;
  sConfigInjected.InjectedRank = ADC_INJECTED_RANK_1;
  sConfigInjected.InjectedSamplingTime = ADC_SAMPLETIME_6CYCLES_5;
  sConfigInjected.InjectedSingleDiff = ADC_SINGLE_ENDED;
  sConfigInjected.InjectedOffsetNumber = ADC_OFFSET_NONE;
  sConfigInjected.InjectedOffset = 0;
  sConfigInjected.InjectedNbrOfConversion = 1;
  sConfigInjected.InjectedDiscontinuousConvMode = DISABLE;
  sConfigInjected.AutoInjectedConv = DISABLE;
  sConfigInjected.QueueInjectedContext = DISABLE;
  sConfigInjected.ExternalTrigInjecConv = ADC_EXTERNALTRIGINJEC_T1_CC4;
  sConfigInjected.ExternalTrigInjecConvEdge = ADC_EXTERNALTRIGINJECCONV_EDGE_RISING;
  sConfigInjected.InjecOversamplingMode = DISABLE;
  if (HAL_ADCEx_InjectedConfigChannel(&hadc2, &sConfigInjected) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC2_Init 2 */

  /* USER CODE END ADC2_Init 2 */

}

static uint32_t HAL_RCC_ADC12_CLK_ENABLED=0;

void HAL_ADC_MspInit(ADC_HandleTypeDef* adcHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  if(adcHandle->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspInit 0 */

  /* USER CODE END ADC1_MspInit 0 */

  /** Initializes the peripherals clocks
  */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC12;
    PeriphClkInit.Adc12ClockSelection = RCC_ADC12CLKSOURCE_SYSCLK;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler();
    }

    /* ADC1 clock enable */
    HAL_RCC_ADC12_CLK_ENABLED++;
    if(HAL_RCC_ADC12_CLK_ENABLED==1){
      __HAL_RCC_ADC12_CLK_ENABLE();
    }

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**ADC1 GPIO Configuration
    PA0     ------> ADC1_IN1
    PA2     ------> ADC1_IN3
    PB1     ------> ADC1_IN12
    */
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* ADC1 interrupt Init */
    HAL_NVIC_SetPriority(ADC1_2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(ADC1_2_IRQn);
  /* USER CODE BEGIN ADC1_MspInit 1 */

  /* USER CODE END ADC1_MspInit 1 */
  }
  else if(adcHandle->Instance==ADC2)
  {
  /* USER CODE BEGIN ADC2_MspInit 0 */

  /* USER CODE END ADC2_MspInit 0 */

  /** Initializes the peripherals clocks
  */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC12;
    PeriphClkInit.Adc12ClockSelection = RCC_ADC12CLKSOURCE_SYSCLK;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler();
    }

    /* ADC2 clock enable */
    HAL_RCC_ADC12_CLK_ENABLED++;
    if(HAL_RCC_ADC12_CLK_ENABLED==1){
      __HAL_RCC_ADC12_CLK_ENABLE();
    }

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**ADC2 GPIO Configuration
    PA6     ------> ADC2_IN3
    */
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* ADC2 interrupt Init */
    HAL_NVIC_SetPriority(ADC1_2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(ADC1_2_IRQn);
  /* USER CODE BEGIN ADC2_MspInit 1 */

  /* USER CODE END ADC2_MspInit 1 */
  }
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef* adcHandle)
{

  if(adcHandle->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspDeInit 0 */

  /* USER CODE END ADC1_MspDeInit 0 */
    /* Peripheral clock disable */
    HAL_RCC_ADC12_CLK_ENABLED--;
    if(HAL_RCC_ADC12_CLK_ENABLED==0){
      __HAL_RCC_ADC12_CLK_DISABLE();
    }

    /**ADC1 GPIO Configuration
    PA0     ------> ADC1_IN1
    PA2     ------> ADC1_IN3
    PB1     ------> ADC1_IN12
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0|GPIO_PIN_2);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_1);

    /* ADC1 interrupt Deinit */
  /* USER CODE BEGIN ADC1:ADC1_2_IRQn disable */
    /**
    * Uncomment the line below to disable the "ADC1_2_IRQn" interrupt
    * Be aware, disabling shared interrupt may affect other IPs
    */
    /* HAL_NVIC_DisableIRQ(ADC1_2_IRQn); */
  /* USER CODE END ADC1:ADC1_2_IRQn disable */

  /* USER CODE BEGIN ADC1_MspDeInit 1 */

  /* USER CODE END ADC1_MspDeInit 1 */
  }
  else if(adcHandle->Instance==ADC2)
  {
  /* USER CODE BEGIN ADC2_MspDeInit 0 */

  /* USER CODE END ADC2_MspDeInit 0 */
    /* Peripheral clock disable */
    HAL_RCC_ADC12_CLK_ENABLED--;
    if(HAL_RCC_ADC12_CLK_ENABLED==0){
      __HAL_RCC_ADC12_CLK_DISABLE();
    }

    /**ADC2 GPIO Configuration
    PA6     ------> ADC2_IN3
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_6);

    /* ADC2 interrupt Deinit */
  /* USER CODE BEGIN ADC2:ADC1_2_IRQn disable */
    /**
    * Uncomment the line below to disable the "ADC1_2_IRQn" interrupt
    * Be aware, disabling shared interrupt may affect other IPs
    */
    /* HAL_NVIC_DisableIRQ(ADC1_2_IRQn); */
  /* USER CODE END ADC2:ADC1_2_IRQn disable */

  /* USER CODE BEGIN ADC2_MspDeInit 1 */

  /* USER CODE END ADC2_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

// 添加 ADC 和内部运放的初始化代码
void BSP_ADC_Init(void)
{
    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);  // 先灭 LED
    
    HAL_OPAMP_Start(&hopamp1);
    HAL_OPAMP_Start(&hopamp2);
    HAL_OPAMP_Start(&hopamp3);
    
    HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
    
    HAL_ADCEx_Calibration_Start(&hadc2, ADC_SINGLE_ENDED);
    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);   // ADC2 校准完成
    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
    
    __HAL_ADC_CLEAR_FLAG(&hadc1, ADC_FLAG_EOC);
    __HAL_ADC_CLEAR_FLAG(&hadc1, ADC_FLAG_JEOC);
    __HAL_ADC_CLEAR_FLAG(&hadc2, ADC_FLAG_JEOC);
    
    HAL_ADCEx_InjectedStart_IT(&hadc1);
	HAL_ADCEx_InjectedStart(&hadc2);
}

/*
 * ADC1 采用中断方式，ADC2 直接开启不触发中断。
 * 三个通道都由 TIM1_CH4 触发注入组转换，
 * 因此在 ADC1 中断触发时读取 ADC2 的 JDR1，即可获取 ADC2 的转换结果。
 */
static int VF_flag = 0;
static uint16_t vf_count = 0;
void HAL_ADCEx_InjectedConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    UNUSED(hadc);
    if(hadc->Instance == ADC1)
    {
        if(ADC_OffSet == 0)
        {
            cnt++;
            offset[0] += ADC1->JDR1;
            offset[1] += ADC2->JDR1;
            offset[2] += ADC1->JDR2;
            if(cnt >= 10)
            {
                offset[0] /= 10.0f;
                offset[1] /= 10.0f;
                offset[2] /= 10.0f;
                ADC_OffSet = 1;
				
            }
        }
        else
        {
			if (VF_flag == 0)
			{
				/* VF 开环拖动 */
				VF_mode();
				vf_count++;

				/* 后台同步更新霍尔插值角度 */
				HALL_Handle.MeasuredElAngle += HALL_Handle.AvrElSpeedDpp;

				/* 切闭环*/
				if (vf_count >= 10000)
				{
					// 1. 先手动更新一次速度反馈（开环时算法端口不刷新，必须补一次）
					//    用底层霍尔实测的真实转速赋值给算法反馈口
					HALL_rtU.SpeedFd = HALL_Handle.HallSpeed;  

					// 2. 转速给定 = 当前实际转速，让速度环初始误差 = 0
					HALL_rtU.SpeedRef = HALL_rtU.SpeedFd;

					// 3. 清零所有PI积分器，彻底消除残留值（最关键的一步）
					HALL_rtDW.Integrator_DSTATE = 0.0f;       // 速度环积分器清零
					HALL_rtDW.Integrator_DSTATE_o = 0.0f;     // Id电流环积分器清零
					HALL_rtDW.Integrator_DSTATE_l = 0.0f;     // Iq电流环积分器清零

					// 4. 电角度平滑同步：用开环角度初始化霍尔角度，避免角度跳变
					HALL_Handle.HallElAngle = VF_rtDW.UnitDelay_DSTATE;
					HALL_Handle.MeasuredElAngle = VF_rtDW.UnitDelay_DSTATE; // 同步插值角度

					// 5. 切换模式标志，下一个周期进入闭环
					VF_flag = 1;
				}
			}
			else
			{
				hall_FOC_mode();
			}
			
        }
    }
}

// 读取母线电压（PA0，分压比 1/26，多次采样取平均，带超时保护）
float get_bus_voltage(void)
{
    float sum = 0.0f;
    int valid = 0;

    for (int i = 0; i < 8; i++)             /* 采 8 次取平均 */
    {
        /* 等 ADC 空闲后再启动规则转换 */
        uint32_t timeout = 10000;
        while ((ADC1->CR & ADC_CR_ADSTART) && --timeout);

        ADC1->CR |= ADC_CR_ADSTART;          /* 启动规则组转换 */

        timeout = 10000;
        while (!(ADC1->ISR & ADC_ISR_EOC) && --timeout);  /* 等 EOC，超时退出 */

        if (timeout == 0) break;             /* 超时了，放弃本次 */

        uint32_t val = ADC1->DR;             /* 读结果，自动清 EOC */
        sum += val * 0.020947f;              /* × (3.3/4096) × 26 */
        valid++;
    }
    return (valid > 0) ? (sum / valid) : 0.0f;
}

//VF开环控制
void VF_mode(void)
{
	ADC_Data[0] = ADC1->JDR1;
	ADC_Data[1] = ADC2->JDR1;
	ADC_Data[2] = ADC1->JDR2;

	CurrlValue[0] = (ADC_Data[0] - offset[0]) * 0.021972f;
	CurrlValue[1] = (ADC_Data[1] - offset[1]) * 0.021972f;
	CurrlValue[2] = (ADC_Data[2] - offset[2]) * 0.021972f;
	
	/* 设置 V/F 输入 */
	VF_rtU.ud = 0.0f;
	VF_rtU.uq = 12.0f;
	VF_rtU.Freq = 4.0f;
	
	VFmode_step();
	
	 /* 写 PWM*/
	TIM1->CCR1 = (uint32_t)VF_rtY.tABC[0];   
	TIM1->CCR2 = (uint32_t)VF_rtY.tABC[1];   
	TIM1->CCR3 = (uint32_t)VF_rtY.tABC[2];   
}

//FOC有感闭环控制




void hall_FOC_mode(void)
{
	/* 角度累加 + 校准（例程里的 ADC 回调逻辑）*/
	
	uint8_t hall_updated = HALL_Handle.bHallUpdated;  // 先保存
	
	//判断当正转一周角度变成偏差值时，进行校准
	if(hall_updated)
	{
		//校准电角度
		HALL_Handle.HallElAngle = HALL_Handle.MeasuredElAngle;
		
		HALL_Handle.MeasuredElAngle += HALL_Handle.AvrElSpeedDpp;
		
		HALL_Handle.HallElAngle += HALL_Handle.AvrElSpeedDpp;
		
		HALL_Handle.bHallUpdated = 0;
		hall_updated = 0;               // 同步本地变量
	}
	
	else
	{
		HALL_Handle.MeasuredElAngle += HALL_Handle.AvrElSpeedDpp;
		
		HALL_Handle.HallElAngle += HALL_Handle.AvrElSpeedDpp + HALL_Handle.DeltaAngle;
	}
	//限制电角度在 0~2PI 之间
	if ( HALL_Handle.HallElAngle < 0.0f)
	{
		HALL_Handle.HallElAngle += 2.0f * PI;
	}
	else if ( HALL_Handle.HallElAngle > (2.0f * PI))
	{
		HALL_Handle.HallElAngle -= 2.0f * PI;
	}
	
	ADC_Data[0] = ADC1->JDR1;
	ADC_Data[1] = ADC2->JDR1;
	ADC_Data[2] = ADC1->JDR2;
	
	CurrlValue[0] = (ADC_Data[0]-offset[0])*0.02197f;
	CurrlValue[1] = (ADC_Data[1]-offset[1])*0.02197f;
	CurrlValue[2] = (ADC_Data[2]-offset[2])*0.02197f;
	
	HALL_rtU.ia = CurrlValue[0];
	HALL_rtU.ib = CurrlValue[1];
	HALL_rtU.ic = CurrlValue[2];
	
	//voltage = get_bus_voltage();
	
	
	HALL_rtU.theat = HALL_Handle.HallElAngle;
	
	// 用 AvrElSpeedDpp (rad/s) 实时换算 RPM
	// AvrElSpeedDpp 是每 0.1ms 的电角度增量(rad)，乘以 10000 得 rad/s
	// RPM = rad/s * 30 / (PI * PolePair)，PolePair = 2
	// 所以 RPM = rad/s * 30 / (PI * 2) = rad/s * 15 / PI
	float instant_speed = (HALL_Handle.AvrElSpeedDpp * 10000.0f) * 15.0f / PI;
	

	// 如果霍尔刚更新过，用霍尔测量的速度；否则用插补速度
	if (hall_updated) {
		HALL_rtU.SpeedFd = HALL_Handle.HallSpeed;	
	} else {
		// 用一阶滤波平滑过渡
		static float last_speed = 0.0f;
		HALL_rtU.SpeedFd = last_speed + 0.1f * (instant_speed - last_speed);
		last_speed = HALL_rtU.SpeedFd;
	}
	
	FOC_Mode0_step();
	
	TIM1->CCR1 = HALL_rtY.tABC[0];
	TIM1->CCR2 = HALL_rtY.tABC[1];
	TIM1->CCR3 = HALL_rtY.tABC[2];
}

/* USER CODE END 1 */
