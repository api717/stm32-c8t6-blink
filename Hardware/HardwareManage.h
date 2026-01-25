#ifndef HARDWAREMANAGE_H_
#define HARDWAREMANAGE_H_

/*头文件定义*/
#include "LED_Init.h"
#include "usart_drv.h"
#include "systick.h"
#include "bsp_iic_debug.h"
#include "bsp_oled_debug.h"
#include "Buzzer_init.h"
#include "mq135_init.h"
#include "DHT11_init.h"
#include "esp_uart.h"
#include "BSP_uart.h"

/*函数声明*/
void Hardware_Init(void);


#endif  
