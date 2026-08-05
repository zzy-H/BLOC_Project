#include "mycan.h"
#include "FOC_Model.h"

volatile uint8_t FDCan_NewFrame = 0;

/* CAN 协议：8 字节帧
 * Byte[0]: 0x01=启动, 其他=停止
 * Byte[1]: 0x01=正转, 其他=反转
 * Byte[2]: 速度高 8 位
 * Byte[3]: 速度低 8 位
 * Byte[4~7]: 保留
 */
void FDCAN_Control(void)
{
    uint16_t speed_temp;

    if(!FDCan_NewFrame) return;
    FDCan_NewFrame = 0;

    if(FDCan_RxData[0] == 0x01)                    /* 启动 */
    {
        speed_temp = ((uint16_t)FDCan_RxData[2] << 8) | FDCan_RxData[3];

        if(FDCan_RxData[1] == 0x01)                /* 正转 */
            rtU.SpeedRef = (float)speed_temp;
        else                                        /* 反转 */
            rtU.SpeedRef = -(float)speed_temp;

        rtU.Motor_OnOff = 1.0f;
    }
    else
    {
        rtU.Motor_OnOff = 0.0f;
    }
}
