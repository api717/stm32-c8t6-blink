#include "HardwareManage.h"

void Hardware_Init(void)
{
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //视工程分组设置
	
	Systick_Init();
	//串口1的初始化
	USART1_Init(115200);
	USART2_Init(115200);
	//esp8266硬件初始化
	ESP8266_GPIO_Init();
	//灯的初始化
	//LED_Init();
	
	//顺序不能反
	//IIC_GPIO_Config();
	//OLED_Init();
	
	//蜂鸣器初始化
	//Buzzer_config();
	
	//烟雾传感器初始化
	//MQ135_Init();
	
	//温湿度传感器初始化
	//DHT11_Init();
}
