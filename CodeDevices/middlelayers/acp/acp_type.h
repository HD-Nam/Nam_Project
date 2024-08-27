#ifndef __ACP_TYPE_H
#define __ACP_TYPE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "router.h"

#define  ACP_NULL            (void *)0
#define  ACP_OK              0x00
#define  ACP_NOT_OK          0x01

typedef uint8_t Acp_ReturnType;


typedef union
{
	struct
	{
		uint8_t Encode;
		uint8_t Version;
		uint8_t IP_length;
		uint8_t Option;
		uint16_t TotalLength;
		uint16_t IP_sourceAddr;
		uint16_t IP_destAddr;
		uint8_t TimeOflive;
		uint8_t Crc;
	}param;
	uint8_t rawdata[12];
}IP_head_t;

typedef union
{
	struct{
		uint8_t FIN :1;
		uint8_t SYN :1;
		uint8_t RST :1;
		uint8_t PSH :1;
		uint8_t ACK :1;
		uint8_t URG :1;
		uint8_t rev :2;
	}bit;
	uint8_t byte;
}acp_flag_t;

typedef union
{
	struct{
		uint8_t 	SourcePort;
		uint8_t 	DestPort;
		uint8_t 	SeqNumber;
		uint8_t 	AckNumber;
		acp_flag_t 	Flag;
		uint8_t 	Crc;
	}param;
	uint8_t rawdata[6];
}TCP_head_t;

typedef struct
{
	IP_head_t  	IP_Head;
	uint8_t 	*IP_padding;
	TCP_head_t 	TCP_Head;
	uint8_t 	*TCP_padding;
}acp_frame_t;

typedef enum
{
	ACP_CLOSE,
	ACP_OPEN,
	ACP_RCVD,
	ACP_SEND,
	ACP_ESTABLISHED,
	ACP_FIN_WAIT_1,
	ACP_CLOSE_WAIT,
	ACP_LAST_ACK,
	ACP_FIN_WAIT_2,
	ACP_TIME_WAIT,
}acp_conn_state;

typedef struct
{
	uint8_t data[64];
	uint16_t length;
}acp_buff;


typedef struct
{
	uint16_t  addr;
	uint8_t   port;
}acp_point;


typedef struct
{
	acp_flag_t Flag;
	uint8_t    TimeToLive;
	acp_point  src;
	acp_point  des;
	uint8_t    SeqNumber;
    uint8_t    AckNumber;
	acp_conn_state state;
	Fifo     *rxBuff;
	Fifo     *txBuff;
}acp_conn;



typedef void (* acp_notify_connected)(acp_conn *conn, uint8_t port);
typedef void (* acp_notify_disconnected)(acp_conn *conn, uint8_t port);
typedef void (* acp_notify_receiver)(acp_conn *conn);
typedef void (* acp_callback_t)(uint8_t *a, uint16_t b);

typedef struct
{
	uint8_t maxConn;
	uint8_t maxPort;
	uint16_t cycleTime;
	uint8_t timeoutConn;
	uint16_t ip_addr;
	acp_conn *ListConn;
	uint8_t  *listPort;
	Fifo	 *fifo;
	Fifo	 *fifo_send;
	acp_notify_connected 	Acp_connected;
	acp_notify_disconnected Acp_disconnected;
}acp;




#ifdef __cplusplus
}
#endif

#endif
