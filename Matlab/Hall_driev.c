#include "hall_driev.h"

#include "tim.h"
#include <math.h>
#include "VFmode.h"
#include "adc.h"
// 定义全局变量
HALL_Handle_t HALL_Handle;

//一阶低通滤波
float FirstOrderRC_LPF(float out_prev, float in, float alpha)
{
	// y(n) = y(n-1) + α*(x(n) - y(n-1))
	/*
		α 滤波系数（平滑系数，\(0<\alpha\le1\)）
		\(x(n)\)：本次原始输入 in
	*/
    return out_prev + alpha * (in - out_prev);
}

//上电读初始值--获取初始化角度
void HALL_Init_Electrical_Angle(void)
{
	
	HALL_Handle_t *phandle = &HALL_Handle;
	
	/*读取三个霍尔传感器的接口，确定当前转子具体在那个扇区*/
	HALL_Handle.HallState = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_8);
	HALL_Handle.HallState |= HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7) << 1;
    HALL_Handle.HallState |= HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6) << 2;
	
	//读一下霍尔状态，判断在那个扇区
	/*
		最终角度 = PHASE_SHIFT（安装偏差，每台电机不同）
          + PI/6（120° 霍尔标准偏置，固定）
          + 扇区偏移（0°/60°/120°/180°/240°/300°）
          + 插值修正（60° 内的速度插值）
	*/
	switch (HALL_Handle.HallState)
	{
		case 5:
		{
			HALL_Handle.HallElAngle = PHASE_SHIFT_ANGLE + PI/6;	
			break;
		}
		
		case 4:
		{
			//(PI/3.0f)
			phandle->HallElAngle = (PI/3.0f)+PHASE_SHIFT_ANGLE + PI/6;
			break;
		}
		
		case 6:
		{
			phandle->HallElAngle = (2.0f*PI/3.0f)+PHASE_SHIFT_ANGLE + PI/6;
			break;
		}
		
		case 2:
		{
			phandle->HallElAngle = (3.0f*PI/3.0f)+PHASE_SHIFT_ANGLE + PI/6;
			break;
		}
		
		case 3:
		{
			phandle->HallElAngle = (4.0f*PI/3.0f)+PHASE_SHIFT_ANGLE + PI/6;
			break;
		}
		
		case 1:
		{
			phandle->HallElAngle = (5.0f*PI/3.0f)+PHASE_SHIFT_ANGLE + PI/6;
			break;
		}
		default:
		{
			break;
		}
	}
	
	phandle->MeasuredElAngle = phandle->HallElAngle;  // 初始时两者对齐
    phandle->bHallUpdated = 0;                          // 初始时无待处理的边沿
	
	phandle->bValidTransition = 0;      // 新增：初始无有效跳变
	phandle->AvrElSpeedDpp = 0.0f;      // 新增：速度清零
	phandle->HallSpeed = 0.0f;          // 新增：霍尔速度清零
	phandle->Direction = POSITIVE;      // 新增：默认正转
	
}

//正转：1 → 5 → 4 → 6 → 2 → 3 → 1，
//反转：1 → 3 → 2 → 6 → 4 → 5 → 1

