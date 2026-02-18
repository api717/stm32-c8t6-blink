#include "app_esp8266.h"
#include "esp8266_uart_init.h"
#include "systick.h"
#include <stdio.h>
#include <string.h>

/*接收缓冲区*/
volatile static char esp_rx_buf[SIZE];
volatile uint16_t esp_rx_w = 0;
volatile uint16_t esp_rx_r = 0;

/*AT引擎内部状态*/
static uint8_t at_index = 0;   //当前执行第几条 	
static uint8_t at_waiting = 0; //当前是在“发完等待”还是“还没发” 	
static uint32_t at_tick = 0;   //什么时候开始等

ESP_InitState_t esp_state = ESP_INIT_RUN_TABLE; //当前的状态

static uint8_t  try_software_reset_count = 0;   //超时后的重连次数
static uint32_t reset_start_tick = 0;

/*AT指令表*/
AT_CMD esp_init_sta_table[] =
{
    {"AT", "OK", 1000},    

    {"AT+RST", "OK", 2000},
	//禁用自动连接
    {"AT+CWAUTOCONN=0", "OK", 1000},

    {"AT+CWMODE=1", "OK", 1000},

    {"AT+CWJAP=\"TP-LINK_ADEB\",\"19979303323\"", "WIFI GOT IP", 20000},

    {"AT+MQTTUSERCFG=0,1,\"dev0\",\"ME3239j3An\",\"version=2018-10-31&res=products%2FME3239j3An%2Fdevices%2Fdev0&et=4102444800&method=md5&sign=ApKkPfCguhXM8Qq4zbxv1A%3D%3D\",0,0,\"\"", "OK", 3000},

    {"AT+MQTTCONN=0,\"mqtts.heclouds.com\",1883,1", "+MQTTCONNECTED", 10000},
};
/**
 * @brief USART2中断接收函数 
 * @param  无
 * @retval 无
 */
void USART2_IRQHandler(void)
{
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        uint8_t data = USART_ReceiveData(USART2);

        uint16_t next = (esp_rx_w + 1) % SIZE;

        if(next != esp_rx_r)
        {
            esp_rx_buf[esp_rx_w] = data;
            esp_rx_w = next;
        }
        
    }
}


void USART_SendString(USART_TypeDef* USARTx, const char *str)
{
    char buf[SIZE];
    sprintf(buf, "%s\r\n", str);
    for(uint16_t i = 0; i < strlen(buf); i++)
    {
        USART_SendData(USARTx, buf[i]);
        while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
    }	
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
}

/**
 * @brief 在缓冲区中查找字符串
 * @param  const char *str 期待回应的字符串
 * @retval SUCCESS成功匹配 FAIL没有成功
 */
uint8_t ESP_HasString(const char *str)
{
    char linear_buf[SIZE];
    uint16_t len = 0;
    
    //关中断，避免查找时缓冲区被修改
    __disable_irq();
    uint16_t w = esp_rx_w;
    uint16_t r = esp_rx_r;
    __enable_irq();
    
    //复制环形缓冲区到线性缓冲区
    while(r != w && len < SIZE - 1)
    {
        linear_buf[len++] = esp_rx_buf[r];
        r = (r + 1) % SIZE;
    }
    linear_buf[len] = '\0';
    
	return strstr(linear_buf, str) ? SUCCESS : FAIL;
}


/**
 * @brief 清空接收缓冲
 * @param  无
 * @retval 无
 */
void ESP_ClearBuf(void)
{
 
    __disable_irq();
    esp_rx_r = 0;
    esp_rx_w = 0;
    __enable_irq();
    
    memset((void*)esp_rx_buf, 0, SIZE);     
}
  
