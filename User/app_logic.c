#include "app_logic.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static const char *find_key_value(const char *json, const char *key)
{
    char key_pat[32];
    const char *pos;

    snprintf(key_pat, sizeof(key_pat), "\"%s\"", key);
    pos = strstr(json, key_pat);
    if (!pos) {
        return NULL;
    }

    pos = strchr(pos, ':');
    if (!pos) {
        return NULL;
    }

    pos++;
    while (*pos == ' ' || *pos == '"') {
        pos++;
    }

    return pos;
}

static int parse_int_value(const char *value)
{
    if (!value) {
        return -1;
    }
    return atoi(value);
}

static void parse_string_value(const char *value, char *out_buf, uint16_t max_len)
{
    uint16_t i = 0;

    if (!value || max_len == 0) {
        return;
    }

    while (value[i] != '"' && value[i] != ',' && value[i] != '}' && value[i] != '\0' && i < max_len - 1) {
        out_buf[i] = value[i];
        i++;
    }
    out_buf[i] = '\0';
}

int Parse_Mqtt_Json(const char *json, ParsedData_t *out_data)
{
    const char *ptr;

    if (!json || !out_data) {
        return -1;
    }

    memset(out_data, 0, sizeof(*out_data));

    ptr = find_key_value(json, "sys_time");
    parse_string_value(ptr, out_data->sys_time, sizeof(out_data->sys_time));

    ptr = find_key_value(json, "mb_id");
    out_data->mb_id = parse_int_value(ptr);

    ptr = find_key_value(json, "bs_id");
    out_data->bs_id = parse_int_value(ptr);

    ptr = find_key_value(json, "sensor_id");
    out_data->sensor_id = parse_int_value(ptr);

    ptr = find_key_value(json, "mode");
    parse_string_value(ptr, out_data->mode, sizeof(out_data->mode));

    if (out_data->mb_id <= 0 || out_data->mb_id >= 255) {
        return -1;
    }

    return 0;
}

int Build_Serial_Udp_String(const ParsedData_t *data, char *out_buf, uint16_t out_len)
{
    char mode_char = '0';
    int written;

    if (!data || !out_buf || out_len == 0) {
        return -1;
    }

    if (data->mode[0] != '\0') {
        mode_char = data->mode[0];
    }

    written = snprintf(out_buf, out_len, "M%dB%dS%d%c", data->mb_id, data->bs_id, data->sensor_id, mode_char);
    if (written < 0) {
        return -1;
    }

    if (written >= out_len) {
        out_buf[out_len - 1] = '\0';
        return (int)(out_len - 1);
    }

    return written;
}
