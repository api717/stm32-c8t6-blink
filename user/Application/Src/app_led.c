#include "app_led.h"
#include "LED_Init.h"
#include "systick.h"

/**
 * @brief: 驱动函数
 * @param: LED led 是对灯的选择 LED_D2, LED_D3	
 * @param: u32 frequency 频率选择
 * @retval: 无
 */
void LED_Flash(LED led,Frequency frequency)
{
		LED_Control(led,ON);
		delay_nms(frequency);
		LED_Control(led,OFF);
		delay_nms(frequency);
}
