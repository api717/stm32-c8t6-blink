#ifndef __APP_ESP8266_H
#define __APP_ESP8266_H

#include "config.h"

#define SIZE 1024
#define MQTT_CLIENT_ID      "dev0"//一定要加""代表是字符串
#define MQTT_USERNAME       "ME3239j3An"
#define MQTT_BROKER         "mqtts.heclouds.com"
//接收缓冲
#define SUCCESS 1
#define FAIL    0
//AT驱动函数返回状态
#define RUNNING 0
#define FINISH  1
#define TIMEOUT 2
/*AT 表结构*/
typedef struct
{
    char *cmd; 		 //命令
    char *resp; 	 //期待回复
    uint32_t timeout;//延迟

}AT_CMD;

typedef enum 
{
    ESP_INIT_RUN_TABLE = 0,   
    ESP_INIT_SUCCESS,      
    ESP_INIT_WAIT_STABLE 
	
} ESP_InitState_t;




//发送 AT 指令并等待 OK 返回（阻塞式）
void USART_SendString(USART_TypeDef* USARTx, const char *str);
void ESP_Process(void);
uint8_t ESP_HasString(const char *str);	
void ESP_ClearBuf(void);
uint8_t AT_Engine_Run(AT_CMD *table, uint8_t size);
void Application_Init(void);
void ESP8266_PublishData(void);


#endif
