#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Serial.h"


uint8_t RxData;	 		//定义用于接收串口数据的变量

int main(void)
{
	/*模块初始化*/
	OLED_Init();		//OLED初始化
	/*显示静态字符串*/
	//OLED_ShowString(1, 1, "HaoShui:");
	OLED_ShowString(1, 1, "YIXINTDS:");
	OLED_ShowString(2, 1, "TEM:");
	/*串口初始化*/
	Serial_Init();		//串口初始化
	//我要一个个用屏幕发数据进行验证
	//BA111_calibration();
	
	while (1)                                                                                                                     
	{
		YIXIN_SendCommand();
		Delay_ms(1000);
//		OLED_ShowHexNum(3, 2, Serial_RxPacket[0], 2);
//		OLED_ShowHexNum(3, 4, Serial_RxPacket[1], 2);	//显示串口接收的数据
//		OLED_ShowHexNum(3, 6, Serial_RxPacket[2], 2);
//		OLED_ShowHexNum(3, 8, Serial_RxPacket[3], 2);
//		OLED_ShowHexNum(3, 10,Serial_RxPacket[4], 2);
//		OLED_ShowHexNum(3, 12,Serial_RxPacket[5], 2);
//		OLED_ShowHexNum(3, 14,Serial_RxPacket[6], 2);
//		OLED_ShowHexNum(4, 1,Serial_RxPacket[7], 2);
//		OLED_ShowHexNum(4, 3,Serial_RxPacket[8], 2);
//		OLED_ShowHexNum(4, 5,Serial_RxPacket[9], 2);
//		OLED_ShowHexNum(4, 7,Serial_RxPacket[10], 2);
		
		Sdata = YIXIN_ParseData();
		OLED_ShowNum(1, 10, Sdata.tds, 4);
		OLED_ShowNum(2, 5, Sdata.temp, 3);
		Delay_ms(100);  // 为显示更新留出时间
		
		
	}
}
