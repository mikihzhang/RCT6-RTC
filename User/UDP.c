#include "UDP.h"
#include "stdio.h"
#include "wchnet.h"
#include "debug.h"

#define UDP_RECE_BUF_LEN  1472

/* Global Variables */
u8 SocketId;

u8 MACAddr[6];
u8 IPAddr[4] = { 10, 1, 3, 100 };         // 本机IP (示例)
u8 GWIPAddr[4] = { 10, 1, 0, 254 };       // 网关/NanoPi IP
u8 IPMask[4] = { 255, 255, 248, 0 };

// MQTT Broker IP (通常与 NanoPi IP 一致)
u8 MQTT_BROKER_IP[4] = { 10, 1, 0, 254 };
u16 MQTT_BROKER_PORT = 1883;

u8 SocketId_UDP = 0xFF;
u8 SocketId_MQTT = 0xFF;
u8 MQTT_Conn_Flag = 0; // 0:断开, 1:已连接TCP

u8 SocketRecvBuf[WCHNET_MAX_SOCKET_NUM][UDP_RECE_BUF_LEN];

// MQTT 接收缓冲
extern u8 MqttRxBuffer[2048];
extern volatile u16 MqttRxLen;

void mStopIfError(u8 iError)
{
    if (iError == WCHNET_ERR_SUCCESS) return;
    printf("Error: %02X\r\n", (u16) iError);
}

// UDP Socket (用于发送)
void WCHNET_CreateUdpSocket(void)
{
    u8 i;
    SOCK_INF TmpSocketInf;
    memset((void *) &TmpSocketInf, 0, sizeof(SOCK_INF));

    TmpSocketInf.DesPort = 1000;
    TmpSocketInf.SourPort = 2000;
    TmpSocketInf.ProtoType = PROTO_TYPE_UDP;
    TmpSocketInf.RecvStartPoint = (u32) SocketRecvBuf[0];
    TmpSocketInf.RecvBufLen = UDP_RECE_BUF_LEN;

    i = WCHNET_SocketCreat(&SocketId_UDP, &TmpSocketInf);
    printf("UDP Socket Created: %d\r\n", SocketId_UDP);
    mStopIfError(i);
}

// MQTT TCP Socket
void WCHNET_CreateMqttSocket(void)
{
    u8 i;
    SOCK_INF TmpSocketInf;
    memset((void *) &TmpSocketInf, 0, sizeof(SOCK_INF));

    memcpy((void *) TmpSocketInf.IPAddr, MQTT_BROKER_IP, 4);
    TmpSocketInf.DesPort = MQTT_BROKER_PORT;
    TmpSocketInf.SourPort = 3000;
    TmpSocketInf.ProtoType = PROTO_TYPE_TCP;
    TmpSocketInf.RecvStartPoint = (u32) SocketRecvBuf[1];
    TmpSocketInf.RecvBufLen = UDP_RECE_BUF_LEN;

    i = WCHNET_SocketCreat(&SocketId_MQTT, &TmpSocketInf);
    printf("MQTT TCP Socket Created: %d\r\n", SocketId_MQTT);
    mStopIfError(i);

    i = WCHNET_SocketConnect(SocketId_MQTT);
    mStopIfError(i);
}

// 发送UDP数据到动态IP: 10.1.3.{mb_id}
extern SOCK_INF SocketInf[];
void UDP_SendTo_DynamicIP(u8 mb_id, const char *data, u32 len)
{
    u8 old_ip[4];
    u8 target_ip[4] = {10, 1, 3, mb_id};
    u32 send_len = len;

    if (SocketId_UDP == 0xFF || !data || len == 0) {
        return;
    }

    memcpy(old_ip, SocketInf[SocketId_UDP].IPAddr, 4);
    memcpy(SocketInf[SocketId_UDP].IPAddr, target_ip, 4);

    WCHNET_SocketSend(SocketId_UDP, (u8*)data, &send_len);
    printf("UDP Send to 10.1.3.%d Len:%d Data:%s\r\n", mb_id, send_len, data);

    memcpy(SocketInf[SocketId_UDP].IPAddr, old_ip, 4);
}

void WCHNET_HandleSockInt(u8 socketid, u8 intstat)
{
    if (intstat & SINT_STAT_RECV) // 收到数据
    {
        u32 len = WCHNET_SocketRecvLen(socketid, NULL);
        if (socketid == SocketId_MQTT)
        {
            if (len > 0 && len < 2048) {
                WCHNET_SocketRecv(socketid, MqttRxBuffer, &len);
                MqttRxLen = len;
            }
        }
        else
        {
            WCHNET_SocketRecv(socketid, NULL, &len);
        }
    }

    if (intstat & SINT_STAT_CONNECT)
    {
        if (socketid == SocketId_MQTT) {
            printf("MQTT TCP Connected!\r\n");
            MQTT_Conn_Flag = 1;
        }
    }

    if (intstat & SINT_STAT_DISCONNECT)
    {
        if (socketid == SocketId_MQTT) {
            printf("MQTT TCP Disconnected!\r\n");
            MQTT_Conn_Flag = 0;
        }
    }
}

void WCHNET_HandleGlobalInt(void)
{
    u8 intstat;
    u16 i;
    u8 socketint;

    intstat = WCHNET_GetGlobalInt();
    if (intstat & GINT_STAT_PHY_CHANGE)
    {
        i = WCHNET_GetPHYStatus();
        if (i & PHY_Linked_Status)
            printf("PHY Link Success\r\n");
    }
    if (intstat & GINT_STAT_SOCKET) {
        for (i = 0; i < WCHNET_MAX_SOCKET_NUM; i++) {
            socketint = WCHNET_GetSocketInt(i);
            if (socketint)
                WCHNET_HandleSockInt(i, socketint);
        }
    }
}
