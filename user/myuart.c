#include "myuart.h"
#include "FOC_Model.h"
#include <string.h>
#include "usart.h"
#include "mybutton.h"

RxData_t rxdata = {0};//发送缓冲区
RecvData_t recv;//接收缓冲区
static uint8_t flag = 0;
extern uint8_t Hal_State;

/**
 * @brief VOFA上位机通信串口初始化函数
 * @note 使用USART3配合DMA空闲中断实现不定长帧接收
 */
void BSP_Vofa_Init(void)
{
    // 开启串口DMA接收，数据存入接收缓冲区，最大接收长度RXLEN
	HAL_UART_Receive_DMA(&huart3, rxdata.buff, RXLEN);
    // 清除串口空闲中断标志，防止上电误触发中断
	__HAL_UART_CLEAR_IDLEFLAG(&huart3);
    // 使能串口空闲中断：总线空闲时触发中断，用于判断一帧数据接收完成
    __HAL_UART_ENABLE_IT(&huart3, UART_IT_IDLE);
}

/**
 * @brief USART3串口空闲中断处理函数
 * @desc 当串口总线空闲时进入此函数，计算本次接收的数据长度并置接收完成标志
 */
void UART3_RxHandler(void)
{
    // 判断是否产生串口空闲中断标志
    if(__HAL_UART_GET_FLAG(&huart3, UART_FLAG_IDLE) == SET)
    {
		HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
		
        // 清零空闲中断标志位，避免重复触发中断
        __HAL_UART_CLEAR_IDLEFLAG(&huart3);
		
        // 停止DMA接收，锁住当前缓存内的数据不再被覆盖
        HAL_UART_DMAStop(&huart3);
		
        // DMA剩余计数寄存器保存还未接收的字节数，总长度减去剩余值 = 实际收到字节数
        rxdata.cnt = RXLEN - __HAL_DMA_GET_COUNTER(huart3.hdmarx);
		
        // 置位接收完成标志，主循环中检测flag即可处理整帧数据
        rxdata.flag = 1;
    }
}

/**
 * @brief USART3串口接收数据解析函数，配合VOFA上位机下发控制参数
 * @note 通信帧协议格式：帧头0xAA 0x55 + 指令码 + 4字节float浮点数据
 * 帧结构：[0]0xAA [1]0x55 [2]指令号 [3~6]4字节浮点参数
 */
void USART3_Anylze(void)
{
	
    // 无接收完成标志，直接退出，无需解析
    if(rxdata.flag != 1) return;
	
    // 校验帧头，帧头不为0xAA、0x55判定为错误帧
    if(rxdata.buff[0] != 0xaa || rxdata.buff[1] != 0x55)
    {
        // 清空接收缓存、计数、标志位
        memset(&rxdata, 0, sizeof(rxdata));
        // 重新开启DMA接收，等待下一帧数据
        HAL_UART_Receive_DMA(&huart3, rxdata.buff, RXLEN);
        return;
    }

    // 提取帧内3~6号字节，存入共用体buff，自动拼接为float浮点数值
    recv.buff[0] = rxdata.buff[3];
    recv.buff[1] = rxdata.buff[4];
    recv.buff[2] = rxdata.buff[5];
    recv.buff[3] = rxdata.buff[6];

    // 根据指令码区分不同下发参数，更新对应控制变量
    switch(rxdata.buff[2])
    {
		case 0x01:	/*指令01：启停电机*/
			flag = !flag;
			if(flag == 1)
			{
				Motor_PWM_Enable();
			}
			else{
				Motor_PWM_Disable();
			}
			
			break;
		case 0x02:  /* 指令02：目标转速设定值 */
			rtU.SpeedRef = recv.f;
			break;
		case 0x03:  /* 指令03：速度环比例系数Kp */
			rt_Simulink_Struct.spd_kp = recv.f;
			break;
		case 0x04:  /* 指令04：速度环积分系数Ki */
			rt_Simulink_Struct.spd_ki = recv.f;
			break;
		case 0x05:  /* 指令05：电流环比例系数Kp */
			rt_Simulink_Struct.CurrKp = recv.f;
			break;
		case 0x06:  /* 指令06：电流环积分系数Ki */
			rt_Simulink_Struct.CurrKi = recv.f;
			break;
		case 0x0C:  /* 指令0C：电机运行模式 0-无感FOC 1-有感霍尔FOC */
			Hal_State = (uint8_t)recv.f;
			break;
		default:
			// 未知指令，不执行任何操作
			break;
    }

    // 一帧数据解析完成，清空接收缓存
    memset(&rxdata, 0, sizeof(rxdata));
    // 重启DMA接收，等待上位机下发下一帧指令
    HAL_UART_Receive_DMA(&huart3, rxdata.buff, RXLEN);
}
