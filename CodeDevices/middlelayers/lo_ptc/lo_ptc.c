#include "lo_ptc.h"

#include "stdlib.h"
#include "string.h"

#define MAX_CONECTION 		10
#define MAX_PORT			10

#define MY_IPADDR 			0x2101
#define VERSION				0x01
#define ENCODE				0x00
#define TIME_OF_LIVE		100
#define TIME_OUT			50
#define NO_OPTION_SELECT 	0x00
#define OPTION_SELECTED		0x01

#define LENGTH_OF_HEAD			10
#define LENGTH_OF_HEAD_TCP			6


lcp_t *newLcp(uint16_t ip_addr, uint8_t maxConn, uint8_t maxPort, uint8_t maxSizePtk, uint8_t cycleTime)
{
	lcp_t *nlcp = (lcp_t *)malloc(sizeof(lcp_t));

	nlcp->maxConn 	= maxConn;
//	nlcp->maxPort 	= maxPort;
	nlcp->cycleTime = cycleTime;
	nlcp->ip_addr 	= ip_addr;
	nlcp->realTime 	= 0;

	nlcp->Lcp_connected  	= NULL;
	nlcp->Lcp_disconnected  = NULL;
	nlcp->Lcp_sendmessage 	= NULL;

	nlcp->ListConn 	= (lcp_conn_t *)malloc(maxConn * sizeof(lcp_conn_t));
//	nlcp->listPort 	= (uint8_t *)malloc(maxPort * sizeof(uint8_t));

	nlcp->fifo		= newFifo(2);
	nlcp->fifo_send = newFifo(2);

//	for(uint8_t countPort = 0; countPort < maxPort; countPort ++)
//	{
//		nlcp->listPort[countPort] = 0;
//	}

	for(uint8_t countConn = 0; countConn < maxConn; countConn ++)
	{
		nlcp->ListConn[countConn].state = CLOSE;
		nlcp->ListConn[countConn].Flag.byte  = 0;
		nlcp->ListConn[countConn].timeCircle = 0;
		nlcp->ListConn[countConn].timeOffset = 0;
		nlcp->ListConn[countConn].timetolive = 0;
	}

	return nlcp;
}

void Lcp_Dispose(lcp_t *lcp_obj)
{
	free(lcp_obj->ListConn);
//	free(lcp_obj->listPort);

	for(uint8_t countConn = 0; countConn < lcp_obj->maxConn; countConn ++)
	{
		Fifo_Dispose(lcp_obj->ListConn[countConn].rxBuff);
		Fifo_Dispose(lcp_obj->ListConn[countConn].txBuff);
	}

	free(lcp_obj);
}

void Lcp_pushMessage(lcp_t *lcp_obj, uint8_t *data, uint16_t length)
{
	Fifo_AddData(lcp_obj->fifo, data, length);
}
uint8_t Lcp_pullMessage(lcp_t *lcp_obj, uint8_t *data, uint16_t *length)
{
	return Fifo_GetData(lcp_obj->fifo, data, length);
}
uint8_t Lcp_pullMessageSend(lcp_t *lcp_obj, uint8_t *data, uint16_t *length) // acp_getPacketsend
{
	return Fifo_GetData(lcp_obj->fifo_send, data, length);
}
uint16_t Lcp_countData(lcp_t *lcp_obj)
{
	return Fifo_GetSizeData(lcp_obj->fifo);
}


lcp_conn_t * Lcp_Connect(lcp_t *lcp_obj, point des)
{
	lcp_conn_t *newConn;
	uint16_t Timeout = 0;
	for(uint8_t count = 0; count < lcp_obj->maxConn; count ++)
	{
		if(lcp_obj->ListConn[count].state == CLOSE)
		{
			newConn = &lcp_obj->ListConn[count];
			newConn->src.addr    = lcp_obj->ip_addr;

			newConn->des.addr    = des.addr;

			newConn->Flag.bit.SYN = 1;
			while(lcp_obj->ListConn[count].state != ESTABLISHED)
			{
				if(++ Timeout == 500)
				{
					lcp_obj->ListConn[count].state = CLOSE;
					return NULL;
				}
				HAL_Delay(1);
			}
			return &lcp_obj->ListConn[count];
		}
	}

	return NULL;
}

