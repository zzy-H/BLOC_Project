#ifndef __MYBUTTON_H
#define __MYBUTTON_H
#include "multi_button.h"
#include "main.h"

/* 单击回调：电机启停切换 */
void on_key_single_click(Button* btn, void* user_data);
/* 长按回调：紧急停止 */
void on_key_long_press(Button* btn, void* user_data);
/* 按键初始化函数：初始化 + 绑定事件回调 + 启动 */
void my_key_init(void);
/* GPIO 读取函数：读取 KEY 引脚电平*/
uint8_t my_read_key_gpio(uint8_t button_id);

//电机PWM统一开启
void Motor_PWM_Enable(void);
// 电机PWM统一关闭
void Motor_PWM_Disable(void);


#endif
