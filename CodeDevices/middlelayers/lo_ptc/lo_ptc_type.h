#ifndef __LO_PTR_TYPE_H__
#define __LO_PTR_TYPE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "buffer.h"

#define GATEWAY
#define NODE
#define LENGTH_HEAD		4

#define LCP_OK			0
#define LCP_NOT_OK		1

#define LCP_FIN		0x01
#define LCP_SYN		0x02
#define LCP_PSH		0x04
#define LCP_ACK		0x08
#define LCP_URG		0x20


typedef union
{
	struct
	{
		uint8_t FIN : 1;
		uint8_t SYN : 1;	// connect flag
		uint8_t PSH : 1;	// send data
		uint8_t ACK	: 1;

	}bit;
	uint8_t byte;
}flag_t;

typedef enum
{
	CLOSE,
	OPEN,
	SEND,
	ESTABLISHED,
	CLOSE_WAIT,
}state_t;

typedef struct
{
	uint16_t  addr;
}point;

typedef struct
{
	flag_t	 	Flag;
	point  		src;
	point  		des;
	uint8_t     timetolive;
	uint8_t    	timeCircle;
    uint8_t    	timeOffset;
    state_t 	state;
	Fifo     	*rxBuff;
	Fifo     	*txBuff;

}lcp_conn_t;

typedef void (* lcp_notify_connected)(lcp_conn_t *conn, uint8_t port);
typedef void (* lcp_notify_sendmessage)(lcp_conn_t *conn, uint8_t port);
typedef void (* lcp_notify_disconnected)(lcp_conn_t *conn, uint8_t port);
typedef void (* lcp_notify_syntime)(uint32_t time);

typedef struct
{
	uint8_t maxConn;
	uint8_t cycleTime;
	uint32_t realTime;
	uint8_t ip_addr;
	lcp_conn_t *ListConn;
	Fifo	 *fifo;
	Fifo	 *fifo_send;
	lcp_notify_connected 	Lcp_connected;
	lcp_notify_sendmessage	Lcp_sendmessage;
	lcp_notify_disconnected Lcp_disconnected;
	lcp_notify_syntime		Lcp_synTime;
}lcp_t;



typedef union
{
	struct
	{
		uint8_t version;
		uint8_t length;
		uint8_t syn_time;
		uint8_t cirle_time;
		uint8_t ofset_time;
		uint8_t id_scr;
		uint8_t id_des;
		flag_t  flag;
	}param;
	uint8_t rawdata[8];
}Head_t;

typedef struct
{
	Head_t  head;
	uint8_t *data;
	uint8_t  crc;
}pack_t;



#ifdef __cplusplus
}
#endif
#endif
