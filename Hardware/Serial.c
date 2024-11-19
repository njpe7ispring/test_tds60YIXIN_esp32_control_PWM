#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <Serial.h>
#include <Delay.h>
uint16_t Serial_TxPacket[8];   //外部使用，用extern，或者用GET函数，指针传递；即可在其他的文件中使用
uint16_t Serial_RxPacket[11];
SensorData Sdata = {0, 0,0.0};
//uint8_t RxData;		//定义串口接收的数据变量
uint8_t Serial_RxFlag;		//定义串口接收的标志位变量
/**
  * 函    数：串口初始化
  * 参    数：无
  * 返 回 值：无
  */
void Serial_Init(void)
{
	/*开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	//开启USART1的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//开启GPIOA的时钟
	
	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);					//将PA9引脚初始化为复用推挽输出
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);					//将PA10引脚初始化为上拉输入
	
	/*USART初始化*/
	USART_InitTypeDef USART_InitStructure;					//定义结构体变量
	USART_InitStructure.USART_BaudRate = 9600;				//波特率
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//硬件流控制，不需要
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;	//模式，发送模式和接收模式均选择
	USART_InitStructure.USART_Parity = USART_Parity_No;		//奇偶校验，不需要
	USART_InitStructure.USART_StopBits = USART_StopBits_1;	//停止位，选择1位
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//字长，选择8位
	USART_Init(USART1, &USART_InitStructure);				//将结构体变量交给USART_Init，配置USART1
	
	/*中断输出配置*/
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);			//开启串口接收数据的中断
	
	/*NVIC中断分组*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);			//配置NVIC为分组2
	
	/*NVIC配置*/
	NVIC_InitTypeDef NVIC_InitStructure;					//定义结构体变量
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;		//选择配置NVIC的USART1线
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//指定NVIC线路使能
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;		//指定NVIC线路的抢占优先级为1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//指定NVIC线路的响应优先级为1
	NVIC_Init(&NVIC_InitStructure);							//将结构体变量交给NVIC_Init，配置NVIC外设
	
	/*USART使能*/
	USART_Cmd(USART1, ENABLE);								//使能USART1，串口开始运行
}

/**
  * 函    数：串口发送一个字节
  * 参    数：Byte 要发送的一个字节
  * 返 回 值：无
  */
void Serial_SendByte(uint8_t Byte)
{
	USART_SendData(USART1, Byte);		//将字节数据写入数据寄存器，写入后USART自动生成时序波形
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);	//等待发送完成
	/*下次写入数据寄存器会自动清除发送完成标志位，故此循环后，无需清除标志位*/
}

/**
  * 函    数：串口发送一个数组
  * 参    数：Array 要发送数组的首地址
  * 参    数：Length 要发送数组的长度
  * 返 回 值：无
  */
void Serial_SendArray(uint16_t *Array, uint16_t Length)
{
	uint16_t i;
	for (i = 0; i < Length; i ++)		//遍历数组
	{
		Serial_SendByte(Array[i]);		//依次调用Serial_SendByte发送每个字节数据
	}
}

/**
  * 函    数：串口发送一个字符串
  * 参    数：String 要发送字符串的首地址
  * 返 回 值：无
  */
void Serial_SendString(char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i ++)//遍历字符数组（字符串），遇到字符串结束标志位后停止
	{
		Serial_SendByte(String[i]);		//依次调用Serial_SendByte发送每个字节数据
	}
}

/**
  * 函    数：次方函数（内部使用）
  * 返 回 值：返回值等于X的Y次方
  */
uint32_t Serial_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;	//设置结果初值为1
	while (Y --)			//执行Y次
	{
		Result *= X;		//将X累乘到结果
	}
	return Result;
}

/**
  * 函    数：串口发送数字
  * 参    数：Number 要发送的数字，范围：0~4294967295
  * 参    数：Length 要发送数字的长度，范围：0~10
  * 返 回 值：无
  */
void Serial_SendNumber(uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i ++)		//根据数字长度遍历数字的每一位
	{
		Serial_SendByte(Number / Serial_Pow(10, Length - i - 1) % 10 + '0');	//依次调用Serial_SendByte发送每位数字
	}
}

/**
  * 函    数：使用printf需要重定向的底层函数
  * 参    数：保持原始格式即可，无需变动
  * 返 回 值：保持原始格式即可，无需变动
  */
int fputc(int ch, FILE *f)
{
	Serial_SendByte(ch);			//将printf的底层重定向到自己的发送字节函数
	return ch;
}

/**
  * 函    数：自己封装的prinf函数
  * 参    数：format 格式化字符串
  * 参    数：... 可变的参数列表
  * 返 回 值：无
  */
void Serial_Printf(char *format, ...)
{
	char String[100];				//定义字符数组
	va_list arg;					//定义可变参数列表数据类型的变量arg
	va_start(arg, format);			//从format开始，接收参数列表到arg变量
	vsprintf(String, format, arg);	//使用vsprintf打印格式化字符串和参数列表到字符数组中
	va_end(arg);					//结束变量arg
	Serial_SendString(String);		//串口发送字符数组（字符串）
}

