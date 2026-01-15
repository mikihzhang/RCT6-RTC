#include "UDP.h"
#include "stdio.h"
#include "wchnet.h"
#include "debug.h"
#include "app_logic.h"

#define UDP_RECE_BUF_LEN  1472

/* Global Variables */
u8 SocketId;
// u8 SocketRecvBuf[WCHNET_MAX_SOCKET_NUM][RECE_BUF_LEN]; // 接收缓冲区

u8 MACAddr[6];
u8 IPAddr[4] = { 10, 1, 3, 100 };         // 本机IP (示例)
u8 GWIPAddr[4] = { 10, 1, 0, 254 };       // 网关/NanoPi IP
u8 IPMask[4] = { 255, 255, 248, 0 };
u8 DESIP[4] = { 10, 1, 3, 0 };            // 默认目标

// MQTT Broker IP (通常是NanoPi的IP，这里设为网关IP，请根据实际情况修改)
u8 MQTT_BROKER_IP[4] = { 10, 1, 0, 254 }; 
u16 MQTT_BROKER_PORT = 1883;

u8 SocketId_UDP = 0xFF;
u8 SocketId_MQTT = 0xFF;
u8 MQTT_Conn_Flag = 0; // 0:断开, 1:已连接TCP

u8 SocketRecvBuf[WCHNET_MAX_SOCKET_NUM][UDP_RECE_BUF_LEN];

// 用于传递接收到的MQTT数据给Main Loop
extern u8 MqttRxBuffer[2048];
extern volatile u16 MqttRxLen;

void mStopIfError(u8 iError)
{
    if (iError == WCHNET_ERR_SUCCESS) return;
    printf("Error: %02X\r\n", (u16) iError);
}

// 创建用于转发数据的UDP Socket
void WCHNET_CreateUdpSocket(void)
{
    u8 i;
    SOCK_INF TmpSocketInf;
    memset((void *) &TmpSocketInf, 0, sizeof(SOCK_INF));
    
    // 源端口设为 2000, 目标端口 1000
    memcpy((void *) TmpSocketInf.IPAddr, DESIP, 4);
    TmpSocketInf.DesPort = 1000;
    TmpSocketInf.SourPort = 2000; 
    TmpSocketInf.ProtoType = PROTO_TYPE_UDP;
    TmpSocketInf.RecvStartPoint = (u32) SocketRecvBuf[0]; // 使用 Buffer 0
    TmpSocketInf.RecvBufLen = UDP_RECE_BUF_LEN;
    
    i = WCHNET_SocketCreat(&SocketId_UDP, &TmpSocketInf);
    printf("UDP Socket Created: %d\r\n", SocketId_UDP);
    mStopIfError(i);
}

// 创建用于连接MQTT Broker的TCP Socket
void WCHNET_CreateMqttSocket(void)
{
    u8 i;
    SOCK_INF TmpSocketInf;
    memset((void *) &TmpSocketInf, 0, sizeof(SOCK_INF));
    
    memcpy((void *) TmpSocketInf.IPAddr, MQTT_BROKER_IP, 4);
    TmpSocketInf.DesPort = MQTT_BROKER_PORT;
    TmpSocketInf.SourPort = 3000; // 本地TCP端口
    TmpSocketInf.ProtoType = PROTO_TYPE_TCP;
    TmpSocketInf.RecvStartPoint = (u32) SocketRecvBuf[1]; // 使用 Buffer 1
    TmpSocketInf.RecvBufLen = UDP_RECE_BUF_LEN;
    
    i = WCHNET_SocketCreat(&SocketId_MQTT, &TmpSocketInf);
    printf("MQTT TCP Socket Created: %d\r\n", SocketId_MQTT);
    mStopIfError(i);
    
    // 立即发起TCP连接
    i = WCHNET_SocketConnect(SocketId_MQTT);
    mStopIfError(i);
}