lcp_conn_t * Lcp_disconnect(lcp_conn_t *connection)
{
	if(connection->state == ESTABLISHED)
	{

		connection->timetolive = 0;
		connection->state = CLOSE;
		connection->des.addr  = 0;
		connection->src.addr  = 0;
		Fifo_Dispose(connection->rxBuff);
		Fifo_Dispose(connection->txBuff);
	}
	return NULL;
}

void Lcp_Runtime(lcp_t *lcp_obj, uint16_t timecircle)
{

	if(++lcp_obj->realTime == 0xFFFFFFFF)
	{
		lcp_obj->realTime = 0;
	}
	if(++ lcp_obj->ListConn->timetolive >= 50)
	{
		Lcp_disconnect(lcp_obj->ListConn);

	}
}

#ifdef NODE

void Lcp_nProcess(lcp_t *lcp_obj, uint8_t Timecycle)
{
	static uint8_t timetolive;
	static uint8_t countTime = 0;
	uint8_t countTimecycle = 1000 / Timecycle;

	uint8_t 	*data 			= NULL;
	uint16_t 	lenData	 = 0;

	uint16_t 	length 			= 0;
	pack_t lcp_dataframe = { .data = NULL	};

	length = Lcp_countData(lcp_obj);

	if(length != 0)
	{
		/* phan tach frame va check crc*/
		data = (uint8_t *)malloc(length * sizeof(uint8_t));
		if((Lcp_pullMessage(lcp_obj, data, &length) == LCP_OK) && (data != NULL))
		{
			memcpy(lcp_dataframe.head.rawdata, data, sizeof(Head_t));
			if(lcp_dataframe.head.param.length > LENGTH_OF_HEAD)
			{
				lenData = lcp_dataframe.head.param.length - LENGTH_OF_HEAD;
				lcp_dataframe.data = malloc((lenData) * sizeof(uint8_t));
				memcpy(lcp_dataframe.data, &data[LENGTH_OF_HEAD], lenData);
				lcp_dataframe.crc = data[lcp_dataframe.head.param.length];
			}
			else
			{
				lcp_dataframe.crc = data[LENGTH_OF_HEAD];
				lcp_dataframe.data = NULL;
			}

			if(lcp_dataframe.crc != Crc8_CalCcit(data, lcp_dataframe.head.param.length))
			{
				goto DROP;
			}

			if(lcp_dataframe.head.param.flag.byte & LCP_SYN)
			{
				if(lcp_obj->ListConn->state == SEND)
				{
					lcp_obj->ListConn->state = ESTABLISHED;
					lcp_obj->ListConn->timeCircle = lcp_dataframe.head.param.cirle_time;
					lcp_obj->ListConn->timeOffset = lcp_dataframe.head.param.ofset_time;
					lcp_obj->ListConn->src.addr   = lcp_dataframe.head.param.id_des;
					lcp_obj->ListConn->des.addr	  = lcp_dataframe.head.param.id_scr;
					lcp_obj->ListConn->txBuff = newFifo(1);
					lcp_obj->ListConn->rxBuff = newFifo(1);

				}
				lcp_obj->realTime = lcp_dataframe.data[0] << 24 | \
														lcp_dataframe.data[1] << 16 | \
														lcp_dataframe.data[2] << 8  |
														lcp_dataframe.data[3];
				goto DROP;
			}
			else if(lcp_dataframe.head.param.flag.byte & LCP_ACK)
			{
				lcp_obj->ListConn->timetolive = 0;
			}

		}
	}

	if(lcp_obj->ListConn->Flag.bit.SYN == 1 && lcp_obj->ListConn->state == CLOSE)
	{
		lcp_obj->ListConn->state = SEND;
		lcp_obj->ListConn->Flag.bit.SYN = 0;
		lcp_obj->ListConn->timetolive = 0;
			lcp_dataframe.head.param.flag.byte = 0;
			lcp_dataframe.head.param.flag.bit.SYN = 1;
			if(lcp_dataframe.data != NULL)
			{
				free(lcp_dataframe.data);
				lcp_dataframe.data = NULL;
				lenData = 0;
			}
			goto Node_SEND;
	}
	if(lcp_obj->ListConn->state == ESTABLISHED && (lcp_obj->ListConn->timeCircle > 0))
	{
		if( lcp_obj->ListConn->Flag.bit.PSH == 1)
		{
			lenData = Fifo_GetSizeData(lcp_obj->ListConn->txBuff);
			if(lcp_obj->Lcp_sendmessage != NULL && lenData == 0)
			{
				lcp_obj->Lcp_sendmessage(lcp_obj->ListConn, 1);
			}
			lenData = Fifo_GetSizeData(lcp_obj->ListConn->txBuff);
			if(lenData > 0)
			{
				lcp_dataframe.data = (uint8_t *)malloc(lenData * sizeof(uint8_t));
				if(Fifo_GetData(lcp_obj->ListConn->txBuff, lcp_dataframe.data, &lenData) == BUFF_OK)
				{
					lcp_dataframe.head.param.flag.bit.PSH = 1;
					lcp_obj->ListConn->Flag.bit.PSH = 0;
					goto Node_SEND;
				}
			}
		}
	}

	if(lcp_obj->ListConn->Flag.bit.FIN == 1 && lcp_obj->ListConn->state == ESTABLISHED)
	{
		lcp_dataframe.head.param.flag.bit.FIN = 1;
		lcp_obj->ListConn->Flag.bit.FIN = 0;
		goto Node_SEND;
	}

	goto DROP;

	Node_SEND:

	lcp_dataframe.head.param.version      	= 0;
	lcp_dataframe.head.param.id_scr 		= lcp_obj->ListConn->src.addr;
	lcp_dataframe.head.param.id_des   		= lcp_obj->ListConn->des.addr;
	lcp_dataframe.head.param.cirle_time 	= lcp_obj->ListConn->timeCircle;
	lcp_dataframe.head.param.ofset_time		= lcp_obj->ListConn->timeOffset;
	lcp_dataframe.head.param.syn_time		= lcp_obj->realTime;
	lcp_dataframe.head.param.length 		= LENGTH_OF_HEAD + lenData;

		Buffer *BuffRespond = newBuffer();
		Buffer_AddData(BuffRespond, lcp_dataframe.head.rawdata, LENGTH_OF_HEAD);
		if(lcp_dataframe.data != NULL)
		{

			Buffer_AppendData(BuffRespond,0, lcp_dataframe.data, lenData);
			lcp_dataframe.crc = Crc8_CalCcit(BuffRespond->data, BuffRespond->length);

			Buffer_AppendData(BuffRespond, 0, &lcp_dataframe.crc, 1);
		}
		else
		{
			lcp_dataframe.crc = Crc8_CalCcit(lcp_dataframe.head.rawdata, 10);
			Buffer_AppendData(BuffRespond, 0, &lcp_dataframe.crc, 1);
		}

		Fifo_AddData(lcp_obj->fifo_send, BuffRespond->data, BuffRespond->length);

		Buffer_Dispose(BuffRespond);

DROP:
	if( ++ countTime == countTimecycle)
	{
		countTime = 0;
		if( ++ timetolive == 255)
		{
			timetolive = 0;
		}
	}
	if(data != NULL)
	{
		free(data);
		data = NULL;
	}

	if(lcp_dataframe.data != NULL)
	{
		free(lcp_dataframe.data);
		lcp_dataframe.data = NULL;
	}


}

#endif


uint8_t Lcp_getConn(lcp_t *lcp_obj, uint8_t port, lcp_conn_t *conn)
{
	lcp_conn_t *_conn = conn;
	uint8_t CountConn = 0;
	for(uint8_t count = 0; count < lcp_obj->maxConn; count ++)
	{
//		if(lcp_obj->ListConn[count].state == ESTABLISHED && lcp_obj->ListConn[count].src.port == port)
		{
			_conn[CountConn] = lcp_obj->ListConn[count];
			CountConn ++;
		}
	}
	return CountConn;
}

void Lcp_EventEstablish(lcp_t *lcp_obj, lcp_notify_connected notify)
{
	lcp_obj->Lcp_connected = notify;
}

void Lcp_EventSendMessage(lcp_t *lcp_obj, lcp_notify_sendmessage notify)
{
	lcp_obj->Lcp_sendmessage = notify;
}

void Lcp_EventDisconnected(lcp_t *lcp_obj, lcp_notify_disconnected notify)
{

	lcp_obj->Lcp_disconnected = notify;
}

