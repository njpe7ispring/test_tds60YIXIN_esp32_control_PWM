#ifndef __SERIAL_H
#define __SERIAL_H

#include <stdio.h>
typedef enum { false, true } bool;
typedef struct {
    uint16_t tds;
	uint16_t cond;
    float temp;
} SensorData;
extern SensorData Sdata;
extern SensorData S2data;
void Serial_Init(void);
void Serial2_Init(void);
void Serial_SendByte(uint8_t Byte);
void Serial2_SendByte(uint8_t Byte);
void Serial_SendArray(uint16_t *Array, uint16_t Length);
void Serial2_SendArray(uint16_t *Array, uint16_t Length);
void Serial_SendString(char *String);
void Serial2_SendString(char *String);
void Serial_SendNumber(uint32_t Number, uint8_t Length);
void Serial2_SendNumber(uint32_t Number, uint8_t Length);
void Serial_Printf(char *format, ...);
void Serial2_Printf(char *format, ...);

SensorData YIXIN_ParseData(void);
SensorData YIXIN2_ParseData(void);
void YIXIN_SendCommand(void);
void YIXIN2_SendCommand(void);
void sendHexData(char *hexString);
void sendHexData2(char *hexString);
extern uint16_t Serial_TxPacket[8];   //外部使用，用extern，或者用GET函数，指针传递；即可在其他的文件中使用
extern uint16_t Serial_RxPacket[11];
extern uint16_t Serial2_TxPacket[8];   //外部使用，用extern，或者用GET函数，指针传递；即可在其他的文件中使用
extern uint16_t Serial2_RxPacket[11];
uint8_t Serial_GetRxFlag(void);
uint8_t Serial2_GetRxFlag(void);
//uint8_t Serial_GetRxData(void);
void Serial_SendPacket(void);
#endif
