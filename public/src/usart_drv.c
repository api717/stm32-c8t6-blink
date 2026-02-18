#include "usart_drv.h"
#include "systick.h"

volatile u8 recv_data = 0; 

/**
 * @brief USART1 初始化
 * @param 波特率
 * @retval 无
 */

void USART1_Init(u32 baud)
{
	//1.开始外设时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_USART1|RCC_APB2Periph_AFIO, ENABLE);
	
	//2.初始化引脚
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽模式
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;//TX
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;  //上拉输入模式
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10; // RX
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);


	//3.配置USART1+初始化 串口
	USART_InitTypeDef USART_InitStruct;
	USART_InitStruct.USART_BaudRate = baud; //波特率设置
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode =USART_Mode_Rx|USART_Mode_Tx;//全双工
	USART_InitStruct.USART_Parity = USART_Parity_No;//不校验
	USART_InitStruct.USART_StopBits =USART_StopBits_1;//1个停止位
	USART_InitStruct.USART_WordLength =USART_WordLength_8b;//8字长
	USART_Init(USART1, &USART_InitStruct);


	//4.配置中断类型：接收数据寄存非空中断
	/*当USART接收到一个字节数据，
	**并将其存入接收数据寄存器（USART_DR）时，
	**该标志位??自动置1
	*/
	/*放在启用中断前*/
	USART_ClearFlag(USART1, USART_FLAG_RXNE | USART_FLAG_ORE);
	/*  USART_ITConfig  中断使能 */
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);//启用
	
	/*  NVIC_Init  NVIC初始化*/
	//用来分配中断优先级的
	//5.NVIC初始化
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel =USART1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;//抢占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;//响应优先级
	NVIC_Init(&NVIC_InitStruct);

	/*  USART_Cmd 串口使能*/
	//启动串口使能USART1
	USART_Cmd(USART1, ENABLE);

}


/**
 * @brief USART 中断服务函数 //在startup里面找
 * @param 无
 * @retval 无
 */

void USART1_IRQHandler(void)
{
	
	//判断是那个中断源产生中断
	//检查指定的 USART 中断发生与否 
	//就是如果是USART_IT_RXNE产生中断 即有数据就会中断
	if(USART_GetITStatus(USART1,USART_IT_RXNE) == SET)
	{
		
		//接收电脑发送过来的数据
		recv_data = USART_ReceiveData(USART1);
		//发回去
		USART_SendData(USART1, recv_data);
	}
}
 

//重定向
int fputc(int c, FILE *stream)
{
	//数据重定向到串口1
	USART_SendData(USART1, (u16)c);
	//等待发送完成
	//未完成标志位会是 reset
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET);
	//返回写入的字符
	return c;
}

