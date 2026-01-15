#ifndef __APP_LOGIC_H
#define __APP_LOGIC_H

#include <stdint.h>
#include "ch32v20x.h"

// MQTT 解析结构体
typedef struct {
    char sys_time[25]; // "2026-01-09 10:53:30"
    int mb_id;         // 主板编号
    int bs_id;         // 站点编号
    int sensor_id;     // 传感器编号
    char mode[16];     // 模式字符串
} ParsedData_t;

// 解析 MQTT JSON payload
// 成功返回 0，失败返回 -1
int Parse_Mqtt_Json(const char *json, ParsedData_t *out_data);

// 生成串口字符串(<=16字节)用于UDP发送
// 返回生成的长度，失败返回 -1
int Build_Serial_Udp_String(const ParsedData_t *data, char *out_buf, uint16_t out_len);

#endif