/*编写霍尔正反转角度计算代码，并将该函数放到定时器 4 的回调函数中去执行：*/
void HALL_Get_Electrical_Angle(void *pHandleVoid)
{
	// 将通用空指针强制转换为霍尔结构体句柄指针，方便操作霍尔相关参数
	HALL_Handle_t *phandle = (HALL_Handle_t *)pHandleVoid;
	
	// 读取TIM4通道1捕获寄存器的值，获取定时器高速捕获计数值（用于计算电机转速）
	phandle->hHighSpeedCapture = HAL_TIM_ReadCapturedValue(&htim4,TIM_CHANNEL_1);
	
	if (phandle->hHighSpeedCapture == 0) {
		return;  // 霍尔还没触发，直接返回，避免除零
	}
	
	// 保存上一次的霍尔状态，用于前后两次霍尔编码对比，判断换相边沿、转向
	phandle->bPrevHallState = phandle->HallState;
	
	/*读取三个霍尔传感器的接口，确定当前转子具体在那个扇区*/
	HALL_Handle.HallState = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_8);
	HALL_Handle.HallState |= HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7) << 1;
    HALL_Handle.HallState |= HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6) << 2;
	
	/**********************************************************************
	* 功能：霍尔状态解码与电角度基准标定
	* 逻辑：根据「当前霍尔状态 + 上一时刻霍尔状态」的跳转关系
	*       1. 判断电机正/反转方向
	*       2. 标定本次霍尔跳变沿对应的电气角度基准
	* 基准定义：正转方向 1→5 跳变沿为角度原点，对应 PHASE_SHIFT_ANGLE
	*          每跨过一个霍尔区间，电气角度递增 π/3（60°）
	**********************************************************************/
	switch (phandle->HallState)
	{
		case 5:
		{
			// 当前霍尔状态为 5（二进制 0b101）
			if(phandle->bPrevHallState == 1)
			{
				// 正转跳转：1 → 5，对应电角度基准原点
				phandle->MeasuredElAngle = PHASE_SHIFT_ANGLE;
				phandle->Direction = POSITIVE;
				phandle->bValidTransition = 1;   // ← 加这行
			}
			else if (phandle->bPrevHallState == 4)
			{
				// 反转跳转：4 → 5，跨过 5/4 区间边界，电角度偏移 π/3（60°）
				phandle->MeasuredElAngle = PHASE_SHIFT_ANGLE + PI / 3.0f;
				phandle->Direction = NEGATIVE;
				phandle->bValidTransition = 1;   // ← 加这行
			}
			else
			{
				// 非正常状态跳转（信号干扰/霍尔故障），不更新角度与方向
				phandle->bValidTransition = 0;   // ← 加这行
			}
			break;
		}

		case 4:
		{
			// 当前霍尔状态为 4（二进制 0b100）
			if(phandle->bPrevHallState == 5)
			{
				// 正转跳转：5 → 4，跨过 5/4 区间边界，电角度偏移 π/3（60°）
				phandle->MeasuredElAngle = PHASE_SHIFT_ANGLE + PI/3.0f;
				phandle->Direction = POSITIVE;
				phandle->bValidTransition = 1;   // ← 加这行
			}
			else if (phandle->bPrevHallState == 6)
			{
				// 反转跳转：6 → 4，跨过 4/6 区间边界，电角度偏移 2π/3（120°）
				phandle->MeasuredElAngle = PHASE_SHIFT_ANGLE + 2*PI / 3.0f;
				phandle->Direction = NEGATIVE;
				phandle->bValidTransition = 1;   // ← 加这行
			}
			else
			{
				// 非正常状态跳转（信号干扰/霍尔故障），不更新角度与方向
				phandle->bValidTransition = 0;   // ← 加这行
			}
			break;
		}

		case 6:
		{
			// 当前霍尔状态为 6（二进制 0b110）
			if(phandle->bPrevHallState == 4)
			{
				// 正转跳转：4 → 6，跨过 4/6 区间边界，电角度偏移 2π/3（120°）
				phandle->MeasuredElAngle = PHASE_SHIFT_ANGLE + 2*PI/3.0f;
				phandle->Direction = POSITIVE;
				phandle->bValidTransition = 1;
			}
			else if (phandle->bPrevHallState == 2)
			{
				// 反转跳转：2 → 6，跨过 6/2 区间边界，电角度偏移 π（180°）
				phandle->MeasuredElAngle = PHASE_SHIFT_ANGLE + PI;
				phandle->Direction = NEGATIVE;
				phandle->bValidTransition = 1;
			}
			else
			{
				// 非正常状态跳转（信号干扰/霍尔故障），不更新角度与方向
				phandle->bValidTransition = 0;
			}
			break;
		}

		case 2:
		{
			// 当前霍尔状态为 2（二进制 0b010）
			if(phandle->bPrevHallState == 6)
			{
				// 正转跳转：6 → 2，跨过 6/2 区间边界，电角度偏移 π（180°）
				phandle->MeasuredElAngle = PHASE_SHIFT_ANGLE + PI;
				phandle->Direction = POSITIVE;
				phandle->bValidTransition = 1;
			}
			else if (phandle->bPrevHallState == 3)
			{
				// 反转跳转：3 → 2，跨过 2/3 区间边界，电角度偏移 4π/3（240°）
				phandle->MeasuredElAngle = PHASE_SHIFT_ANGLE + 4*PI/3;
				phandle->Direction = NEGATIVE;
				phandle->bValidTransition = 1;
			}
			else
			{
				// 非正常状态跳转（信号干扰/霍尔故障），不更新角度与方向
				phandle->bValidTransition = 0;
			}
			break;
		}

		case 3:
		{
			// 当前霍尔状态为 3（二进制 0b011）
			if(phandle->bPrevHallState == 2)
			{
				// 正转跳转：2 → 3，跨过 2/3 区间边界，电角度偏移 4π/3（240°）
				phandle->MeasuredElAngle = PHASE_SHIFT_ANGLE + 4*PI/3;
				phandle->Direction = POSITIVE;
				phandle->bValidTransition = 1;
			}
			else if (phandle->bPrevHallState == 1)
			{
				// 反转跳转：1 → 3，跨过 3/1 区间边界，电角度偏移 5π/3（300°）
				phandle->MeasuredElAngle = PHASE_SHIFT_ANGLE + 5*PI/3;
				phandle->Direction = NEGATIVE;
				phandle->bValidTransition = 1;
			}
			else
			{
				// 非正常状态跳转（信号干扰/霍尔故障），不更新角度与方向
				phandle->bValidTransition = 0;
			}
			break;
		}

		case 1:
		{
			// 当前霍尔状态为 1（二进制 0b001）
			if(phandle->bPrevHallState == 3)
			{
				// 正转跳转：3 → 1，跨过 3/1 区间边界，电角度偏移 5π/3（300°）
				phandle->MeasuredElAngle = PHASE_SHIFT_ANGLE + 5*PI/3;
				phandle->Direction = POSITIVE;
				phandle->bValidTransition = 1;
			}
			else if (phandle->bPrevHallState == 5)
			{
				// 反转跳转：5 → 1，跨过 1/5 区间边界，回到电角度基准原点
				phandle->MeasuredElAngle = PHASE_SHIFT_ANGLE;
				phandle->Direction = NEGATIVE;
				phandle->bValidTransition = 1;
			}
			else
			{
				// 非正常状态跳转（信号干扰/霍尔故障），不更新角度与方向
				phandle->bValidTransition = 0;
			}
			break;
		}
		
	}
	
	if (!phandle->bValidTransition)
	{
		return;  // 无效跳转直接返回，不计算速度和角度修正
	}
	
	// 电角度归一化限幅：将实测电气角度约束到 [0, 2π) 标准周期区间
	// 电角度是周期量，一个完整电周期为 2π；超出区间时通过加减整周期，映射回有效范围
	if (phandle->MeasuredElAngle < 0.0f)
	{
		// 角度出现负值时，叠加一个完整电周期 2π，将其折回到正角度区间
		phandle->MeasuredElAngle += 2.0f * PI;
	}
	else if (phandle->MeasuredElAngle > (2.0f * PI))
	{
		// 角度超过一个完整电周期时，减去一个完整电周期 2π，折回有效区间内
		phandle->MeasuredElAngle -= 2.0f * PI;
	}
	
	//10KHz 下的平均电角速度 FOC 模型在 ADC 中断里执行，所以频率是 10000 一圈 2PI 分成 6 个扇区，每个扇占 PI/3
	// 计算两次霍尔跳变间的平均电角速度，单位：弧度/控制周期（Delta Per Period）
	// 原理：电角度增量 ÷ 两次跳变间隔对应的控制周期数，得到每个控制周期的电角度增量
	// 用于两次霍尔跳变之间的角度线性插值，输出连续平滑的电角度
	//3.2Mhz是预分频值（50-1）-------160M/50=3.2
	phandle->AvrElSpeedDpp = (PI/3) / ((phandle->hHighSpeedCapture / 3200000) * 10000);

	//将 rad/s 转换为 rad/min（rpm）
	/*
		// 通用公式：PolePair 为电机极对数 == 2
		phandle->SpeedRpm = (PI/3) / (phandle->hHighSpeedCapture / 3200000) * 60.0f / (2PI * PolePair);
	*/
	phandle->TempSpeed = (PI/3)/(phandle->hHighSpeedCapture/3200000)*30/(2*PI);
	phandle->TempSpeed = phandle->TempSpeed * phandle->Direction;//正转*1，反转*-1

	/* 一阶滤波 */
	phandle->HallSpeed = FirstOrderRC_LPF(phandle->HallSpeed ,phandle->TempSpeed,0.2379f);
	phandle->AvrElSpeedDpp = phandle->AvrElSpeedDpp * phandle->Direction;
	/*
		//当前方法是基于之前 60°的霍尔时间去处理的，并不是当前的值，我们默认每个扇区速度一样。
		但霍尔测量总归是有误差的，所以要进行补偿，补偿方法的话就是霍尔测量的角度减去电流环中累加
		的角度再除 10000；
	*/
	phandle->DeltaAngle = (phandle->MeasuredElAngle - phandle->HallElAngle) / 10000;
	
	 phandle->bHallUpdated = 1;       
}
