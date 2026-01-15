#ifndef UDP_H_
#define UDP_H_

#include "string.h"
#include "eth_driver.h"

extern u8 MACAddr[6];
extern u8 IPAddr[4];
extern u8 GWIPAddr[4];
extern u8 IPMask[4];

// Socket IDs
extern u8 SocketId_UDP;   // 用于发送数据的UDP Socket
extern u8 SocketId_MQTT;  // MQTT Broker TCP Socket

extern u8 MQTT_Conn_Flag; // TCP连接状态标志

void mStopIfError(u8 iError);
void TIM2_Init(void);
void WCHNET_CreateUdpSocket(void);
void WCHNET_CreateMqttSocket(void); // MQTT TCP Socket
void WCHNET_HandleSockInt(u8 socketid, u8 intstat);
void WCHNET_HandleGlobalInt(void);
void UDP_SendTo_DynamicIP(u8 mb_id, const char *data, u32 len);

#endif
