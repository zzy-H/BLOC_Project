#ifndef __MYCAN_H
#define __MYCAN_H

#include "main.h"

/* CAN 接收到的数据（在 fdcan.c 的 HAL_FDCAN_RxFifo0Callback 中更新）*/
extern uint8_t FDCan_RxData[8];
extern volatile uint8_t FDCan_NewFrame;   /* 有新帧标志，main 循环里查 */

/* CAN 控制解析：根据 FDCan_RxData 协议设置电机参数 */
void FDCAN_Control(void);

#endif
