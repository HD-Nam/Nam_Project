#ifndef __MAINAPP_H
#define __MAINAPP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "buffer.h"
#include "router.h"
#include "lo_ptc.h"
#include "crc.h"
#include "acp.h"
#include "user_uart.h"
#include "DHT22.h"
#include "MQ135.h"
#include "PM25.h"

typedef union
{
	struct
	{
		uint16_t temp;
		uint16_t Humi;
		uint16_t Pm25;
		uint16_t CO;
		uint16_t C02;
	}param;
	struct
	{
		uint8_t temp1;
		uint8_t temp2;
		uint8_t Humi1;
		uint8_t Humi2;
		uint8_t Pm25_1;
		uint8_t Pm25_2;
		uint8_t CO_1;
		uint8_t CO_2;
		uint8_t C02_1;
		uint8_t C02_2;

	}paramBCD;
	uint8_t rawdata[10];

}data_sensor;

extern uint8_t flag_read_SS_done;
extern data_sensor value;
extern lcp_conn_t *conn;
extern lcp_t  *_lcp_cl;

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;

void mainApp(void);
void Sleep_session(void);
void Fullpower_session(void);

#ifdef __cplusplus
}
#endif
#endif
