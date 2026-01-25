#include "app_mq135.h"
#include "mq135_init.h"
#include "math.h"


extern uint8_t DMA_Buffer_Full_Flag;
extern uint16_t ADC_Buffer[BUFFER_SIZE];
float ppm = 0;
/**
  * @brief  求ppm
  * @param  无
  * @retval ppm
  * @note
  * 根据手册提供的各污染气体灵敏度 拟合成幂函数
  * 需要根据Rs/R0推算ppm，所以拟合函数时，x轴为Rs/R0，y轴为ppm，推导出y=ax^b
  * 图表没有每个点对应具体数值只能大致估计，所以测量值存在误差，想要完全精确请根据环境做多次标定
  */
float MQ135_Get_PPM(uint16_t ADC_ConvertedValue)
{
	/* 读取AO输出电压 */
    float vrl = (float)ADC_ConvertedValue / 4096 * VC;
    /* 换算Rs电阻 */
    float Rs = (float)(VC - vrl) * RL / vrl;
                       
    float Rs0 = Rs/R0;  /* Rs/R0 */
    
    /* y=ax^b x为Rs/R0，ab的取值根据数据手册图表自行拟合成幂函数 */
    ppm =  A*pow(Rs/R0,B);
	
    if (ppm < 10.0f) ppm = 10.0f;
    if (ppm > 1000.0f) ppm = 1000.0f;
	
	return ppm;
}
/**
  * @brief  求ADC平均值
  * @param  无
  * @retval 无
  * 
  */
uint16_t Get_Average(void)
{
	float sum = 0;
	uint16_t average = 0;
	if(DMA_Buffer_Full_Flag)
	{
		for(int i = 0;i < BUFFER_SIZE;i ++)
		{
			sum += ADC_Buffer[i];
		}
		average = sum / BUFFER_SIZE;
		DMA_Buffer_Full_Flag = 0;
	}
	return average;
}

float APP_MQ135(void)
{
	uint16_t average = Get_Average();
	
	if(average)
	{
		ppm = MQ135_Get_PPM(average);
	}
	return ppm;
}
