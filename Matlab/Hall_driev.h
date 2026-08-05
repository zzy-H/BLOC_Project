#ifndef __HALL_DRIVER_H
#define __HALL_DRIVER_H

#include "main.h"
#include "FOC_Model.h"

#define PI  3.14159265358979f

//218° 是霍尔的安装偏差角，需要用示波器测出来把 218°转成弧度
#define PHASE_SHIFT_ANGLE (float)(218.0f/360.0f*2.0f*PI)
#define POSITIVE  1
#define NEGATIVE  -1

typedef struct
{
	uint8_t HallState; //霍尔状态
	
	float AvrElSpeedDpp; //电角速度 rad/s
	
	float MeasuredElAngle; //检测到的电角度
	
	float HallElAngle; //计算后的电角度
	
	int8_t Direction; //电机方向
	
	float HallSpeed; //机械转速 rpm/min
	
	float TempSpeed; //机械转速 rpm/min
	
	float DeltaAngle; //角度补偿
	
	uint8_t bPrevHallState; //前一次霍尔状态
	
	float hHighSpeedCapture; //定时器检测霍尔信号时的计数
	
	float MeasureTest; //测试值
	
	uint8_t bHallUpdated;  // 霍尔边沿更新标志，TIM4 回调置 1，ADC 回调查后清 0
	
	uint8_t bValidTransition;  // 标记是否为有效霍尔跳变
}HALL_Handle_t;


float FirstOrderRC_LPF(float out_prev, float in, float alpha);

void HALL_Init_Electrical_Angle(void);

void HALL_Get_Electrical_Angle(void *pHandleVoid);
#endif
