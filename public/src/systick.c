#include "systick.h"

volatile u32 nus,nms;
/*
** @ brief: 初始化systick
** @ param: systick系统时间频率基准值
** @ retval: 无
*/
void Systick_Init(u8 sysclk)
{
	//选择AHB/8 
	SysTick->CTRL &= ~(0x01 << 2);
	//计算1ms 和 1us 计时所需次数
	/*
	*sysclk = f /1000000 假设load就为 72 此时 秒数为
	* 72 / 9000000 = 8 /1000000 就是8 us，在函数中load是为 n * nms 
	*而nms 想要是 代表 1 us 所装的 load值 所以 是 load = sysclk /8
	*/
	nus = sysclk / 8;
	nms = nus *1000;
}


void delay_ms(u32 n)
{
	//1.设置重装载值
	SysTick->LOAD = n*nms;
	//road上限为 0FFFFFF 约为 1.86秒

	//2.清除VAl寄存器,防止上次计时残留
	SysTick->VAL = 0;

	//3.开启定时器
	SysTick->CTRL |= 0x01;

	//4.等待计时完成(COUNTFLAG置1)
	u32 temp;
	do{
		temp = SysTick->CTRL;
	}while((temp & 0x01) && !(temp & (0x01 << 16)));

	//5.手动关闭定时器
	SysTick->CTRL &= 0x01;

	//6.清除VAl寄存器
	SysTick->VAL = 0;
	
}

/**
 * @brief 微秒级定时(最大不超过1.86s)
 * @param n定时的微秒数
 * @retval 无
 */
void delay_us(u32 n)
{
	//1.设置重装载值
	SysTick->LOAD = n * nus;

	//2.清除VAl寄存器,防止上次计时残留
	SysTick->VAL = 0;

	//3.开启定时器
	SysTick->CTRL |= 0x01;

	//4.等待计时完成(COUNTFLAG置1)
	u32 temp;
	do{
		temp = SysTick->CTRL;
	}while((temp & 0x01) && !(temp & (0x01 << 16)));

	//5.手动关闭定时器
	SysTick->CTRL &= ~(0x01);

	//6.清除VAl寄存器
	SysTick->VAL = 0;
}


/**
 * @brief 毫秒级定时
 * @param n定时的毫秒数
 * @retval 无
 */
void delay_nms(u32 n)
{
	u32 remain,num,i;

	//计算定时的轮数
	num = n / 1800;

	//余数最后单独定时
	remain = n % 1800;

	//开始定时
	for(i = 0;i < num;i++)
		delay_ms(1800);

	//余数定时
	if(remain != 0)
		delay_ms(remain);
}


