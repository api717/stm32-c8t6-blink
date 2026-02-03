#include "app_esp8266.h"
#include "esp8266_uart_init.h"
#include "systick.h"
#include <stdio.h>
#include <string.h>

// 接收缓冲（全局）

char esp_rx_buf[SIZE];
uint16_t esp_rx_len = 0;
char token[] = "version=2018-10-31&res=products%2FME3239j3An%2Fdevices%2Fdev0&et=4102444800&method=md5&sign=ApKkPfCguhXM8Qq4zbxv1A%3D%3D";


// USART2 中断接收（简单实现）
void USART2_IRQHandler(void)
{
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        uint8_t data = USART_ReceiveData(USART2);  // 读取 DR 会自动清除 RXNE 标志

        if(esp_rx_len < SIZE - 1)
        {
            esp_rx_buf[esp_rx_len++] = data;
        }
        
    }
}

// 发送 AT 指令并等待响应（阻塞）
uint8_t ESP8266_SendAT(const char* cmd, const char* resp, uint32_t timeout_ms)
{
    uint32_t tick = 0;
    esp_rx_len = 0;  // 清空接收缓冲
    memset(esp_rx_buf, 0, sizeof(esp_rx_buf));

    // 发送命令 + \r\n
    char buf[SIZE];
    sprintf(buf, "%s\r\n", cmd);
    for(uint16_t i = 0; i < strlen(buf); i++)
    {
        USART_SendData(USART2, buf[i]);
        while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
    }

    // 等待响应
    while(tick < timeout_ms)
    {
        delay_ms(1);
        tick++;

        if(strstr(esp_rx_buf, resp) != NULL)
        {
            return 1; // 成功
        }
        if(strstr(esp_rx_buf, "ERROR") != NULL)
        {
            return 0; // 失败
        }
    }
    return 0;
}

// 发送 MQTT 上报数据（示例）
void ESP8266_PublishData(void)
{
    char json[256];
	int id = 717;
    sprintf(json, "{\"id\":\"%d\",\"version\":\"1.0\",\"params\":{\"CurrentTemperature\":{\"value\":25.5},\"RelativeHumidity\":{\"value\":68.0},\"NH3Concentration\":{\"value\":450.0}}}",id);

    uint16_t len = strlen(json);
	
	printf("[PUB] 准备上报数据，长度=%d 字节\r\n", len);
    printf("[PUB] JSON内容: %s\r\n", json);           // ← 如果太长可以注释掉
	printf("[PUB] 发送 AT+MQTTPUBRAW...\r\n");
	
    ESP8266_SendAT("AT+MQTTPUBRAW=0,\"$sys/ME3239j3An/dev0/thing/property/post\",%d,0,0", ">", 500);
	printf("[PUB] 收到 > ，开始发送 JSON 数据...\r\n");
    // 等待 > 后发送 JSON
    for(uint16_t i = 0; i < len; i++)
    {
        USART_SendData(USART2, json[i]);
        while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
    }
	
    USART_SendData(USART2, '\r');
    USART_SendData(USART2, '\n');
	
	delay_ms(50);
    
    printf("[PUB] JSON 数据已发送完成\r\n");
}

// 主逻辑示例
void Application_Init(void)
{
	
	printf("\r\n\r\n===== ESP8266 初始化开始 =====\r\n");
    ESP8266_GPIO_Init();

    //基本初始化
	printf("1. 测试 AT 命令...\r\n");
	
    if (ESP8266_SendAT("AT","OK", 1000) == 1) 
	{
        printf("→ AT 通信正常\r\n");
    } 
	else 
	{
        printf("→ AT 通信失败！请检查接线/波特率\r\n");
    }
	
	//复位
    printf("2. 复位模块...\r\n");
    ESP8266_SendAT("AT+RST", "OK", 5000);
    delay_ms(3000);   // 等待重启完成
    printf("→ 复位完成，等待 3 秒\r\n");
	

    // 设置 Station 模式
    printf("3. 设置为 Station 模式...\r\n");
    if (ESP8266_SendAT("AT+CWMODE=1", "OK", 1000) == 1) 
	{
        printf("→ 设置为 Station 模式成功\r\n");
    }
	
    // 连接 WiFi
    printf("4. 正在连接 WiFi TP-LINK_ADEB ...\r\n");
    if (ESP8266_SendAT("AT+CWJAP=\"TP-LINK_ADEB\",\"19979303323\"", "WIFI GOT IP", 15000) == 1) 
	{
        printf("→ WiFi 连接成功！已获取 IP\r\n");
    } 
	else 
	{
        printf("→ WiFi 连接失败！请检查 ssid / 密码 / 信号\r\n");
        
    }

    // MQTT 配置（用你的永久 token）
    char cmd[512];
	printf("5. 配置 MQTT USER ...\r\n");
	sprintf(cmd, "AT+MQTTUSERCFG=0,1,\"dev0\",\"ME3239j3An\",\"%s\",0,0,\"\"", token);
	printf("发送: %s\r\n", cmd);
	
    if (ESP8266_SendAT(cmd, "OK", 2000) == 1) 
	{
        printf("→ MQTTUSERCFG 配置成功\r\n");
    } 
	else 
	{
        printf("→ MQTTUSERCFG 失败！请检查 token 是否过期/格式\r\n");
        
    }

    // 连接 MQTT 服务器
    printf("6. 正在连接 thingsboard 云端 mqtts.heclouds.com:1883 ...\r\n");
    if (ESP8266_SendAT("AT+MQTTCONN=0,\"mqtts.heclouds.com\",1883,1", "+MQTTCONNECTED", 10000) == 1) 
	{
        printf("→ MQTT 连接成功！\r\n");
    } 
	else 
	{
        printf("→ MQTT 连接失败！请检查网络 / token / 服务器地址\r\n");
        
    }
	// 订阅属性上报的回复主题
    printf("7. 订阅属性上报回复主题...\r\n");
    if (ESP8266_SendAT("AT+MQTTSUB=0,\"$sys/ME3239j3An/dev0/thing/property/post/reply\",1", "OK", 2000) == 1)
	{
        printf("→ 订阅成功！可以开始上报数据了\r\n");
    } 
	else 
	{
        printf("→ 订阅失败！\r\n");
    }
	
	printf("===== ESP8266 初始化完成 =====\r\n\r\n");
    
}
