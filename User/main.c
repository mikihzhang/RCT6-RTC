#include "debug.h"
#include "ch32v20x.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "wchnet.h"
#include "UDP.h"
#include "GPIO.h"
#include "tim.h"
// #include "DS18B20.h"   // 如果DS18B20还需要则保留，不需要也可以删除
// #include "MCP41010.h"  // 如果数字电位器还需要则保留
#include "MQTTPacket.h"

// MQTT全局变量
u8 MqttTxBuffer[1024];
u8 MqttRxBuffer[2048];
volatile u16 MqttRxLen = 0;

// 系统状态
typedef enum {
    STATE_NET_INIT,
    STATE_TCP_CONNECTING,
    STATE_MQTT_CONNECT,
    STATE_MQTT_SUBSCRIBE,
    STATE_MQTT_RUNNING,
    STATE_ERROR
} SystemState_t;

SystemState_t CurrentState = STATE_NET_INIT;

// 简单的JSON值提取函数
void JSON_GetValue(char *json, char *key, char *out, int max_len) {
    char key_pat[32];
    sprintf(key_pat, "\"%s\"", key);
    char *p = strstr(json, key_pat);
    if(!p) { out[0]=0; return; }
    
    p = strchr(p, ':');
    if(!p) { out[0]=0; return; }
    p++;
    
    while(*p == ' ' || *p == '"') p++; // 跳过空格和引号
    
    int i=0;
    while(*p && *p != '"' && *p != ',' && *p != '}' && *p != ' ' && i < max_len-1) {
        out[i++] = *p++;
    }
    out[i] = 0;
}

// MQTT 消息处理回调
void Handle_MQTT_Message(unsigned char* payload, int len) {
    if(len >= 2048) payload[2047] = 0;
    else payload[len] = 0;

    printf("MQTT Payload: %s\r\n", payload);

    // 解析JSON
    char mb_id_str[10], sys_time[32], bs_id[10], sensor_id[10], mode[16];
    
    JSON_GetValue((char*)payload, "mb_id", mb_id_str, sizeof(mb_id_str));
    JSON_GetValue((char*)payload, "sys_time", sys_time, sizeof(sys_time));
    JSON_GetValue((char*)payload, "bs_id", bs_id, sizeof(bs_id));
    JSON_GetValue((char*)payload, "sensor_id", sensor_id, sizeof(sensor_id));
    JSON_GetValue((char*)payload, "mode", mode, sizeof(mode));

    int mb_id = atoi(mb_id_str);
    
    if(mb_id > 0) {
        // 格式化为串口数据格式
        char udp_tx_buf[256];
        int tx_len = snprintf(udp_tx_buf, sizeof(udp_tx_buf), 
            "#TIME:%s,MB:%s,BS:%s,SID:%s,MODE:%s;\r\n",
            sys_time, mb_id_str, bs_id, sensor_id, mode);
            
        // 通过UDP转发到 10.1.3.x
        UDP_SendTo_DynamicIP((u8)mb_id, udp_tx_buf, tx_len);
    }
}

// 辅助发送函数
int transport_sendPacketBuffer(int sock, unsigned char* buf, int buflen) {
    u32 len = buflen;
    return WCHNET_SocketSend(sock, buf, &len);
}

