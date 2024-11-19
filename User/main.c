#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Serial.h"
#include "Encoder.h"
#include "Motor.h"

uint8_t RxData;	 		//定义用于接收串口数据的变量
uint8_t KeyNum;		//定义用于接收按键键码的变量
int8_t Speed;		//定义速度变量

int8_t Num;			//定义待被旋转编码器调节的变量
int main(void)
{
	/*模块初始化*/
	OLED_Init();		//OLED初始化
	/*显示静态字符串*/
	Motor_Init();		//直流电机初始化
	Encoder_Init();		//旋转编码器初始化   每次改变1%
	/*串口初始化*/
	Serial_Init();		//串口初始化
	/*显示静态字符串*/
	OLED_ShowString(1, 1, "Speed:");		//1行1列显示字符串Speed:
	/*显示静态字符串*/
	OLED_ShowString(2, 1, "Num:");			//2行1列显示字符串Num:      Num控制在（-100-100）因为速度有上限低于或者高于极限都不在变动
	OLED_ShowString(3, 1, "YIXINTDS:");
	OLED_ShowString(4, 1, "TEM:");
	Motor_SetSpeed(100);  //电机先全速启动
	Delay_ms(1000);
	Num = 100;
	while (1)                                                                                                                     
	{
		Num += Encoder_Get();				//获取自上此调用此函数后，旋转编码器的增量值，并将增量值加到Num上
		OLED_ShowSignedNum(2, 5, Num, 5);	//显示Num
		Speed = Num;
			if (Speed < -100)					//按键1按下
		{
			Speed = -100;				//速度变量变为-100为极限
		}
		if (Speed > 100)					//按键1按下
		{
			Speed = 100;				//速度变量变为-100为极限
		}
		Motor_SetSpeed(Speed);				//设置直流电机的速度为速度变量
		OLED_ShowSignedNum(1, 7, Speed, 3);	//OLED显示速度变量
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
		OLED_ShowNum(3, 10, Sdata.tds, 4);
		OLED_ShowNum(4, 5, Sdata.temp, 3);
		Delay_ms(100);  // 为显示更新留出时间
		
		
	}
}
