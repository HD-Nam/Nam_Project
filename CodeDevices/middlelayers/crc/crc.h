#ifndef __CRC_H__
#define __CRC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "main.h"

uint8_t Crc8_CalCcit(uint8_t *data, uint16_t length);
uint16_t Get_CRC16_Modbus(uint8_t *Data, uint8_t Length);
#ifdef __cplusplus
}
#endif
#endif