int main(void)
{  
    SystemInit();
    USART_Printf_Init(115200); 
    MCU_GPIO_Init();
    Delay_Init();
    // GPIO_SPI_Init();
    // GPIO_SPI_SetSpeed(2);
    
    // 初始化其他保留的外设
    // DS18B20_InitAll();
    // BOMA_Control();  // 删除: 不再需要拨码开关
    // ADC_Function_Init(); // 删除: 不再需要ADC
    
    Delay_Ms(1000);
    // MCP41010_BEGIN();
    TIM1_Int_Init(120 - 1, 1000 - 1);
    TIM2_Init();

    // 删除从拨码开关获取IP的逻辑
    // IPAddr[3] = PORT_value; 
    // DESIP[3] = box_value;
    
    // 确保 UDP.c 中定义的 IPAddr 是正确的静态IP
    printf("Init WCHNET...\r\n");
    printf("IP: %d.%d.%d.%d\r\n", IPAddr[0], IPAddr[1], IPAddr[2], IPAddr[3]);

    WCHNET_GetMacAddr(MACAddr);
    u8 i = ETH_LibInit(IPAddr, GWIPAddr, IPMask, MACAddr);
    mStopIfError(i);
    
    WCHNET_CreateUdpSocket(); 
    
    uint32_t last_timer = 0;
    uint32_t keepalive_timer = 0;

    while(1)
    {
        WCHNET_MainTask();
        
        if(WCHNET_QueryGlobalInt()) {
            WCHNET_HandleGlobalInt();
        }
        
        // MQTT 状态机
        switch(CurrentState) {
            case STATE_NET_INIT:
                if(WCHNET_GetPHYStatus() & PHY_Linked_Status) {
                    printf("Net Linked, Connecting TCP...\r\n");
                    WCHNET_CreateMqttSocket(); 
                    CurrentState = STATE_TCP_CONNECTING;
                    last_timer = 0; 
                }
                break;
                
            case STATE_TCP_CONNECTING:
                if(MQTT_Conn_Flag) {
                    CurrentState = STATE_MQTT_CONNECT;
                }
                last_timer++;
                if(last_timer > 100000) { 
                    printf("TCP Timeout, Retry...\r\n");
                    WCHNET_SocketClose(SocketId_MQTT, TCP_CLOSE_NORMAL);
                    CurrentState = STATE_NET_INIT;
                }
                break;
                
            case STATE_MQTT_CONNECT:
                {
                    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
                    data.MQTTVersion = 3;
                    data.clientID.cstring = "CH32V208_Client";
                    data.keepAliveInterval = 60;
                    data.cleansession = 1;
                    
                    int len = MQTTSerialize_connect(MqttTxBuffer, sizeof(MqttTxBuffer), &data);
                    transport_sendPacketBuffer(SocketId_MQTT, MqttTxBuffer, len);
                    
                    Delay_Ms(500); 
                    CurrentState = STATE_MQTT_SUBSCRIBE; 
                    printf("Sent MQTT Connect\r\n");
                }
                break;
                
            case STATE_MQTT_SUBSCRIBE:
                {
                    MQTTString topicString = MQTTString_initializer;
                    topicString.cstring = "task/downlink"; 
                    int msgid = 1;
                    int req_qos = 0;
                    
                    int len = MQTTSerialize_subscribe(MqttTxBuffer, sizeof(MqttTxBuffer), 0, msgid, 1, &topicString, &req_qos);
                    transport_sendPacketBuffer(SocketId_MQTT, MqttTxBuffer, len);
                    printf("Sent Subscribe\r\n");
                    CurrentState = STATE_MQTT_RUNNING;
                }
                break;
                
            case STATE_MQTT_RUNNING:
                if(MqttRxLen > 0) {
                    MQTTHeader header = {0};
                    header.byte = MqttRxBuffer[0];
                    if(header.bits.type == PUBLISH) {
                        unsigned char dup, retained;
                        int qos, payloadlen_in;
                        unsigned char* payload_in;
                        unsigned short msgid; 
                        MQTTString receivedTopic;
                        
                        if(MQTTDeserialize_publish(&dup, &qos, &retained, &msgid, &receivedTopic,
                                                &payload_in, &payloadlen_in, MqttRxBuffer, MqttRxLen) == 1) 
                        {
                            Handle_MQTT_Message(payload_in, payloadlen_in);
                        }
                    }
                    MqttRxLen = 0;
                }
                
                keepalive_timer++;
                if(keepalive_timer > 500000) { 
                    int len = MQTTSerialize_pingreq(MqttTxBuffer, sizeof(MqttTxBuffer));
                    transport_sendPacketBuffer(SocketId_MQTT, MqttTxBuffer, len);
                    keepalive_timer = 0;
                }
                
                if(MQTT_Conn_Flag == 0) {
                    CurrentState = STATE_NET_INIT;
                }
                break;
                
            default:
                break;
        }
    }
}

