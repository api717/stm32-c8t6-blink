#ifndef _ESP8266_UART_INIT_H
#define _ESP8266_UART_INIT_H

#include "config.h"

#define ESP_RST_PORT    GPIOB
#define ESP_RST_PIN     GPIO_Pin_1     // PB1 ½Ó ESP8266 RST
#define ESP_EN_PORT     GPIOB
#define ESP_EN_PIN      GPIO_Pin_3     // PB2 ½Ó ESP8266 CH_PD/EN

void USART2_Init(uint32_t baudrate);
void USART2_NVIC_Config(void);
void ESP8266_GPIO_Init(void);
void ESP8266_Reset(void);
void ESP8266_Enable(void);


#endif
