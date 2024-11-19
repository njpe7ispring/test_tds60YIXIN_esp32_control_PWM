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
void Serial_Init(void);
void Serial_SendByte(uint8_t Byte);
void Serial_SendArray(uint16_t *Array, uint16_t Length);
void Serial_SendString(char *String);
void Serial_SendNumber(uint32_t Number, uint8_t Length);
void Serial_Printf(char *format, ...);

SensorData YIXIN_ParseData(void);
void YIXIN_SendCommand(void);
void sendHexData(char *hexString);

extern uint16_t Serial_TxPacket[8];   //�ⲿʹ�ã���extern��������GET������ָ�봫�ݣ��������������ļ���ʹ��
extern uint16_t Serial_RxPacket[11];
uint8_t Serial_GetRxFlag(void);
//uint8_t Serial_GetRxData(void);
void Serial_SendPacket(void);
#endif
