/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    fdcan.c
  * @brief   This file provides code for the configuration
  *          of the FDCAN instances.
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
#include "fdcan.h"

/* USER CODE BEGIN 0 */
#include "mycan.h"


//设置发送的 CAN 报文的参数
FDCAN_TxHeaderTypeDef TxHeader={0};

//保存接收到的 CAN 通信报文的参数信息
FDCAN_RxHeaderTypeDef RxHeader={0};

/* USER CODE END 0 */

FDCAN_HandleTypeDef hfdcan1;

/* FDCAN1 init function */
void MX_FDCAN1_Init(void)
{

  /* USER CODE BEGIN FDCAN1_Init 0 */

  /* USER CODE END FDCAN1_Init 0 */

  /* USER CODE BEGIN FDCAN1_Init 1 */

  /* USER CODE END FDCAN1_Init 1 */
  hfdcan1.Instance = FDCAN1;
  hfdcan1.Init.ClockDivider = FDCAN_CLOCK_DIV1;
  hfdcan1.Init.FrameFormat = FDCAN_FRAME_CLASSIC;
  hfdcan1.Init.Mode = FDCAN_MODE_NORMAL;
  hfdcan1.Init.AutoRetransmission = DISABLE;
  hfdcan1.Init.TransmitPause = DISABLE;
  hfdcan1.Init.ProtocolException = DISABLE;
  hfdcan1.Init.NominalPrescaler = 20;
  hfdcan1.Init.NominalSyncJumpWidth = 1;
  hfdcan1.Init.NominalTimeSeg1 = 10;
  hfdcan1.Init.NominalTimeSeg2 = 5;
  hfdcan1.Init.DataPrescaler = 20;
  hfdcan1.Init.DataSyncJumpWidth = 1;
  hfdcan1.Init.DataTimeSeg1 = 10;
  hfdcan1.Init.DataTimeSeg2 = 5;
  hfdcan1.Init.StdFiltersNbr = 1;
  hfdcan1.Init.ExtFiltersNbr = 1;
  hfdcan1.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
  if (HAL_FDCAN_Init(&hfdcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN FDCAN1_Init 2 */

  /* USER CODE END FDCAN1_Init 2 */

}

void HAL_FDCAN_MspInit(FDCAN_HandleTypeDef* fdcanHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  if(fdcanHandle->Instance==FDCAN1)
  {
  /* USER CODE BEGIN FDCAN1_MspInit 0 */

  /* USER CODE END FDCAN1_MspInit 0 */

  /** Initializes the peripherals clocks
  */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_FDCAN;
    PeriphClkInit.FdcanClockSelection = RCC_FDCANCLKSOURCE_PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler();
    }

    /* FDCAN1 clock enable */
    __HAL_RCC_FDCAN_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**FDCAN1 GPIO Configuration
    PA11     ------> FDCAN1_RX
    PA12     ------> FDCAN1_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF9_FDCAN1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* FDCAN1 interrupt Init */
    HAL_NVIC_SetPriority(FDCAN1_IT0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(FDCAN1_IT0_IRQn);
  /* USER CODE BEGIN FDCAN1_MspInit 1 */

  /* USER CODE END FDCAN1_MspInit 1 */
  }
}

void HAL_FDCAN_MspDeInit(FDCAN_HandleTypeDef* fdcanHandle)
{

  if(fdcanHandle->Instance==FDCAN1)
  {
  /* USER CODE BEGIN FDCAN1_MspDeInit 0 */

  /* USER CODE END FDCAN1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_FDCAN_CLK_DISABLE();

    /**FDCAN1 GPIO Configuration
    PA11     ------> FDCAN1_RX
    PA12     ------> FDCAN1_TX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_11|GPIO_PIN_12);

    /* FDCAN1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(FDCAN1_IT0_IRQn);
  /* USER CODE BEGIN FDCAN1_MspDeInit 1 */

  /* USER CODE END FDCAN1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

//消息初始化
void Can_Message_Init(void)
{
// 配置滤波器：接收 ID 0x000~0x1FF 的所有报文
	FDCAN_FilterTypeDef Filter={0};
	
	Filter.FilterConfig=FDCAN_FILTER_TO_RXFIFO0;// 滤波器配置：使能/禁用过滤或分配给某些 FIFO
	
	Filter.FilterID1= 0x000;// 第一个过滤器标识符
	
	Filter.FilterID2= 0x7FF;// 第二个过滤器标识符（范围过滤或掩码过滤使用）
	
	//Filter.FilterID1= 0x00000000;// 第一个过滤器标识符
	//Filter.FilterID2=0x01ffffff;// 第二个过滤器标识符（范围过滤或掩码过滤使用）
	
	Filter.FilterIndex= 0;// 滤波器索引，表示当前配置的是哪个滤波器
	
	Filter.FilterType= FDCAN_FILTER_RANGE;//滤波器类型：范围过滤或掩码过滤
	
	Filter.IdType=FDCAN_STANDARD_ID;//标识符类型：标准标识符或扩展标识符
	
	if (HAL_FDCAN_ConfigFilter(&hfdcan1, &Filter) != HAL_OK) {
		Error_Handler();//错误处理
	}
	
//配置发送的数据帧类型
	TxHeader.BitRateSwitch=FDCAN_BRS_OFF;//是否启用速率切换（仅在 CAN FD 模式下有效）
	
	TxHeader.DataLength=FDCAN_DLC_BYTES_8;//数据长度代码 (DLC)，指定数据字段的长度
	
	//TxHeader.DataLength=FDCAN_DLC_BYTES_16;
	
	TxHeader.ErrorStateIndicator=FDCAN_ESI_ACTIVE;//错误状态指示器，指定是否使用活动错误状态指示。
	
	TxHeader.FDFormat=FDCAN_CLASSIC_CAN;// 是否使用 CAN FD 格式← 这个是经典 CAN，只能用 8 字节
	
	TxHeader.Identifier=0x125;//消息标识符，用于标识 CAN 帧。
	
	TxHeader.IdType=FDCAN_STANDARD_ID;//标识符类型，指定是标准还是扩展标识符。
	
	TxHeader.MessageMarker=0x02;//消息标记，用于区分传输消息。
	
	TxHeader.TxEventFifoControl=FDCAN_NO_TX_EVENTS;//是否将传输事件存储到 Tx 事件 FIFO。
	
	TxHeader.TxFrameType=FDCAN_DATA_FRAME;//帧类型，指定是数据帧还是远程帧。
	
	if(HAL_FDCAN_ActivateNotification(&hfdcan1,FDCAN_IT_RX_FIFO0_NEW_MESSAGE,FDCAN_RX_FIFO0)!=HAL_OK)// 激活 FDCAN1 的接收 FIFO
	{
		Error_Handler();
	}

//开启 CAN 通信
	HAL_FDCAN_Start(&hfdcan1);//开启 FDCAN 通信
}

// CAN 接收缓冲区
uint8_t FDCan_RxData[8] = {0};

/*
 * FDCAN 接收回调函数
 * 当收到新报文时自动调用
 * FDCan_RxData[0~7] 保存收到的数据
 */
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
    /* 判断触发类型是否为新消息 */
    if((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != RESET)
    {
        if(hfdcan->Instance == FDCAN1)
        {
            /* 从 RxFifo0 读取报文 */
            if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &RxHeader, FDCan_RxData) != HAL_OK)
            {
                Error_Handler();//读取失败
            }
            /* TODO: 在这里处理接收到的数据 */
			FDCan_NewFrame = 1;
			HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
        }
    }
}


/* USER CODE END 1 */
