#include "mainApp.h"

void Event_SendMessage(lcp_conn_t * Conn, uint8_t port);
void uart_readbyte(uint8_t byte);
void RTC_enable_int(void);

typedef void(*mode_funct_t)(void);

mode_funct_t mode_funct[2]  = { Fullpower_session, Sleep_session};

uint8_t session_run = 0;

Router *_router = NULL;
lcp_t  *_lcp_cl = NULL;

lcp_conn_t *conn = NULL;

uint8_t data[10] = {1,2,3,4,5,6,7,8,9};
uint8_t flag_read_SS_done = 0;

void mainApp(void)
{
	_router = newRouter(3, 40, 30);
	Router_SlipConfig(_router, true);
	uart_cf_t uartcf;
	uartcf.ModulUart 	= USART_1;
	uartcf.baudrate  	= 9600;
	uartcf.WordLength 	= MODE_8BIT;
	uartcf.Parity.PC    = PARITY_DISABLE;
	uartcf.StopBit   	= MODE_1BIT;
	uartcf.INT_mode		= INT_ENABLE;

	uart_init(uartcf);

	uart_recive_add_callback(uart_readbyte);

	_lcp_cl	= newLcp(0x15, 1, 1, 40, 0);

	Lcp_EventSendMessage(_lcp_cl, Event_SendMessage);

	HAL_TIM_Base_Start_IT(&htim1);
	HAL_TIM_Base_Start_IT(&htim2);
	HAL_TIM_Base_Start_IT(&htim3);

	conn = Lcp_Connect(_lcp_cl, (point){0x21});

	RTC_enable_int();

	uint8_t datarx[15] = {0};
	uint16_t length = 0;
	while(1)
	{
		if(conn == NULL || conn->state == CLOSE)
		{
			conn = Lcp_Connect(_lcp_cl, (point){0x21});
		}

		mode_funct[session_run]();
	}
}

void Event_SendMessage(lcp_conn_t * Conn, uint8_t port)
{

	if(conn != NULL)
	{
		Fifo_AddData(Conn->txBuff, value.rawdata, 10);
	}

}


void uart_readbyte(uint8_t byte)
{
	Router_PushMessageRx(_router, &byte, 1);
}

void HAL_RTCEx_RTCEventCallback(RTC_HandleTypeDef *hrtc)
{
	if(_lcp_cl->ListConn->state == ESTABLISHED)
	{
		if((_lcp_cl->ListConn->timeCircle > 0))
		{
			if(_lcp_cl->ListConn->timeOffset == 0)
			{
				if((_lcp_cl->realTime % _lcp_cl->ListConn->timeCircle) == 9)
				{
					session_run = 1;
					_lcp_cl->ListConn->Flag.bit.PSH = 1;
				}
			}
			else
			{
				if((_lcp_cl->realTime % _lcp_cl->ListConn->timeCircle) == (_lcp_cl->ListConn->timeOffset - 1))
				{
					session_run = 1;
					_lcp_cl->ListConn->Flag.bit.PSH = 1;
				}
			}
		}
		Lcp_Runtime(_lcp_cl, 1000);
	}
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	uint8_t data[50] = {0};
	uint16_t length = 0;
	if(htim->Instance == TIM1)
	{
		Lcp_nProcess(_lcp_cl, 3);
		Router_MessageHandler(_router);
		if(Router_PullMessageRx(_router, data, &length) == BUFF_OK)
		{
			Lcp_pushMessage(_lcp_cl, data, length);
		}
	}

	if(htim->Instance == TIM2)
	{
		if(Lcp_pullMessageSend(_lcp_cl , data, &length) == LCP_OK)
		{
			Router_PushMessageTx(_router, data, length);
		}
		if(Router_PullMessageTx(_router, data, &length) == BUFF_OK)
		{
			uart_WriteData(USART_1, data, length);
		}
	}

	if(htim->Instance == TIM3)
	{
//		Lcp_Runtime(_lcp_cl, 1000);
//		Lcp_Runtime(_lcp, 1000);
	}
}

void RTC_enable_int(void)
{
	uint32_t *rtc = 0x40002800;
	*rtc = 0x01;
}

