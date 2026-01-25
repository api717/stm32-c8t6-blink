#include "app_oled.h"
#include "systick.h"
#include "bsp_oled_debug.h"

void Screen_Init(void)
{
		OLED_CLS();  	 //全屏清0（灭）
	
		delay_nms(100);  //稍等稳定	
		OLED_ShowCN24x24(0, 1, 0);  
		for (uint8_t i = 0; i < 4; i++)
		{
			OLED_ShowCN(30 + i* 16, 1, i); //脐橙仓储
		}
		OLED_ShowCN(0,5,7);
		OLED_ShowCN(16,5,8);

}
