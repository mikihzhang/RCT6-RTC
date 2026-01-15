#ifndef UDP_H_
#define UDP_H_

#include "string.h"
#include "eth_driver.h"

extern u8 MACAddr[6];
extern u8 IPAddr[4];
extern u8 GWIPAddr[4];
extern u8 IPMask[4];
extern u8 DESIP[4];

// Socket IDs
extern u8 SocketId_UDP;   // 用于发送数据的UDP Socket
extern u8 SocketId_MQTT;  // 用于连接Broker的TCP Socket

extern u8 MQTT_Conn_Flag; // TCP连接状态标志

void mStopIfError(u8 iError);
void TIM2_Init(void);
void WCHNET_CreateUdpSocket(void);
void WCHNET_CreateMqttSocket(void); // 新增：创建TCP Socket
void WCHNET_HandleSockInt(u8 socketid, u8 intstat);
void WCHNET_HandleGlobalInt(void);
void WCHNET_UDP_Recv(u8 id, u8 *buf, u32 len, u8 *addr, u16 port);
// 新增：发送UDP数据到动态IP的函数
void UDP_SendTo_DynamicIP(u8 mb_id, char *data, u32 len);
void WCHNET_CreateUDP(void);

#endif