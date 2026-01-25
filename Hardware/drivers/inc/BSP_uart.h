#ifndef __BSP_USART_H
#define __BSP_USART_H

#include "stm32f10x.h"
#include <stdio.h>   // 用于 printf 重定向

// 定义 USART3 相关宏（PB10 TX, PB11 RX）
#define ESP_USARTx                USART3
#define ESP_USART_CLK             RCC_APB1Periph_USART3
#define ESP_USART_APBxClkCmd      RCC_APB1PeriphClockCmd

#define ESP_USART_GPIO_CLK        RCC_APB2Periph_GPIOB
#define ESP_USART_GPIO_APBxClkCmd RCC_APB2PeriphClockCmd

#define ESP_USART_TX_PORT         GPIOB
#define ESP_USART_TX_PIN          GPIO_Pin_10
#define ESP_USART_RX_PORT         GPIOB
#define ESP_USART_RX_PIN          GPIO_Pin_11

#define ESP_USART_BAUDRATE        115200   // 默认波特率，可改

// 接收缓冲区大小
#define USART_REC_LEN             200
extern uint8_t USART_RX_BUF[USART_REC_LEN];     // 接收缓冲区
extern uint16_t USART_RX_STA;                   // 接收状态标记

// 函数声明
void USART3_Init(u32 bound);                    // USART3 初始化
void USART_SendString(USART_TypeDef* USARTx, char *str);  // 发送字符串


#endif /* __BSP_USART_H */
