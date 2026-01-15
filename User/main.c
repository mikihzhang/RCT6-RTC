#include "debug.h"
#include "ch32v20x.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "wchnet.h"
#include "UDP.h"
#include "GPIO.h"
#include "tim.h"
#include "MQTTPacket.h"
#include "app_logic.h"

// MQTT全局缓冲
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

static void Handle_MQTT_Message(unsigned char* payload, int len)
{
    ParsedData_t data;
    char udp_tx_buf[16];
    int tx_len;

    if (len >= 2048) {
        payload[2047] = 0;
    } else {
        payload[len] = 0;
    }

    printf("MQTT Payload: %s\r\n", payload);

    if (Parse_Mqtt_Json((const char *)payload, &data) != 0) {
        printf("MQTT JSON parse failed\r\n");
        return;
    }

    tx_len = Build_Serial_Udp_String(&data, udp_tx_buf, sizeof(udp_tx_buf));
    if (tx_len <= 0) {
        printf("UDP payload build failed\r\n");
        return;
    }

    UDP_SendTo_DynamicIP((u8)data.mb_id, udp_tx_buf, (u32)tx_len);
}

// 发送函数
static int transport_sendPacketBuffer(int sock, unsigned char* buf, int buflen)
{
    u32 len = buflen;
    return WCHNET_SocketSend(sock, buf, &len);
}

int main(void)
{
    SystemInit();
    USART_Printf_Init(115200);
    MCU_GPIO_Init();
    Delay_Init();

    Delay_Ms(1000);
    TIM1_Int_Init(120 - 1, 1000 - 1);
    TIM2_Init();

    printf("Init WCHNET...\r\n");
    printf("IP: %d.%d.%d.%d\r\n", IPAddr[0], IPAddr[1], IPAddr[2], IPAddr[3]);

    WCHNET_GetMacAddr(MACAddr);
    u8 i = ETH_LibInit(IPAddr, GWIPAddr, IPMask, MACAddr);
    mStopIfError(i);

    WCHNET_CreateUdpSocket();

    uint32_t last_timer = 0;
    uint32_t keepalive_timer = 0;

    while (1)
    {
        WCHNET_MainTask();

        if (WCHNET_QueryGlobalInt()) {
            WCHNET_HandleGlobalInt();
        }

        switch (CurrentState) {
            case STATE_NET_INIT:
                if (WCHNET_GetPHYStatus() & PHY_Linked_Status) {
                    printf("Net Linked, Connecting TCP...\r\n");
                    WCHNET_CreateMqttSocket();
                    CurrentState = STATE_TCP_CONNECTING;
                    last_timer = 0;
                }
                break;

            case STATE_TCP_CONNECTING:
                if (MQTT_Conn_Flag) {
                    CurrentState = STATE_MQTT_CONNECT;
                }
                last_timer++;
                if (last_timer > 100000) {
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
                if (MqttRxLen > 0) {
                    MQTTHeader header = {0};
                    header.byte = MqttRxBuffer[0];
                    if (header.bits.type == PUBLISH) {
                        unsigned char dup, retained;
                        int qos, payloadlen_in;
                        unsigned char* payload_in;
                        unsigned short msgid;
                        MQTTString receivedTopic;

                        if (MQTTDeserialize_publish(&dup, &qos, &retained, &msgid, &receivedTopic,
                                                    &payload_in, &payloadlen_in, MqttRxBuffer, MqttRxLen) == 1)
                        {
                            Handle_MQTT_Message(payload_in, payloadlen_in);
                        }
                    }
                    MqttRxLen = 0;
                }

                keepalive_timer++;
                if (keepalive_timer > 500000) {
                    int len = MQTTSerialize_pingreq(MqttTxBuffer, sizeof(MqttTxBuffer));
                    transport_sendPacketBuffer(SocketId_MQTT, MqttTxBuffer, len);
                    keepalive_timer = 0;
                }

                if (MQTT_Conn_Flag == 0) {
                    CurrentState = STATE_NET_INIT;
                }
                break;

            default:
                break;
        }
    }
}