void Serial_SendPacket(void)
{
	//Serial_SendByte(0xFF);
	Serial_SendArray(Serial_TxPacket, 8);
	//Serial_SendByte(0xFE);
	
}
/**
  * 函    数：获取串口接收标志位
  * 参    数：无
  * 返 回 值：串口接收标志位，范围：0~1，接收到数据后，标志位置1，读取后标志位自动清零
  */
uint8_t Serial_GetRxFlag(void)
{
	if (Serial_RxFlag == 1)			//如果标志位为1
	{
		Serial_RxFlag = 0;
		return 1;					//则返回1，并自动清零标志位
	}
	return 0;						//如果标志位为0，则返回0
}


/**
  * 函    数：BA111校准函数
  * 参    数：无
  * 返 回 值：无
  */
//void BA111_calibration(void)
//{

//	BA111_Calibration(); 
//	BA111_Check_callback();
//	Delay_ms(1000);     //注意此款芯片命令发送间隔为1s，所以最好延迟一秒，放置芯片无法处理命令
//	BA111_Set_res(); 
//	BA111_Check_callback();	
//	Delay_ms(1000);
//	BA111_Set_B_NTC();
//	BA111_Check_callback();
//	Delay_ms(1000);
//}
//void BA111_Getsensordata()
//{
//	char *Detection_data = "A000000000A0"; 
//	BA111_sendHexData(Detection_data);
//	Sdata = BA111_ParseData(); // 更新全局的dataBA111_ParseData();
//}
//SensorData BA111_ParseData(void) {
//    if (Serial_GetRxFlag()) {
//            if (Serial_RxPacket[0] == 0xAA) {
//                int tempAdc = (Serial_RxPacket[3] << 8) | Serial_RxPacket[4];
//               
//                uint8_t checksum = Serial_RxPacket[5];
//                uint8_t sum = 0;

//                for (uint8_t i = 0; i < 5; i++) {
//                    sum += Serial_RxPacket[i];
//                }
//                sum = sum & 0xFF;

//                if (sum == checksum) {
//					 Sdata.temp = tempAdc / 100.0;
//					Sdata.tds = (Serial_RxPacket[1] << 8) | Serial_RxPacket[2];
//                   // printf("TDS: %d ppm   Temp: %.2f °C\r\n", Sdata.tds, Sdata.temp);
//                }
//            }
//    }
//    return Sdata;
//}




///**
//  * 函    数：BA111基线校准
//  * 参    数：无
//  * 返 回 值：无
//  */
//void BA111_Calibration(void)
//{
//    char *Calibration_data = "A600000000A6";
//    BA111_sendHexData(Calibration_data);
//}
///**
//  * 函    数：BA111校准信息发送后回调函数
//  * 参    数：无
//  * 返 回 值：无
//  */
//void BA111_Check_callback(void)
//{
//	while(1)
//	{
//		if(BA111_Check_DataReceived())
//		{
//		//printf("设置成功 ");
//		break;
//	}
//	}
//}

///**
//  * 函    数：检查数据接收状态
//  * 参    数：无
//  * 返 回 值：true表示接收到预期格式的数据，false表示未接收到或接收的数据不符合预期
//  */


//bool BA111_Check_DataReceived(void)
//{
//    // 检查是否有数据接收完成
//    if (Serial_GetRxFlag()) 
//    {
//        // 确认有数据接收完成，读取接收缓冲区的6字节数据
////        for (int i = 0; i < 6; i++) 
////        {
////			//Serial_RxPacket[pRxPacket]
////			//printf("Serial_RxPacket[i]:%d",Serial_RxPacket[i]);
////			//串口将收到的数据回传回去，用于测试
////        }
////        
//        // 检查数据包的格式
//        if (Serial_RxPacket[0] == 0xAC && 
//            Serial_RxPacket[1] == 0x00 &&
//            Serial_RxPacket[2] == 0x00 &&
//            Serial_RxPacket[3] == 0x00 &&  
//            Serial_RxPacket[4] == 0x00 &&
//            Serial_RxPacket[5] == 0xAC) 
//        {
//			//Serial_DrvFlag = 1;
//            return true; // 数据包格式正确
//        } 
//        else if (Serial_RxPacket[0] == 0xAC) 
//        {
//            // 根据Serial_RxPacket[1]的值判断错误类型
//            switch (Serial_RxPacket[1]) 
//            {
//                case 0x01:
//                    // Serial_Printf("BA111命令帧异常\r\n");
//                    break;
//                case 0x02:
//                    // Serial_Printf("BA111设备忙\r\n");
//                    break;
//                case 0x03:
//                    // Serial_Printf("BA111校准失败\r\n");
//                    break;
//                case 0x04:
//                    // Serial_Printf("BA111检测温度超出范围\r\n");
//                    break;
//                default:
//                    // Serial_Printf("未知失败\r\n");
//                    break;
//            }
//            return false; // 数据包格式错误或错误类型
//        }
//    }
//    return false; // 没有接收到数据
//}
//下面是错误的处理
//void BA111_sendHexData(char *hexString)
//{
//    uint8_t i = 0;
//    while(hexString[i] != '\0') // 遍历字符串直到结束符
//    {
//        uint8_t hexValue = 0;
//        if(hexString[i] >= '0' && hexString[i] <= '9')
//        {
//            hexValue = hexString[i] - '0'; // 将字符'0'-'9'转换为数值0-9
//        }
//        else if(hexString[i] >= 'A' && hexString[i] <= 'F')
//        {
//            hexValue = hexString[i] - 'A' + 10; // 将字符'A'-'F'转换为数值10-15
//        }
//        else if(hexString[i] >= 'a' && hexString[i] <= 'f')
//        {
//            hexValue = hexString[i] - 'a' + 10; // 将字符'a'-'f'转换为数值10-15
//        }
//        Serial_SendByte(hexValue); // 发送转换后的字节
//        i++;
//    }
//}