uint8_t AT_Engine_Run(AT_CMD *table, uint8_t size)
{
	
	static uint32_t rst_delay_tick = 0;
    static uint8_t waiting_rst = 0;
	
    if(at_index >= size)
        return FINISH;

    AT_CMD *cur = &table[at_index];
    
    
	/* 等待 RST 后的稳定期 */
    if(waiting_rst)
    {
        if(millis() - rst_delay_tick >= 5000)  // 等 5 秒
        {
            waiting_rst = 0;
            ESP_ClearBuf();  // 清空乱码
            at_index++;      // 进入下一条
            at_waiting = 0;
        }
        return RUNNING;
    }	
	
	/* 发送当前指令 */
    if(at_waiting == 0)
    {
       
        //只在第一条指令时清空
        if(at_index == 0)
            ESP_ClearBuf();
     
        
        USART_SendString(USART2, cur->cmd);
        
        at_tick = millis();
        at_waiting = 1;
        
        return RUNNING;
    }

    // 已发送，正在等回复
    if(ESP_HasString(cur->resp))
    {
		printf("收到回复: %s\r\n", cur->resp);
        
        // 如果是 AT+RST 且收到 OK，进入延迟等待
        if(strstr(cur->cmd, "AT+RST") && strstr(cur->resp, "OK"))
        {
            printf("等待 ESP8266 重启...\r\n");
            rst_delay_tick = millis();
            waiting_rst = 1;
            at_waiting = 0;
            return RUNNING;
        }
        
        ESP_ClearBuf();
        at_index++;
        at_waiting = 0;
        return RUNNING;
    }

    // 超时判断
    if(millis() - at_tick > cur->timeout)
    {
        printf("超时! 期待: %s\r\n", cur->resp);
        at_index = 0;
        at_waiting = 0;
        waiting_rst = 0;
        return TIMEOUT;
    }

    return RUNNING;
}

/**
 * @brief  状态机连接服务器
 * @param  无
 * @retval 无
 */
void Application_Init(void)
{
	uint8_t ret;
	static uint32_t last_debug = 0;
	
	switch(esp_state)
    {
        case ESP_INIT_RUN_TABLE:
			
		    // 每秒打印一次当前状态
            if(millis() - last_debug > 1000)
            {
                last_debug = millis();
                printf("当前AT[%d]: %s, 等待: %s\r\n", 
                       at_index, 
                       esp_init_sta_table[at_index].cmd,
                       esp_init_sta_table[at_index].resp);
                
                // 打印缓冲区内容
                char buf[256];
                uint16_t len = 0;
                uint16_t r = esp_rx_r, w = esp_rx_w;
                
                while(r != w && len < 255)
                {
                    buf[len++] = esp_rx_buf[r];
                    r = (r + 1) % SIZE;
                }
                buf[len] = '\0';
                printf("缓冲区: [%s]\r\n", buf);
            }
			
			ret = AT_Engine_Run(esp_init_sta_table, sizeof(esp_init_sta_table)/sizeof(AT_CMD));
			
            if(ret == FINISH)
            {
                printf("ESP 初始化完成\r\n");
                esp_state = ESP_INIT_SUCCESS;
				try_software_reset_count = 0;  //成功后清零计数
            }
			else if(ret == TIMEOUT)
            {
                printf("AT 超时，尝试恢复...\r\n");
                if(try_software_reset_count < 3)
                {
                    printf("第 %d 次软件复位\r\n", try_software_reset_count + 1);
                    USART_SendString(USART2, "AT+RST\r\n");
                    reset_start_tick = millis();
                    esp_state = ESP_INIT_WAIT_STABLE;   // 进入等待状态
                    try_software_reset_count++;
                }
                else
                {
                    printf("软件复位 3 次失败，进行硬件复位\r\n");
                    ESP8266_Reset();
                    
                    try_software_reset_count = 0;
                }
				reset_start_tick = millis();
                esp_state = ESP_INIT_WAIT_STABLE;
            }
            break;

        case ESP_INIT_SUCCESS:
            break;

        case ESP_INIT_WAIT_STABLE:
			
			if(millis() - reset_start_tick >= 5000)
            {
                ESP_ClearBuf();
                at_index = 0;
                at_waiting = 0;
                esp_state = ESP_INIT_RUN_TABLE;  // 重新开始跑表
            }
            break;
        
    }
    
}

/**
 * @brief  发送 MQTT 上报数据（示例）
 * @param  无
 * @retval 无
 */
void ESP8266_PublishData(void)
{
    char json[SIZE];
    sprintf(json, "{\"id\":\"717\",\"version\":\"1.0\",\"params\":{\"CurrentTemperature\":{\"value\":25.5},\"RelativeHumidity\":{\"value\":68.0},\"NH3Concentration\":{\"value\":450.0}}}");
	
}



