#ifndef COMMON_PARAMETERS_H_
#define COMMON_PARAMETERS_H_

/*LED*/
/*类型定义*/
typedef enum
{
	LED_D2,
	LED_D3	
}LED;

typedef enum
{
	ON,
	OFF
}led_state;

/*频率*/
/*类型定义*/
typedef enum
{
	HZ_1 = 1000,
	HZ_8 = 125,
	HZ_10 = 100
}Frequency;

/*宏定义 -- 灯*/
#define  LED_D2_GPIO 	GPIOC
#define  LED_D2_PIN  	GPIO_Pin_13
#define  LED_D2_Periph  RCC_APB2Periph_GPIOC
#define  LED_D3_GPIO 	GPIOB
#define  LED_D3_PIN  	GPIO_Pin_1
#define  LED_D3_Periph  RCC_APB2Periph_GPIOB


#endif
