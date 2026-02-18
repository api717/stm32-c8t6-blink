#ifndef _SYSTICK_H_
#define _SYSTICK_H_


#include "stm32f10x.h"
void Systick_Init(void);
uint32_t millis(void);
void delay_ms(uint32_t ms);

#endif

