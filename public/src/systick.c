#include "systick.h"

volatile uint32_t sys_ms = 0;


void Systick_Init(void)
{
    SysTick_Config(SystemCoreClock / 1000);
}

void SysTick_Handler(void)
{
    sys_ms++;
}

uint32_t millis(void)
{
    return sys_ms;
}


void delay_ms(uint32_t ms)
{
    uint32_t t = millis();
    while(millis() - t < ms);
}
	
