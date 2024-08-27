#include "acp.h"

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

#define STATUS_CLIENT_HANDSHAKE		0x01
#define STATUS_CLIENT_TRANSFER		0x02
#define STATUS_CLIENT_CLOSE			0x03
#define STATUS_SERVER_HANDSHAKE		0x04

#define LENGTH_OF_HEAD_IP			12
#define LENGTH_OF_HEAD_TCP			6


acp *newAcp(uint16_t ip_addr, uint8_t maxConn, uint8_t maxPort, uint8_t maxSizePtk, uint16_t cycleTime)
{
	acp *nacp = (acp *)malloc(sizeof(acp));

	nacp->maxConn = maxConn;
	nacp->maxPort = maxPort;
	nacp->cycleTime = cycleTime;
	nacp->ip_addr = ip_addr;
	nacp->Acp_connected  = NULL;
	nacp->Acp_disconnected  = NULL;

//	nacp->Acp_connected = (acp_notify_connected *)malloc(maxConn * sizeof(acp_notify_connected));
//	nacp->Acp_disconnected = (acp_notify_disconnected *)malloc(maxConn * sizeof(acp_notify_disconnected));
	nacp->ListConn = (acp_conn *)malloc(maxConn * sizeof(acp_conn));
	nacp->listPort = (uint8_t *)malloc(maxPort * sizeof(uint8_t));
	nacp->fifo	= newFifo(2);
	nacp->fifo_send = newFifo(2);

	for(uint8_t countPort = 0; countPort < maxPort; countPort ++)
	{
		nacp->listPort[countPort] = 0;
	}

	for(uint8_t countConn = 0; countConn < maxConn; countConn ++)
	{
		nacp->ListConn[countConn].state = ACP_CLOSE;
		nacp->ListConn[countConn].Flag.byte  = 0;
		nacp->ListConn[countConn].SeqNumber = 0;
		nacp->ListConn[countConn].AckNumber = 0;
	}

	return nacp;
}

void Acp_Dispose(acp *acp_obj)
{
	free(acp_obj->ListConn);
	free(acp_obj->listPort);

	for(uint8_t countConn = 0; countConn < acp_obj->maxConn; countConn ++)
	{
		Fifo_Dispose(acp_obj->ListConn[countConn].rxBuff);
		Fifo_Dispose(acp_obj->ListConn[countConn].txBuff);
	}

	free(acp_obj);
}

void Acp_pushMessage(acp *acp_obj, uint8_t *data, uint16_t length)
{
	Fifo_AddData(acp_obj->fifo, data, length);
}
Acp_ReturnType Acp_pullMessage(acp *acp_obj, uint8_t *data, uint16_t *length)
{
	return Fifo_GetData(acp_obj->fifo, data, length);
}
Acp_ReturnType Acp_pullMessageSend(acp *acp_obj, uint8_t *data, uint16_t *length) // acp_getPacketsend
{
	return Fifo_GetData(acp_obj->fifo_send, data, length);
}
uint16_t Acp_countData(acp *acp_obj)
{
	return Fifo_GetSizeData(acp_obj->fifo);
}


Acp_ReturnType Acp_Listen(acp *acp_obj, uint8_t port)
{
	for(uint8_t countPort = 0; countPort < acp_obj->maxPort; countPort ++)
	{
		if(acp_obj->listPort[countPort] == 0 && acp_obj->ListConn[countPort].state == ACP_CLOSE)
		{
			acp_obj->listPort[countPort] = port;
			acp_obj->ListConn[countPort].state = ACP_OPEN;
			return ACP_OK;
		}
	}
	return ACP_NOT_OK;
}

