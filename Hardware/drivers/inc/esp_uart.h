#ifndef __ESP_DOWNLOAD_H
#define __ESP_DOWNLOAD_H

#include "stm32f10x.h"

// ESP8266 控制引脚定义（可根据你的接线修改）
#define ESP_IO_PORT     GPIOB
#define ESP_IO_PIN      GPIO_Pin_8   // 接模块的 IO (GPIO0)

#define ESP_RST_PORT    GPIOB
#define ESP_RST_PIN     GPIO_Pin_9   // 接模块的 RST

// 函数声明
void ESP_Download_GPIO_Config(void);
void ESP_Enter_Download_Mode(void);

#endif /* __ESP_DOWNLOAD_H */
