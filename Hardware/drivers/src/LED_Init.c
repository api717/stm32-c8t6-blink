#include "LED_Init.h"

/**
 * @brief: LED-D2,LED-D3的驱动,低电平点亮
 * @param: 无
 * @retval: 无
 */
void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_APB2PeriphClockCmd(LED_D2_Periph|LED_D3_Periph,ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = LED_D2_PIN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LED_D2_GPIO,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = LED_D3_PIN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LED_D3_GPIO,&GPIO_InitStruct);
	
	GPIO_SetBits(LED_D3_GPIO,LED_D3_PIN);
	GPIO_ResetBits(LED_D2_GPIO,LED_D2_PIN);
}
/**
 * @brief: 驱动函数
 * @param: LED led 是对灯的选择  LED_D2, LED_D3	
 * @param: led_state 是对状态的选择  ON开 OFF关
 * @retval: 无
 */
void LED_Control(LED led,led_state state)
{
	if(led == LED_D2)
	{
		if(state == ON)       GPIO_ResetBits(LED_D2_GPIO,LED_D2_PIN); 
		else if(state == OFF) GPIO_SetBits(LED_D2_GPIO,LED_D2_PIN);
	}
	else if(led == LED_D3)
	{
		if(state == ON)       GPIO_ResetBits(LED_D3_GPIO,LED_D3_PIN);
		else if(state == OFF) GPIO_SetBits(LED_D3_GPIO,LED_D3_PIN);
	}
		
}