// 发送UDP数据到指定的 mb_id (10.1.3.x)
extern SOCK_INF SocketInf[];
void UDP_SendTo_DynamicIP(u8 mb_id, char *data, u32 len)
{
    if(SocketId_UDP == 0xFF) return;

    // 1. 备份旧IP
    u8 old_ip[4];
    memcpy(old_ip, SocketInf[SocketId_UDP].IPAddr, 4);

    // 2. 设置目标IP: 10.1.3.{mb_id}
    u8 target_ip[4] = {10, 1, 3, mb_id};
    memcpy(SocketInf[SocketId_UDP].IPAddr, target_ip, 4);

    // 3. 发送数据
    u32 send_len = len;
    WCHNET_SocketSend(SocketId_UDP, (u8*)data, &send_len);
    printf("UDP Send to 10.1.3.%d Len:%d\r\n", mb_id, send_len);

    // 4. 恢复IP (以免影响其他逻辑)
    memcpy(SocketInf[SocketId_UDP].IPAddr, old_ip, 4);
}

void WCHNET_HandleSockInt(u8 socketid, u8 intstat)
{
    if (intstat & SINT_STAT_RECV) // 收到数据
    {
        u32 len = WCHNET_SocketRecvLen(socketid, NULL);
        if(socketid == SocketId_MQTT) 
        {
            // 如果是MQTT数据，读入到主循环缓冲区
            // 注意：这里简单处理，实际应考虑环形缓冲
            if(len > 0 && len < 2048) {
                WCHNET_SocketRecv(socketid, MqttRxBuffer, &len);
                MqttRxLen = len; // 通知主循环处理
            }
        }
        else 
        {
            // 清空其他Socket的数据
            WCHNET_SocketRecv(socketid, NULL, &len);
        }
    }
    
    if (intstat & SINT_STAT_CONNECT) // TCP连接成功
    {
        if(socketid == SocketId_MQTT) {
            printf("MQTT TCP Connected!\r\n");
            MQTT_Conn_Flag = 1;
        }
    }
    
    if (intstat & SINT_STAT_DISCONNECT) // TCP断开
    {
        if(socketid == SocketId_MQTT) {
            printf("MQTT TCP Disconnected!\r\n");
            MQTT_Conn_Flag = 0;
            // 可以在main中检测此标志并重连
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

/*
 * 函数名: WCHNET_UDP_Recv
 * 描述  : UDP接收回调函数
 */
void WCHNET_UDP_Recv(u8 id, u8 *buf, u32 len, u8 *addr, u16 port)
{
    // 打印来源信息
    printf("RX from IP: %d.%d.%d.%d, Port: %d\r\n", 
           addr[0], addr[1], addr[2], addr[3], port);

    // 安全处理字符串结束符
    if (len < RECE_BUF_LEN) {
        buf[len] = '\0';
    } else {
        buf[RECE_BUF_LEN - 1] = '\0';
    }

    // 调用逻辑处理
    // 传入 id (SocketId) 以便逻辑层使用同一个Socket句柄发送数据
    Process_Network_Packet(id, (char*)buf, (uint16_t)len);
}

/*
 * 函数名: WCHNET_CreateUDP
 * 描述  : 创建UDP Socket
 */
void WCHNET_CreateUDP(void)
{
    u8 i;
    SOCK_INF TmpSocketInf;

    memset((void *)&TmpSocketInf, 0, sizeof(SOCK_INF));

    TmpSocketInf.SourPort = 8080;  // 监听来自 NanoPi 的端口
    TmpSocketInf.ProtoType = PROTO_TYPE_UDP;
    
    // 创建Socket
    i = WCHNET_SocketCreat(&SocketId, &TmpSocketInf);
    if (i == WCHNET_ERR_SUCCESS)
    {
        printf("UDP Socket Created, ID: %d. Listening on Port 8080\r\n", SocketId);
        // 注册接收回调
        WCHNET_RegisterRecvCallBack(SocketId, WCHNET_UDP_Recv);
    }
    else
    {
        printf("WCHNET_SocketCreat Fail: %02x\r\n", i);
    }
}
