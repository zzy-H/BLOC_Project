#ifndef __MYUART_H
#define __MYUART_H

#include "main.h"

#define RXLEN  32

typedef struct {
    uint8_t  buff[RXLEN];
    uint16_t cnt;
    uint8_t  flag;
} RxData_t;

typedef union {
    uint8_t buff[4];
    float   f;
} RecvData_t;

extern RxData_t rxdata;
extern RecvData_t recv;

/* 初始化 DMA 接收 */
void BSP_Vofa_Init(void);

/* 串口 IDLE 中断处理（放在 USART3_IRQHandler 里调）*/
void UART3_RxHandler(void);

/* 帧解析（放在 main 循环里调）*/
void USART3_Anylze(void);

#endif
