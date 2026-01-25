#ifndef MQ135_INIT_H
#define MQ135_INIT_H

/*头文件*/
#include "config.h"

#define BUFFER_SIZE  64  
/*ADC 编号选择*/
#define ADC_APBxClock_FUN         RCC_APB2PeriphClockCmd
#define ADCx                      ADC1
#define ADC_CLK                   RCC_APB2Periph_ADC1

/*AO口 宏定义*/
#define ADC_GPIO_APBxClock_Fun    RCC_APB2PeriphClockCmd
#define ADC_GPIO_CLK              RCC_APB2Periph_GPIOA
#define ADC_PORT                  GPIOA
#define ADC_PIN                   GPIO_Pin_0

/*ADC 通道定义*/
#define ADC_CHANNEL               ADC_Channel_0

/*ADC 中断相关宏定义*/
#define ADC_IRQ                   ADC1_2_IRQn
#define ADC_IQRHandler            ADC1_2_IRQHandler

/*DMA相关宏定义*/
#define DMA_APBxClock_FUN   RCC_AHBPeriphClockCmd
#define DMA_CLK             RCC_AHBPeriph_DMA1
#define DMA_CHANNEL         DMA1_Channel1


/*函数声明*/
void Mq135_Config(void);
void Mq135_ADC_Config(void);
void MQ135_Init(void);

#endif
