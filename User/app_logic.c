#include "app_logic.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "debug.h"
#include "wchnet.h"

/* * 简单的字符串查找辅助函数，用于提取JSON值 
 * 避免引入大型JSON库，节省资源
 */
static char* parse_json_value(char* json, const char* key) {
    char* pos = strstr(json, key);
    if (pos) {
        // 找到key，跳过key和冒号、引号等
        pos += strlen(key);
        while (*pos == ' ' || *pos == ':' || *pos == '"') pos++;
        return pos;
    }
    return NULL;
}

static int parse_int(char* val_str) {
    if (!val_str) return -1;
    return atoi(val_str);
}

static void parse_string(char* val_str, char* out_buf, uint16_t max_len) {
    if (!val_str) return;
    uint16_t i = 0;
    while (val_str[i] != '"' && val_str[i] != ',' && val_str[i] != '}' && i < max_len - 1) {
        out_buf[i] = val_str[i];
        i++;
    }
    out_buf[i] = '\0';
}

/* * 核心处理函数：解析网络数据并转发 
 * socket_id: 用于发送UDP数据的socket句柄 (复用接收的socket)
 * payload: 接收到的JSON原始数据
 * len: 数据长度
 */
void Process_Network_Packet(u8 socket_id, char* payload, uint16_t len) {
    ParsedData_t data = {0};
    
    printf("RX JSON: %s\r\n", payload); // 调试打印

    // --- 1. 解析 JSON 数据 ---
    
    // 提取 sys_time
    char* ptr = parse_json_value(payload, "sys_time");
    if (ptr) parse_string(ptr, data.sys_time, sizeof(data.sys_time));

    // 提取 mb_id (主板ID) -> 关键参数，用于生成IP
    ptr = parse_json_value(payload, "mb_id");
    if (ptr) data.mb_id = parse_int(ptr);

    // 提取 bs_id (基站ID)
    ptr = parse_json_value(payload, "bs_id");
    if (ptr) data.bs_id = parse_int(ptr);

    // 提取 sensor_id
    ptr = parse_json_value(payload, "sensor_id");
    if (ptr) data.sensor_id = parse_int(ptr);
    
    // 提取 mode
    ptr = parse_json_value(payload, "mode");
    if (ptr) parse_string(ptr, data.mode, sizeof(data.mode));

    // --- 2. 逻辑处理与分发 ---
    if (data.mb_id > 0 && data.mb_id < 255) {
        
        // A. 构建目标 IP 地址 (10.1.3.x)
        // 这里的 x 就是 mb_id
        uint8_t target_ip[4] = {10, 1, 3, (uint8_t)data.mb_id};
        
        // CH9121 默认或者你配置的 UDP 监听端口
        // 请确保目标主板上的 CH9121 配置为 UDP Server 或者 UDP Client 监听此端口
        uint16_t target_port = 8888; 

        // B. 构建串口协议帧 (二进制格式)
        // 目标 CH9121 收到这个包后，会原封不动地从串口吐出给主板 MCU
        // 格式示例: [头0xAA][头0x55][长度][CMD][MB_ID][BS_ID_H][BS_ID_L][SENSOR_ID][TIME...][校验]
        
        uint8_t tx_buffer[200]; // 缓冲区稍微大一点以防时间字符串过长
        uint8_t idx = 0;
        
        tx_buffer[idx++] = 0xAA; // 帧头1
        tx_buffer[idx++] = 0x55; // 帧头2
        tx_buffer[idx++] = 0;    // 长度占位 (idx=2)
        
        tx_buffer[idx++] = 0x01; // CMD: 0x01 (示例：下发配置指令)
        
        // 填充业务数据
        tx_buffer[idx++] = (uint8_t)data.mb_id;          // 主板ID
        tx_buffer[idx++] = (uint8_t)(data.bs_id >> 8);   // 基站ID 高8位
        tx_buffer[idx++] = (uint8_t)(data.bs_id & 0xFF); // 基站ID 低8位
        tx_buffer[idx++] = (uint8_t)data.sensor_id;      // 传感器ID
        
        // 填充模式 mode (字符串)
        uint8_t mode_len = strlen(data.mode);
        memcpy(&tx_buffer[idx], data.mode, mode_len);
        idx += mode_len;
        tx_buffer[idx++] = 0x00; // 字符串结束符 (可选，方便接收端解析)
        
        // 填充时间 sys_time (字符串)
        uint8_t time_len = strlen(data.sys_time);
        memcpy(&tx_buffer[idx], data.sys_time, time_len);
        idx += time_len;
        
        // 回填长度 (长度 = 当前索引 - 帧头3字节)
        tx_buffer[2] = idx - 3; 
        
        // 计算校验和 (从 CMD 开始到数据结束的累加和)
        uint8_t checksum = 0;
        for(int i = 3; i < idx; i++) {
            checksum += tx_buffer[i];
        }
        tx_buffer[idx++] = checksum; // 帧尾/校验
        
        // C. 通过 UDP 发送二进制帧到目标主板
        // 此时，本机的以太网口 -> 网络 -> 目标IP的CH9121 -> 目标主板串口
        WCHNET_SocketSendTo(socket_id, target_ip, target_port, tx_buffer, idx);
        
        printf("Forwarded Protocol Frame to %d.%d.%d.%d:%d (Len:%d)\r\n", 
               target_ip[0], target_ip[1], target_ip[2], target_ip[3], target_port, idx);
        
    } else {
        printf("Error: Invalid MB_ID received: %d\r\n", data.mb_id);
    }
}
