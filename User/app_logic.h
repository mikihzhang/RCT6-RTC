#ifndef __APP_LOGIC_H
#define __APP_LOGIC_H

#include <stdint.h>
#include "ch32v20x.h"

// 定义解析后的数据结构
typedef struct {
    char sys_time[25]; // "2026-01-09 10:53:30"
    int mb_id;         // 主板序号
    int bs_id;         // 基站序号
    int sensor_id;     // 传感器序号
    char mode[16];     // inspection 或 manual
} ParsedData_t;

// 函数声明
// 注意增加了 socket_id 参数，用于 UDP 发送
void Process_Network_Packet(u8 socket_id, char* payload, uint16_t len);

#endif