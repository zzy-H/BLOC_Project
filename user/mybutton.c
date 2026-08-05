#include "mybutton.h"
#include "tim.h"
/* 创建按键对象（一个 KEY 按键，PC13） */
Button key_button;

/* 
	GPIO 读取函数：读取 KEY 引脚电
	按下为高电平
	GPIO 读取函数
*/
uint8_t my_read_key_gpio(uint8_t button_id)
{
	//忽略参数
	(void)button_id;
	
	//获取引脚值
	return HAL_GPIO_ReadPin(KEY_GPIO_Port,KEY_Pin);
}


/* 按键初始化函数：初始化 + 绑定事件回调 + 启动 */
void my_key_init(void)
{
	/*
	* button_init(按键对象, GPIO读取函数, 有效电平, 按键ID)
	* 
	* 第三个参数 active_level：
	*   0 = 按下时 GPIO 为低电平
	*   1 = 按下时 GPIO 为高电平---本项目采用这个
	*
	*	该函数在multi_button.c中
	*/
	button_init(&key_button,my_read_key_gpio,1,1);
	
	/*
		这里绑定需要的按键事件
		button_attach(按键对象, 事件类型, 回调函数, 用户数据)
	
		该函数在multi_button.c中
	*/
	//注册短按按键回调函数
	button_attach(&key_button,BTN_SINGLE_CLICK,on_key_single_click,NULL);
	//注册长按按键回调函数
	button_attach(&key_button, BTN_LONG_PRESS_START, on_key_long_press, NULL);
	
	/*
		注册到链表
		button_start
	*/
	button_start(&key_button);
}


/* ========================================== */
/* 以下写回调函数                          */
/* ========================================== */

/* 单击回调：电机启停切换 */
void on_key_single_click(Button* btn, void* user_data)
{
	(void)btn;
	(void)user_data;
	
	HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
	/* TODO: 在这里写电机启停切换的代码 */
	static uint8_t motor_run = 0;
	motor_run = !motor_run; //每次切换更新状态，默认停止
	
	if (motor_run) {
        // 停止 PWM
        Motor_PWM_Disable();
    } else {
		// 启动 PWM
		Motor_PWM_Enable();
		
    }
	/* 例如：motor_toggle(); */
	
}

uint8_t open_can = 0;
/* 
	长按切换上位机还是点位器控制
	如果是上位机就要LED1闪烁
	点位器就常亮
*/
void on_key_long_press(Button* btn, void* user_data)
{
	(void)btn;
	(void)user_data;
	
	open_can = !open_can;// 切换控制源
}
//电机PWM统一开启
void Motor_PWM_Enable(void)
{
	// 开启通道1 主输出引脚 CH1 (上桥管)
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	// 开启通道1 互补输出引脚 CH1N (下桥管)
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);

	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);

	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3);
	
	//触发 ADC 采样。
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
}

// 电机PWM统一关闭
void Motor_PWM_Disable(void)
{
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1);

    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
    HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_2);

    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3);
    HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_3);

    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_4);
}
