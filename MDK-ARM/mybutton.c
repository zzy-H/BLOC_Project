#include "multi_button.h"
#include "main.h"

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
		这里绑定你需要的按键事件
		button_attach(按键对象, 事件类型, 回调函数, 用户数据)
	
		该函数在multi_button.c中
	*/
	//短按按键
	button_attach(&key_button,BTN_SINGLE_CLICK,on_key_single_click,NULL);
	//长按按键
	button_attach(&key_button, BTN_LONG_PRESS_START, on_key_long_press, NULL);
	
	/*
		注册到链表
		button_start
	*/
	button_start(&key_button);
}


/* ========================================== */
/* 以下写你的回调函数                          */
/* ========================================== */

/* 单击回调：电机启停切换 */
void on_key_single_click(Button* btn, void* user_data)
{
	(void)btn;
	(void)user_data;
	/* TODO: 在这里写电机启停切换的代码 */
	/* 例如：motor_toggle(); */
}


/* 长按回调：紧急停止 */
void on_key_long_press(Button* btn, void* user_data)
{
	(void)btn;
	(void)user_data;
	/* TODO: 在这里写紧急停止的代码 */
	/* 例如：motor_emergency_stop(); */
}