#include "mq135_init.h"
#include "systick.h"


uint16_t ADC_Buffer[BUFFER_SIZE];  //DMA目标数组
volatile uint8_t DMA_Buffer_Full_Flag = 0;
/**
 * @brief: 烟雾传感器的初始化
 * @param: 无
 * @retval: 无
 */
void Mq135_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	ADC_GPIO_APBxClock_Fun(ADC_GPIO_CLK, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = ADC_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	
	GPIO_Init(ADC_PORT,&GPIO_InitStructure);

}

/**
 * @brief: 软件触发 + 连续转换 + DMA循环 + DMA传输完成中断。
 * @param: 无
 * @retval: 无
 */

void Mq135_ADC_Config(void)
{
	ADC_InitTypeDef ADC_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//时钟使能
	ADC_APBxClock_FUN(ADC_CLK, ENABLE);
	DMA_APBxClock_FUN(DMA_CLK, ENABLE);
	
	//ADC时钟频率设置
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);
	
	/*ADC模式配置*/
	ADC_DeInit(ADCx);
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;//独立模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE ; //单通道不能扫描
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//是否连续转换
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;	
	ADC_Init(ADCx, &ADC_InitStructure);
	
	//配置规则通道的参数
	ADC_RegularChannelConfig(ADCx, ADC_CHANNEL,1, ADC_SampleTime_55Cycles5);
//	//校准（此时 ADC 和 DMA 都还没启动）
	ADC_Cmd(ADCx, ENABLE);
	delay_us(10);
	//ADC校准
	ADC_ResetCalibration(ADCx);
	while(ADC_GetResetCalibrationStatus(ADCx));
	ADC_StartCalibration(ADCx);
	while(ADC_GetCalibrationStatus(ADCx));
	ADC_Cmd(ADCx, DISABLE);//手册标准不写目前没事
	
	/*DMA初始化配置*/
	DMA_DeInit(DMA_CHANNEL);										  //复位通道1
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;   //ADC数据寄存器
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADC_Buffer;  	  //目标数组内存地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; 	 			  //外设到内存
    DMA_InitStructure.DMA_BufferSize = BUFFER_SIZE;  				  //缓冲大小
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //外设地址不自增
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;           //内存地址自增    
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; //16位数据
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//16位数据  因为数组类型是uint16_t
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;  				   //循环模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;                //高优先级
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  					   //禁止内存到内存
    DMA_Init(DMA_CHANNEL, &DMA_InitStructure);
	
	//开启DMA 中断（可选，传输完成中断）
    DMA_ITConfig(DMA_CHANNEL, DMA_IT_TC, ENABLE);
	
	//开启DMA 通道
    DMA_Cmd(DMA_CHANNEL, ENABLE);
	
	//ADC开启DMA请求
	ADC_DMACmd(ADCx, ENABLE);
	
	//软件启动ADC
	ADC_Cmd(ADCx, ENABLE);	  
	ADC_SoftwareStartConvCmd(ADCx, ENABLE);
	
	/*NVIC配置*/
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;      
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;          
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;                 
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);  
	
}

 void MQ135_Init(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	Mq135_Config();
	Mq135_ADC_Config();
}
/**
 * @brief  DMA1 Channel1 中断服务函数
 * @param: 无
 * @retval: 无        
 */
void DMA1_Channel1_IRQHandler(void)
{
    if (DMA_GetITStatus(DMA1_IT_TC1) != RESET)
    {
		DMA_Buffer_Full_Flag = 1;
        DMA_ClearITPendingBit(DMA1_IT_TC1);
    }  
}



