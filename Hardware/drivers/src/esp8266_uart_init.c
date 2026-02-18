#include "esp8266_uart_init.h"
#include "systick.h"
  
//USART2 初始化（PA2 TX, PA3 RX）
void USART2_Init(uint32_t baudrate)
{
	GPIO_InitTypeDef GPIO_InitStruct;
    USART_InitTypeDef USART_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    // PA2 TX
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // PA3 RX
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    USART_InitStruct.USART_BaudRate = baudrate;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, &USART_InitStruct);


	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);   // 收到1字节 → 产生中断
	//USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);   // 总线空闲（一帧结束）→ 产生中断
	
	USART2_NVIC_Config();
	
    USART_Cmd(USART2, ENABLE);
	
	
}

//典型的 NVIC 配置函数（你工程里应该叫 ESP8266_USART_NVIC_Configuration() 或类似）
void USART2_NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
  
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;           //中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;   //抢占优先级（越小越高）
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;          //子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    
    NVIC_Init(&NVIC_InitStructure);
}


// ESP8266 RST 和 CH_PD/EN GPIO 初始化
void ESP8266_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStruct.GPIO_Pin = ESP_RST_PIN | ESP_EN_PIN;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(ESP_RST_PORT, &GPIO_InitStruct);

    ESP8266_Reset();     // 先复位
    ESP8266_Enable();    // 再拉高启用
}

//复位 ESP8266（低电平有效）
void ESP8266_Reset(void)
{
    GPIO_ResetBits(ESP_RST_PORT, ESP_RST_PIN);
    delay_ms(200);
    GPIO_SetBits(ESP_RST_PORT, ESP_RST_PIN); 
    delay_ms(2000);     //等待启动
}

//启用ESP8266（CH_PD/EN 高电平有效）
void ESP8266_Enable(void)
{
    GPIO_SetBits(ESP_EN_PORT, ESP_EN_PIN);
}