acp_conn * Acp_Connect(acp *acp_obj, acp_point des, acp_point scr)
{
	acp_conn *newConn;
	uint8_t Timeout = 0;
	uint8_t Scr_port = rand() % 255;
	for(uint8_t count = 0; count < acp_obj->maxConn; count ++)
	{
		if(acp_obj->ListConn[count].state == ACP_CLOSE)
		{
			newConn = &acp_obj->ListConn[count];
//			newConn->src.addr    = scr.addr;
//			newConn->src.port	 = scr.port;

			newConn->src.addr = acp_obj->ip_addr;
			for(uint8_t count = 0; count < acp_obj->maxConn; count ++)
			{
				if(Scr_port == acp_obj->ListConn[count].src.port)
				{
					Scr_port = rand() % 255;
					count = 0;
				}
			}
			newConn->src.port = Scr_port;

			newConn->des.addr    = des.addr;
			newConn->des.port	 = des.port;

			newConn->Flag.bit.SYN = 1;
			while(acp_obj->ListConn[count].state != ACP_ESTABLISHED)
			{
				if(++ Timeout == acp_obj->timeoutConn)
				{
					acp_obj->ListConn[count].state = ACP_CLOSE;
					return NULL;
				}
				HAL_Delay(1);
			}
			return &acp_obj->ListConn[count];
		}
	}

	return NULL;
}

acp_conn * Acp_disconnect(acp_conn *connection)
{
	uint16_t timeout = 0;
	if(connection->state == ACP_ESTABLISHED)
	{
		connection->Flag.bit.FIN = 1;
		while(connection->state != ACP_CLOSE)
		{
			if(++ timeout == 5000)
			{
				connection->state = ACP_CLOSE;
				connection->AckNumber = 0;
				connection->SeqNumber = 0;
				connection->des.addr  = 0;
				connection->des.port  = 0;
				connection->src.addr  = 0;
				connection->src.port  = 0;
				Fifo_Dispose(connection->rxBuff);
				Fifo_Dispose(connection->txBuff);
				break;
			}
			HAL_Delay(1);
		}
	}
	return NULL;
}




