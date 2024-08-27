#ifndef __USER_UART_H__
#define __USER_UART_H__

#ifdef __cplusplus
extern "C"{
#endif

#include "main.h"
#include "user_uart_type.h"

#define UART_CLK			64000000
#define ENABLE				0x01
#define BISBLE				0x00
#define USART_STATR_TXE 	0x0080
#define USART_STATR_TC      0x0040

void uart_init(uart_cf_t UartConFig);
void uart_writebyte(Modul_uart_t UartNumb, uint8_t byte);
void uart_WriteData(Modul_uart_t UartNumb, uint8_t *data, uint16_t length);
void uart_recive_add_callback(UartFunctCallBack_t CallBack);


#ifdef __cplusplus
}
#endif

#endif