void sendHexData(char *hexString) {
    int hexStringLength = strlen(hexString);
    if (hexStringLength % 2 != 0) {
        // 长度为奇数，补零
        char temp[hexStringLength + 1];
        strncpy(temp, hexString, hexStringLength);
        temp[hexStringLength] = '0';
        hexString = temp;
        hexStringLength++;
    }

    uint8_t i = 0;
    while (i < hexStringLength) {
        uint8_t hexValueHigh = 0;
        uint8_t hexValueLow = 0;
        if (hexString[i] >= '0' && hexString[i] <= '9') {
            hexValueHigh = hexString[i] - '0';
        } else if (hexString[i] >= 'A' && hexString[i] <= 'F') {
            hexValueHigh = hexString[i] - 'A' + 10;
        } else if (hexString[i] >= 'a' && hexString[i] <= 'f') {
            hexValueHigh = hexString[i] - 'a' + 10;
        }
        i++;

        if (i < hexStringLength) {
            if (hexString[i] >= '0' && hexString[i] <= '9') {
                hexValueLow = hexString[i] - '0';
            } else if (hexString[i] >= 'A' && hexString[i] <= 'F') {
                hexValueLow = hexString[i] - 'A' + 10;
            } else if (hexString[i] >= 'a' && hexString[i] <= 'f') {
                hexValueLow = hexString[i] - 'a' + 10;
            }
            i++;

            uint8_t byte = (hexValueHigh << 4) | hexValueLow;
            Serial_SendByte(byte);
        }
    }
}

//铱鑫电子获取数据命令
void YIXIN_SendCommand()
{
	char *cmd = "5507050100000062";
	sendHexData(cmd);
}


SensorData YIXIN_ParseData(void) {
    if (Serial_GetRxFlag()) {
            if (Serial_RxPacket[0] == 0x55 && Serial_RxPacket[1] == 0x0a && Serial_RxPacket[2] == 0x85 && Serial_RxPacket[3] == 0x01){
                int tempAdc = (Serial_RxPacket[6] << 8) | Serial_RxPacket[7];
                int condAdc = (Serial_RxPacket[4] << 8) | Serial_RxPacket[5];
                uint8_t checksum = Serial_RxPacket[10];
                uint8_t sum = 0;

                for (uint8_t i = 0; i < 10; i++) {
                    sum += Serial_RxPacket[i];
                } 
                sum = sum & 0xFF;

                if (sum == checksum) {
					 Sdata.temp = tempAdc / 10.0;
					Sdata.cond = condAdc /10;
					Sdata.tds = Sdata.cond /2;
                   // printf("TDS: %d ppm   Temp: %.2f °C\r\n", Sdata.tds, Sdata.temp);
                }
            }
    }
    return Sdata;
}

///**
//  * 函    数：USART1中断函数
//  * 参    数：无
//  * 返 回 值：无
//  * 注意事项：此函数为中断函数，无需调用，中断触发后自动执行
//  *           函数名为预留的指定名称，可以从启动文件复制
//  *           请确保函数名正确，不能有任何差异，否则中断函数将不能进入
//  */
void USART1_IRQHandler(void)
{
	//static uint8_t RxState = 0;//初始化一次只能 同全局变量，在生命周期一直有效；但是静态变量只能够在本函数使用
	static uint8_t pRxPacket = 0;//指示数据吧到哪一位

	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)		//判断是否是USART1的接收事件触发的中断
	{
		uint8_t RxData = USART_ReceiveData(USART1);	
			Serial_RxPacket[pRxPacket] = RxData;
			pRxPacket ++;
			if (pRxPacket >= 11)                    //接收数据11个字节
			{
				Serial_RxFlag = 1;	
				pRxPacket = 0;
			}
		}
	
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);			//清除USART1的RXNE标志位
																//读取数据寄存器会自动清除此标志位
																//如果已经读取了数据寄存器，也可以不执行此代码
}

