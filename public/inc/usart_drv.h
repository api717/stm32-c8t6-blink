#ifndef _USART_DRV_H_
#define _USART_DRV_H_

#include "stm32f10x.h"
#include <stdio.h>

extern volatile u8 recv_data;
void USART1_Init(u32 baud);

#endif
