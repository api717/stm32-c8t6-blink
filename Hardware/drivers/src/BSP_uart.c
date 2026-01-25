#include "BSP_uart.h"

// 接收缓冲区和状态
uint8_t USART_RX_BUF[USART_REC_LEN];
uint16_t USART_RX_STA = 0;   // 高字节：接收字节数 低字节：接收完成标志(0x8000)

// USART3 中断服务函数
void USART3_IRQHandler(void)
{
    uint8_t res;
    
    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  // 接收中断
    {
        res = USART_ReceiveData(USART3);   // 读取接收到的字节
        
        if((USART_RX_STA & 0x8000) == 0)   // 还未接收完成
        {
            if(USART_RX_STA & 0x4000)      // 已经收到 0x0d
            {
                if(res != 0x0a) USART_RX_STA = 0;   // 不是 0x0a，重新开始
                else USART_RX_STA |= 0x8000;        // 接收完成了
            }
            else
            {
                if(res == 0x0d) USART_RX_STA |= 0x4000;
                else
                {
                    USART_RX_BUF[USART_RX_STA & 0x3FFF] = res;
                    USART_RX_STA++;
                    if(USART_RX_STA > (USART_REC_LEN-1)) USART_RX_STA = 0;  // 防止溢出
                }
            }
        }
    }
}

/**
  * @brief  USART3 初始化（PB10 TX, PB11 RX）
  * @param  bound 波特率（例如 115200）
  * @retval 无
  */
void USART3_Init(u32 bound)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    // 1. 开启时钟
    ESP_USART_APBxClkCmd(ESP_USART_CLK, ENABLE);
    ESP_USART_GPIO_APBxClkCmd(ESP_USART_GPIO_CLK, ENABLE);
    
    // 2. 配置 TX 引脚 (PB10) 为复用推挽输出
    GPIO_InitStructure.GPIO_Pin = ESP_USART_TX_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(ESP_USART_TX_PORT, &GPIO_InitStructure);
    
    // 3. 配置 RX 引脚 (PB11) 为浮空输入（或上拉输入，根据需求）
    GPIO_InitStructure.GPIO_Pin = ESP_USART_RX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   // 浮空输入（推荐）
    // 或 GPIO_Mode_IPU 上拉输入
    GPIO_Init(ESP_USART_RX_PORT, &GPIO_InitStructure);
    
    // 4. USART3 参数配置
    USART_InitStructure.USART_BaudRate = bound;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(ESP_USARTx, &USART_InitStructure);
    
    // 5. 开启接收中断
    USART_ITConfig(ESP_USARTx, USART_IT_RXNE, ENABLE);
    
    // 6. 配置 NVIC 中断
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    // 7. 使能 USART3
    USART_Cmd(ESP_USARTx, ENABLE);
}

/**
  * @brief  发送字符串到 USART3
  * @param  str 要发送的字符串
  * @retval 无
  */
void USART_SendString(USART_TypeDef* USARTx, char *str)
{
    while(*str)
    {
        while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
        USART_SendData(USARTx, *str++);
    }
    while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
}

