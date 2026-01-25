#ifndef BUZZER_INIT_H_
#define BUZZER_INIT_H_

/*头文件定义*/
#include "config.h"

/* 定义蜂鸣器连接的GPIO端口, 用户只需要修改下面的代码即可改变控制的蜂鸣器引脚 */
#define BEEP_GPIO_PORT    	GPIOB			            /* GPIO端口 */
#define BEEP_GPIO_CLK 	    RCC_APB2Periph_GPIOB		/* GPIO端口时钟 */
#define BEEP_GPIO_PIN		GPIO_Pin_0			        /* 连接到蜂鸣器的GPIO */

/* 高电平时，蜂鸣器响 */
#define ON  1
#define OFF 0

/* 定义控制IO的宏 */
#define BEEP_ON                   GPIO_SetBits(BEEP_GPIO_PORT,BEEP_GPIO_PIN);
#define BEEP_OFF                  GPIO_ResetBits(BEEP_GPIO_PORT,BEEP_GPIO_PIN);    

/*函数声明*/
void Buzzer_config(void);

#endif
