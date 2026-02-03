#ifndef __APP_ESP8266_H
#define __APP_ESP8266_H

#include "config.h"


#define SIZE 1024
  
//发送 AT 指令并等待 OK 返回（阻塞式）
uint8_t ESP8266_SendAT(const char* cmd, const char* resp, uint32_t timeout_ms);

//发送数据（不带 \r\n）
void ESP8266_SendData(const char* data, uint16_t len);
void ESP8266_PublishData(void);
void Application_Init(void);
//接收缓冲区（简单环形缓冲或 DMA 方式更佳，这里先用简单方式）
extern char esp_rx_buf[SIZE];
extern uint16_t esp_rx_len;

#endif
