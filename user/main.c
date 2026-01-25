#include "HardwareManage.h"
#include "config.h"
#include <stdlib.h>
#include "app_oled.h"
#include "app_led.h"
#include "app_mq135.h"
#include "math.h"
#include "DHT11_init.h"
#include "esp_uart.h"
#include "BSP_uart.h"

DHT11_Data_TypeDef  DHT11_Data;

int main (void)
{
		
		//Hardware_Init();
		Systick_Init(72);
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
		ESP_Download_GPIO_Config();
		ESP_Enter_Download_Mode();
		
	
		//Screen_Init();
		while(1)
		{  
			
			//float a = APP_MQ135();
			//printf("氨气 = %f\r\n",a);
//			if(DHT11_Read_TempAndHumidity(&DHT11_Data) == SUCCESS)
//			{	
//				
//				float humi = DHT11_Data.humi_int+DHT11_Data.humi_deci;
//				float temp = DHT11_Data.temp_int+DHT11_Data.temp_deci;
//				printf("humi湿度是 = %f\r\n",humi);
//				printf("temp温度是 = %f\r\n",temp);
//			}
//			delay_nms(200);
			
			
		}
	
}