void Acp_Process(acp *acp_obj, uint8_t Timecycle)
{
	static uint8_t timetolive		= 0;
	static uint8_t countTime = 0;
	uint8_t countTimecycle = 100 / Timecycle;

	uint8_t 	*data 			= NULL;
	uint16_t 	lengTCP_Padding = 0;
	uint8_t 	pos 			= 0;
	uint8_t 	PosConn 		= 0;
	Buffer 		*acp_buff 		= NULL;
	uint16_t 	length 			= 0;
	acp_frame_t acp_dataframe = { .IP_padding = NULL,
								  .TCP_padding = NULL, };

	length = Acp_countData(acp_obj);

	if(length != 0)
	{
		/* phan tach frame va check crc*/
		data = (uint8_t *)malloc(length * sizeof(uint8_t));
		if((Acp_pullMessage(acp_obj, data, &length) == ACP_OK) && (data != NULL))
		{
			acp_buff = newBuffer();
			Buffer_AddData(acp_buff, data, length);
			Buffer_SplitData(acp_buff, pos, LENGTH_OF_HEAD_IP + 1);
			Buffer_GetData(acp_buff, pos, acp_dataframe.IP_Head.rawdata, &length);
			pos ++;
			if(acp_dataframe.IP_Head.param.IP_length > LENGTH_OF_HEAD_IP)
			{
				acp_dataframe.IP_padding = (uint8_t *)malloc(acp_dataframe.IP_Head.param.IP_length - 12);
				Buffer_SplitData(acp_buff, pos, acp_dataframe.IP_Head.param.IP_length - 12);
				Buffer_GetData(acp_buff, pos, acp_dataframe.IP_padding, &length);
				pos ++;
				Buffer *Buff_crc = newBuffer();
				Buffer_AddData(Buff_crc, acp_dataframe.IP_Head.rawdata, 11);
				Buffer_AppendData(Buff_crc, 0, acp_dataframe.IP_padding, length);

				if(acp_dataframe.IP_Head.param.Crc != Crc8_CalCcit(Buff_crc->data, Buff_crc->length))
				{
					Buffer_Dispose(Buff_crc);
					Buffer_Dispose(acp_buff);
					goto DROP;
				}
				Buffer_Dispose(Buff_crc);
			}
			else
			{
				if(acp_dataframe.IP_Head.param.Crc != Crc8_CalCcit(acp_dataframe.IP_Head.rawdata, 11))
				{
					Buffer_Dispose(acp_buff);
					goto DROP;
				}
			}
			Buffer_SplitData(acp_buff, pos, LENGTH_OF_HEAD_TCP + 1);
			Buffer_GetData(acp_buff, pos, acp_dataframe.TCP_Head.rawdata, &length);
			pos ++;
			if((acp_dataframe.IP_Head.param.TotalLength - acp_dataframe.IP_Head.param.IP_length) > LENGTH_OF_HEAD_TCP)
			{
				lengTCP_Padding = (acp_dataframe.IP_Head.param.TotalLength - acp_dataframe.IP_Head.param.IP_length) - LENGTH_OF_HEAD_TCP;
				acp_dataframe.TCP_padding = (uint8_t *)malloc(lengTCP_Padding);
				Buffer_GetData(acp_buff, pos, acp_dataframe.TCP_padding, &lengTCP_Padding);

				Buffer *Buff_crc = newBuffer();
				Buffer_AddData(Buff_crc, acp_dataframe.TCP_Head.rawdata, LENGTH_OF_HEAD_TCP - 1);
				Buffer_AppendData(Buff_crc, 0, acp_dataframe.TCP_padding, lengTCP_Padding);

				if(acp_dataframe.TCP_Head.param.Crc != Crc8_CalCcit(Buff_crc->data, Buff_crc->length))
				{
					Buffer_Dispose(Buff_crc);
					Buffer_Dispose(acp_buff);
					goto DROP;
				}
				Buffer_Dispose(Buff_crc);
			}
			else
			{
				if(acp_dataframe.TCP_Head.param.Crc != Crc8_CalCcit(acp_dataframe.TCP_Head.rawdata, 5))
				{
					Buffer_Dispose(acp_buff);
					goto DROP;
				}
			}

			if(acp_dataframe.IP_Head.param.IP_destAddr != acp_obj->ip_addr && acp_dataframe.IP_Head.param.IP_sourceAddr != acp_obj->ip_addr)
			{
				Buffer_Dispose(acp_buff);
				goto DROP;
			}

			Buffer_Dispose(acp_buff);

			/* tim connection */
			for(uint8_t count = 0; count < acp_obj->maxConn; count ++)
			{
				if((acp_dataframe.TCP_Head.param.Flag.byte & ACP_SYN) && (acp_dataframe.TCP_Head.param.DestPort == acp_obj->listPort[count]) && \
						acp_obj->ListConn[count].state == ACP_OPEN)
				{
					PosConn = count;
					acp_obj->ListConn[count].des.addr = acp_dataframe.IP_Head.param.IP_sourceAddr;
					acp_obj->ListConn[count].src.addr = acp_dataframe.IP_Head.param.IP_destAddr;
					acp_obj->ListConn[count].des.port = acp_dataframe.TCP_Head.param.SourcePort;
					acp_obj->ListConn[count].src.port = acp_dataframe.TCP_Head.param.DestPort;
					acp_obj->ListConn[count].AckNumber = acp_dataframe.TCP_Head.param.SeqNumber;
					acp_obj->ListConn[count].SeqNumber = 0;
					acp_obj->ListConn[count].state = ACP_RCVD;
					acp_obj->ListConn[count].TimeToLive = timetolive;
					Acp_Listen(acp_obj, acp_obj->listPort[count]);
					goto SEND_SYNACK;
				}

				else if(acp_obj->ListConn[count].des.addr == acp_dataframe.IP_Head.param.IP_sourceAddr && \
				   acp_obj->ListConn[count].src.addr == acp_dataframe.IP_Head.param.IP_destAddr && \
				   acp_obj->ListConn[count].des.port == acp_dataframe.TCP_Head.param.SourcePort && \
				   acp_obj->ListConn[count].src.port == acp_dataframe.TCP_Head.param.DestPort)
				{
					PosConn = count;

					if((acp_obj->ListConn[count].AckNumber + 1 == acp_dataframe.TCP_Head.param.SeqNumber))
					{
						acp_obj->ListConn[count].AckNumber = acp_dataframe.TCP_Head.param.SeqNumber;
					}
					else if((acp_dataframe.TCP_Head.param.Flag.byte & ACP_SYN) != ACP_SYN)
					{
						goto SEND_SYN;
					}

//					acp_obj->ListConn[count].SeqNumber = acp_dataframe.TCP_Head.param.AckNumber + 1;
					// receive
					switch (acp_dataframe.TCP_Head.param.Flag.byte )
					{
						case ACP_SYN:
							if(acp_obj->ListConn[count].state == ACP_ESTABLISHED)
							{
								acp_obj->ListConn[count].AckNumber = acp_dataframe.TCP_Head.param.SeqNumber;
								acp_obj->ListConn[count].SeqNumber = acp_dataframe.TCP_Head.param.AckNumber;
							}
							goto SEND_ACK;

						case ACP_SYN | ACP_ACK:
							if(acp_obj->ListConn[count].state == ACP_SEND)
							{
								acp_obj->ListConn[count].state = ACP_ESTABLISHED;
								acp_obj->ListConn[count].rxBuff = newFifo(1);
								acp_obj->ListConn[count].txBuff = newFifo(1);
								goto SEND_ACK;
							}
							goto SEND_RST;

						case ACP_ACK:
							if(acp_obj->ListConn[count].state == ACP_RCVD)
							{
								acp_obj->ListConn[count].state = ACP_ESTABLISHED;
								if(acp_obj->Acp_connected != NULL)
								{
									acp_obj->Acp_connected(&acp_obj->ListConn[count], acp_obj->ListConn[count].src.port);
								}
								acp_obj->ListConn[count].rxBuff = newFifo(1);
								acp_obj->ListConn[count].txBuff = newFifo(1);
							}
							else if(acp_obj->ListConn[count].state == ACP_FIN_WAIT_1)
							{
								acp_obj->ListConn[count].state = ACP_FIN_WAIT_2;
							}
							else if(acp_obj->ListConn[count].state == ACP_LAST_ACK)
							{
								acp_obj->ListConn[count].state = ACP_CLOSE;
								if(acp_obj->Acp_disconnected != NULL)
								{
									acp_obj->Acp_disconnected(&acp_obj->ListConn[count], acp_obj->ListConn[count].src.port);
								}
								acp_obj->ListConn[count].des.addr = 0;
								acp_obj->ListConn[count].des.port = 0;
								acp_obj->ListConn[count].src.addr = 0;
								acp_obj->ListConn[count].src.port = 0;
								acp_obj->ListConn[count].AckNumber = 0;
								acp_obj->ListConn[count].SeqNumber = 0;
								Fifo_Dispose(acp_obj->ListConn[count].rxBuff);
								Fifo_Dispose(acp_obj->ListConn[count].txBuff);
							}
							// notify receive ack
							/*     */
							goto DROP;
						case ACP_PSH:
							if(acp_obj->ListConn[count].state == ACP_ESTABLISHED)
							{
								Fifo_AddData(acp_obj->ListConn[count].rxBuff, acp_dataframe.TCP_padding, lengTCP_Padding);
								goto SEND_ACK;
							}
							goto SEND_RST;
						case ACP_FIN:
							if(acp_obj->ListConn[count].state == ACP_ESTABLISHED)
							{
								acp_obj->ListConn[count].state = ACP_CLOSE_WAIT;
								acp_obj->ListConn[count].TimeToLive = timetolive;
								goto SEND_ACK;
							}
							else if(acp_obj->ListConn[count].state == ACP_FIN_WAIT_2)
							{
								acp_obj->ListConn[count].state = ACP_TIME_WAIT;
								goto SEND_ACK;
							}
						default :
							break;
					}
					break;
				}
			}
			goto DROP;


		SEND_SYNACK:
				acp_dataframe.TCP_Head.param.Flag.byte = 0;
				acp_dataframe.TCP_Head.param.Flag.bit.ACK = 1;
				acp_dataframe.TCP_Head.param.Flag.bit.SYN = 1;
				goto SEND;
		SEND_ACK:
				acp_dataframe.TCP_Head.param.Flag.byte = 0;
				acp_dataframe.TCP_Head.param.Flag.bit.ACK = 1;
				if(acp_dataframe.TCP_padding != NULL)
				{
					free(acp_dataframe.TCP_padding);
					acp_dataframe.TCP_padding = NULL;
					lengTCP_Padding = 0;
				}
				goto SEND;
		}
	}

	for(uint8_t count = 0; count < acp_obj->maxConn; count ++)
	{
		PosConn = count;
		if(acp_obj->listPort[count] != 0 && acp_obj->ListConn[count].state == ACP_CLOSE)
		{
			acp_obj->ListConn[count].state = ACP_OPEN;
		}
		if(acp_obj->ListConn[count].state == ACP_ESTABLISHED)
		{
			lengTCP_Padding = Fifo_GetSizeData(acp_obj->ListConn[count].txBuff);
			if(lengTCP_Padding > 0)
			{
				acp_dataframe.TCP_padding = (uint8_t *)malloc(lengTCP_Padding);
				if(Fifo_GetData(acp_obj->ListConn[count].txBuff, acp_dataframe.TCP_padding, &lengTCP_Padding) == BUFF_OK)
				{
					acp_dataframe.TCP_Head.param.Flag.bit.PSH = 1;
					goto SEND;
				}
			}
			else if(acp_obj->ListConn[count].Flag.bit.FIN == 1)
			{
				acp_obj->ListConn[count].state = ACP_FIN_WAIT_1;
				goto SEND_FIN;
			}
		}
		else if(acp_obj->ListConn[count].state == ACP_CLOSE && acp_obj->ListConn[count].Flag.bit.SYN == 1)
		{
			acp_obj->ListConn[count].state = ACP_SEND;
			goto SEND_SYN;
		}
		else if(acp_obj->ListConn[count].state == ACP_CLOSE_WAIT)
		{
			acp_obj->ListConn[count].state = ACP_LAST_ACK;
			goto SEND_FIN;
		}
		else if(acp_obj->ListConn[count].state == ACP_RCVD)
		{
			if(timetolive - acp_obj->ListConn[count].TimeToLive > TIME_OUT)
			{
				acp_obj->ListConn[count].state = ACP_OPEN;
			}
		}
		else if(acp_obj->ListConn[count].state == ACP_LAST_ACK)
		{
			if(timetolive - acp_obj->ListConn[count].TimeToLive > TIME_OUT)
			{
				acp_obj->ListConn[count].state = ACP_CLOSE;
				if(acp_obj->Acp_disconnected != NULL)
				{
					acp_obj->Acp_disconnected(&acp_obj->ListConn[count], acp_obj->ListConn[count].src.port);
				}
				acp_obj->ListConn[count].des.addr = 0;
				acp_obj->ListConn[count].des.port = 0;
				acp_obj->ListConn[count].src.addr = 0;
				acp_obj->ListConn[count].src.port = 0;
				acp_obj->ListConn[count].AckNumber = 0;
				acp_obj->ListConn[count].SeqNumber = 0;
				Fifo_Dispose(acp_obj->ListConn[count].rxBuff);
				Fifo_Dispose(acp_obj->ListConn[count].txBuff);
			}
		}
		else if(acp_obj->ListConn[count].state == ACP_TIME_WAIT)
		{
			acp_obj->ListConn[count].state = ACP_CLOSE;
			acp_obj->ListConn[count].AckNumber = 0;
			acp_obj->ListConn[count].SeqNumber = 0;
			acp_obj->ListConn[count].des.addr  = 0;
			acp_obj->ListConn[count].des.port  = 0;
			acp_obj->ListConn[count].src.addr  = 0;
			acp_obj->ListConn[count].src.port  = 0;
			Fifo_Dispose(acp_obj->ListConn[count].rxBuff);
			Fifo_Dispose(acp_obj->ListConn[count].txBuff);
		}

	}

	goto DROP;

	SEND_SYN:
			acp_dataframe.TCP_Head.param.Flag.byte = 0;
			acp_dataframe.TCP_Head.param.Flag.bit.SYN = 1;
			if(acp_dataframe.TCP_padding != NULL)
			{
				free(acp_dataframe.TCP_padding);
				acp_dataframe.TCP_padding = NULL;
				lengTCP_Padding = 0;
			}
			goto SEND;

	SEND_RST:
			acp_dataframe.TCP_Head.param.Flag.byte = 0;
			acp_dataframe.TCP_Head.param.Flag.bit.RST = 1;
			goto SEND;

	SEND_FIN:
			acp_dataframe.TCP_Head.param.Flag.byte = 0;
			acp_dataframe.TCP_Head.param.Flag.bit.FIN = 1;
			goto SEND;

SEND:

	acp_obj->ListConn[PosConn].SeqNumber += 1;
	acp_dataframe.IP_Head.param.IP_sourceAddr 	= acp_obj->ListConn[PosConn].src.addr;
	acp_dataframe.IP_Head.param.IP_destAddr   	= acp_obj->ListConn[PosConn].des.addr;
	acp_dataframe.TCP_Head.param.SourcePort 	= acp_obj->ListConn[PosConn].src.port;
	acp_dataframe.TCP_Head.param.DestPort 		= acp_obj->ListConn[PosConn].des.port;
	acp_dataframe.TCP_Head.param.AckNumber 		= acp_obj->ListConn[PosConn].AckNumber;
	acp_dataframe.TCP_Head.param.SeqNumber		= acp_obj->ListConn[PosConn].SeqNumber;

	acp_dataframe.IP_Head.param.Encode 			= ENCODE;
	acp_dataframe.IP_Head.param.Version 		= VERSION;
	acp_dataframe.IP_Head.param.Option 		 	= NO_OPTION_SELECT;
	acp_dataframe.IP_Head.param.TimeOflive 	 	= TIME_OF_LIVE;
	acp_dataframe.IP_Head.param.IP_length 	 	= LENGTH_OF_HEAD_IP;
	acp_dataframe.IP_Head.param.TotalLength 	= LENGTH_OF_HEAD_IP + LENGTH_OF_HEAD_TCP + lengTCP_Padding;
	acp_dataframe.IP_padding  				 	= NULL;

	acp_dataframe.IP_Head.param.Crc = Crc8_CalCcit(acp_dataframe.IP_Head.rawdata, LENGTH_OF_HEAD_IP - 1);

		Buffer *BuffRespond = newBuffer();
		Buffer_AddData(BuffRespond, acp_dataframe.IP_Head.rawdata, LENGTH_OF_HEAD_IP);
		if(acp_dataframe.TCP_padding != NULL)
		{
			Buffer *Buffcrc = newBuffer();
			Buffer_AddData(Buffcrc, acp_dataframe.TCP_Head.rawdata, LENGTH_OF_HEAD_TCP - 1);
			Buffer_AppendData(Buffcrc,0, acp_dataframe.TCP_padding, lengTCP_Padding);
			acp_dataframe.TCP_Head.param.Crc = Crc8_CalCcit(Buffcrc->data, Buffcrc->length);
			Buffer_Dispose(Buffcrc);

			Buffer_AppendData(BuffRespond, 0, acp_dataframe.TCP_Head.rawdata, LENGTH_OF_HEAD_TCP);
			Buffer_AppendData(BuffRespond, 0, acp_dataframe.TCP_padding, lengTCP_Padding);
		}
		else
		{
			acp_dataframe.TCP_Head.param.Crc = Crc8_CalCcit(acp_dataframe.TCP_Head.rawdata, LENGTH_OF_HEAD_TCP - 1);
			Buffer_AppendData(BuffRespond, 0, acp_dataframe.TCP_Head.rawdata, LENGTH_OF_HEAD_TCP);
		}

		Fifo_AddData(acp_obj->fifo_send, BuffRespond->data, BuffRespond->length);

		Buffer_Dispose(BuffRespond);

DROP:

		if( ++ countTime == countTimecycle)
		{
			countTime = 0;
			if( ++ timetolive == 256)
			{
				timetolive = 0;
			}
		}
		if(data != NULL)
		{
			free(data);
			data = NULL;
		}
		if(acp_dataframe.IP_padding != NULL)
		{
			free(acp_dataframe.IP_padding);
			acp_dataframe.IP_padding = NULL;
		}
		if(acp_dataframe.IP_padding != NULL)
		{
			free(acp_dataframe.TCP_padding);
			acp_dataframe.TCP_padding = NULL;
		}
		return;
}

Acp_ReturnType Acp_getConn(acp *acp_obj, uint8_t port, acp_conn *conn)
{
	acp_conn *_conn = conn;
	uint8_t CountConn = 0;
	for(uint8_t count = 0; count < acp_obj->maxConn; count ++)
	{
		if(acp_obj->ListConn[count].state == ACP_ESTABLISHED && acp_obj->ListConn[count].src.port == port)
		{
			_conn[CountConn] = acp_obj->ListConn[count];
			CountConn ++;
		}
	}
	return CountConn;
}

void Acp_EventEstablish(acp *acp_obj, acp_notify_connected notify)
{
	acp_obj->Acp_connected = notify;
}

void Acp_EventDisconnected(acp *acp_obj, acp_notify_disconnected notify)
{

	acp_obj->Acp_disconnected = notify;
}

