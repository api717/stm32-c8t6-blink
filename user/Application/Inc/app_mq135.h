#ifndef APP_MQ135_H
#define APP_MQ135_H

/*头文件*/
#include "config.h"

/*换算常量*/
#define RL                               1     /* 根据硬件原理图可知：RL = 1k */ 
#define R0                               42.0    /* MQ135在洁净空气中的阻值，官方数据手册没有给出，这是实验测试得出，想要准确请多次测试 */ 
#define VC                               5.0   /* MQ135供电电压,根据实际供电修改，默认接5V */
#define A                                7.66  /* y=ax^b 的 a */
#define B                                -2.03 /* y=ax^b 的 b */

/*函数声明*/
float MQ135_Get_PPM(uint16_t ADC_ConvertedValue);
uint16_t Get_Average(void);
float APP_MQ135(void);

#endif

