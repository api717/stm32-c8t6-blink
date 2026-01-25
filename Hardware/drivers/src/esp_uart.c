#include "esp_uart.h"
#include "systick.h"   // 假设你有 delay_ms / delay_us 函数（DWT 或 SysTick 实现）
                     // 如果没有，可用简单循环延时替换

/**
  * @brief  配置 ESP8266 控制引脚 GPIO (PB8 IO, PB9 RST)
  * @param  无
  * @retval 无
  */
void ESP_Download_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 开启 GPIOB 时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
    // IO (GPIO0) 配置为推挽输出，默认高电平
    GPIO_InitStructure.GPIO_Pin = ESP_IO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(ESP_IO_PORT, &GPIO_InitStructure);
    GPIO_SetBits(ESP_IO_PORT, ESP_IO_PIN);  // 默认高（运行模式）
    
    // RST 配置为推挽输出，默认高电平
    GPIO_InitStructure.GPIO_Pin = ESP_RST_PIN;
    GPIO_Init(ESP_RST_PORT, &GPIO_InitStructure);
    GPIO_SetBits(ESP_RST_PORT, ESP_RST_PIN);  // 默认高（不复位）
}

/**
  * @brief  强制 ESP8266 进入下载模式（拉低 GPIO0 + 复位）
  * @note   调用此函数后，模块进入下载模式，工具可立即烧录
  * @param  无
  * @retval 无
  */
void ESP_Enter_Download_Mode(void)
{
    // 1. 先拉低 GPIO0 (IO)
    GPIO_ResetBits(ESP_IO_PORT, ESP_IO_PIN);
    delay_ms(10);  // 等待稳定
    
    // 2. 拉低 RST 复位（保持 200ms 以上，确保进入 bootloader）
    GPIO_ResetBits(ESP_RST_PORT, ESP_RST_PIN);
    delay_nms(300);  // 保持 300ms
    
    // 3. 拉高 RST（结束复位）
    GPIO_SetBits(ESP_RST_PORT, ESP_RST_PIN);
    delay_ms(100);  // 等待 bootloader 稳定
    
    // GPIO0 保持低，直到烧录完成（烧录完手动拔线或代码拉高）
    // 如果想烧录后自动运行，可在烧录成功后调用 GPIO_SetBits(ESP_IO_PORT, ESP_IO_PIN);
}
