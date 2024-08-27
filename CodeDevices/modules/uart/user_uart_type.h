#ifndef __USER_UART_TYPE_H__
#define __USER_UART_TYPE_H__

#ifdef __cplusplus
extern "C"{
#endif

#include "main.h"

typedef void (*UartFunctCallBack_t)(uint8_t data);

typedef union
{
	struct
	{
		uint32_t CBK 		: 1;  	// bit0
		uint32_t RWU		: 1;	// bit1
		uint32_t RE			: 1; 	// bit2
		uint32_t TE 		: 1;  	// bit3
		uint32_t IDLEIE		: 1;	// bit4
		uint32_t RXNEIE		: 1; 	// bit5
		uint32_t TCIE 		: 1;  	// bit6
		uint32_t TXEIE		: 1;	// bit7
		uint32_t PEIE		: 1; 	// bit8
		uint32_t PS 		: 1;  	// bit9
		uint32_t PCE		: 1;	// bit10
		uint32_t WAKE		: 1; 	// bit11
		uint32_t M 			: 1;  	// bit12
		uint32_t UE			: 1;	// bit13
		uint32_t Reserved1	: 18; 	// bit13 - 31
	}bit;
	uint32_t Byte;
}uart_CR1_t;

typedef union
{
	struct
	{
		uint32_t DIV_fraction 		: 4;  	// bit0
		uint32_t DIV_mantisa		: 12;	// bit1
		uint32_t Reserved			: 16; 	// bit2
	}bit;
	uint32_t byte;
}uart_BBR_t;

typedef union
{
	struct
	{
		uint32_t ADD 		: 4;  	// bit0-3
		uint32_t Reserved1	: 1;	// bit4
		uint32_t LBDL		: 1; 	// bit5
		uint32_t LBDIE 		: 1;  	// bit6
		uint32_t Reserved2	: 1;	// bit7
		uint32_t LBCL		: 1; 	// bit8
		uint32_t CPHA 		: 1;  	// bit9
		uint32_t CPOL		: 1;	// bit10
		uint32_t CLKEN		: 1; 	// bit11
		uint32_t STOP 		: 2;  	// bit12-13
		uint32_t LINEN		: 1;	// bit14
		uint32_t Reserved3	: 16; 	// bit15-31

	}bit;
	uint32_t Byte;
}uart_CR2_t;

typedef union
{
	struct
	{
		uint32_t PE 		: 1;  	// bit0
		uint32_t FE			: 1;	// bit1
		uint32_t NF			: 1; 	// bit2
		uint32_t ORE 		: 1;  	// bit3
		uint32_t IDLE		: 1;	// bit4
		uint32_t RXNE		: 1; 	// bit5
		uint32_t TC 		: 1;  	// bit6
		uint32_t TXE		: 1;	// bit7
		uint32_t LBD		: 1; 	// bit8
		uint32_t CTS 		: 1;  	// bit9
		uint32_t Reserved3	: 22; 	// bit10-31

	}bit;
	uint32_t Byte;
}uart_SR_t;

typedef union
{
	struct
	{
		uint32_t EIE 		: 1;  	// bit0
		uint32_t IREN		: 1;	// bit1
		uint32_t IRLP		: 1; 	// bit2
		uint32_t HDSEL 		: 1;  	// bit3
		uint32_t NACK		: 1;	// bit4
		uint32_t SCEN		: 1; 	// bit5
		uint32_t DMAR 		: 1;  	// bit6
		uint32_t DMAT		: 1;	// bit7
		uint32_t RTSE		: 1; 	// bit8
		uint32_t CTSE 		: 2;  	// bit9
		uint32_t CTSIE		: 1;	// bit10
		uint32_t ONEBIT		: 1;    // bit11
		uint32_t Reserved3	: 20; 	// bit12-31

	}bit;
	uint32_t Byte;
}uart_CR3_t;

typedef enum
{
	MODE_8BIT,
	MODE_9BIT,
}WordLength_t;

typedef struct
{
	enum{
		PARITY_DISABLE,
		PARITY_ENABLE,
	}PC;
	enum{
		PARITY_EVEN,
		PARITY_ODD,
	}PS;
}Parity_t;

typedef enum
{
	MODE_1BIT,
	MODE_2BIT,
}StopBit_t;

typedef enum
{
	INT_DISABLE,
	INT_ENABLE,
}Interrupt_t;

typedef enum
{
	USART_1,
	USART_2,
	USART_3,
}Modul_uart_t;

typedef struct
{
	Modul_uart_t	ModulUart;
	uint32_t 		baudrate;
	WordLength_t  	WordLength;
	Parity_t  		Parity;
	StopBit_t  		StopBit;
	Interrupt_t		INT_mode;
}uart_cf_t;

#ifdef __cplusplus
}
#endif

#endif
