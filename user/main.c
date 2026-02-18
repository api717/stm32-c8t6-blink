#include "HardwareManage.h"  
#include "config.h"
#include <stdlib.h>
#include "app_oled.h"
#include "app_led.h"
#include "app_mq135.h"
#include "app_dht11.h"
#include "app_buzzer.h"
#include "app_esp8266.h"
#include <string.h>
#include <stdio.h>




int main (void)
{
		
		Hardware_Init();

		while(1)
		{    
			ESP_Process();
			Application_Init();
		}
	
}






