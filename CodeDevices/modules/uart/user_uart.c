#include "user_uart.h"


UartFunctCallBack_t UartRecieveByte = NULL;
void uart_portInit(uart_cf_t UartConFig);
void uart_writebyte(Modul_uart_t UartNumb, uint8_t byte);


USART_TypeDef *USART[6] = {USART1, USART2, USART3};
uint8_t Psc_Uart[3] = {1,2,2};

const volatile uart_SR_t *UART_SR[3];
uart_CR1_t	USART_CR1[3] 	= {0};
uart_CR2_t 	USART_CR2[3] 	= {0};
uart_BBR_t 	USART_BBR[3] 	= {0};

void uart_init(uart_cf_t UartConFig)
{
	uart_portInit(UartConFig);

	UART_SR[UartConFig.ModulUart] = (uart_SR_t *)&USART[UartConFig.ModulUart]->SR;

	USART_BBR[UartConFig.ModulUart].bit.DIV_mantisa 	= (UART_CLK / Psc_Uart[UartConFig.ModulUart]) / (UartConFig.baudrate * 16);
	USART_BBR[UartConFig.ModulUart].bit.DIV_fraction = (uint32_t)(UART_CLK % (UartConFig.baudrate * 16)) / UartConFig.baudrate;
	USART[UartConFig.ModulUart]->BRR = USART_BBR[UartConFig.ModulUart].byte;

	USART_CR2[UartConFig.ModulUart].bit.STOP = UartConFig.StopBit;
	USART[UartConFig.ModulUart]->CR2 = USART_CR2[UartConFig.ModulUart].Byte;

	USART_CR1[UartConFig.ModulUart].bit.RE 	= ENABLE;
	USART_CR1[UartConFig.ModulUart].bit.TE 	= ENABLE;
	USART_CR1[UartConFig.ModulUart].bit.UE 	= ENABLE;

	USART_CR1[UartConFig.ModulUart].bit.IDLEIE 	= ENABLE;
	USART_CR1[UartConFig.ModulUart].bit.RXNEIE 	= UartConFig.INT_mode;
	USART_CR1[UartConFig.ModulUart].bit.M 		= UartConFig.WordLength;
	USART_CR1[UartConFig.ModulUart].bit.PCE 	= UartConFig.Parity.PC;
	USART_CR1[UartConFig.ModulUart].bit.PS 		= UartConFig.Parity.PS;

	USART[UartConFig.ModulUart]->CR1 = USART_CR1[UartConFig.ModulUart].Byte;
}


void uart_WriteData(Modul_uart_t UartNumb,uint8_t *data, uint16_t length)
{

	uint8_t byte_rec = 0;
//	UART_SR[UartConFig.ModulUart]->Byte = USART[UartNumb]->SR;

	if(UART_SR[UartNumb]->bit.ORE == 1)				// if(USART1_SR.bit.ORE == 1)
	{
		byte_rec = USART[UartNumb]->DR;
		byte_rec = USART[UartNumb]->DR;
		USART[UartNumb]->SR   =   0x00;
	}
	for(uint8_t count = 0; count < length; count ++)
	{
		uart_writebyte(UartNumb, data[count]);
	}
//    while((USART[UartNumb]->SR & USART_STATR_TC)!= USART_STATR_TC){}
	while(UART_SR[UartNumb]->bit.TC == 0){}
}


void uart_recive_add_callback(UartFunctCallBack_t CallBack)
{
	UartRecieveByte = CallBack;
}




void uart_writebyte(Modul_uart_t UartNumb, uint8_t byte)
{
//    while((USART[UartNumb]->SR & USART_STATR_TXE)!= USART_STATR_TXE){}

	 while(UART_SR[UartNumb]->bit.TXE == 0){}
    USART[UartNumb]->DR = byte;
}

void uart_portInit(uart_cf_t UartConFig)
{
	  GPIO_InitTypeDef GPIO_InitStruct = {0};

	    /* Peripheral clock enable */
	  switch(UartConFig.ModulUart)
	  {
	  case 0:
	    __HAL_RCC_USART1_CLK_ENABLE();
	    __HAL_RCC_GPIOA_CLK_ENABLE();

	    GPIO_InitStruct.Pin = GPIO_PIN_9;
	    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	    GPIO_InitStruct.Pin = GPIO_PIN_10;
	    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	    GPIO_InitStruct.Pull = GPIO_NOPULL;
	    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	    HAL_NVIC_SetPriority(USART1_IRQn, 1, 0);
	    HAL_NVIC_EnableIRQ(USART1_IRQn);
	    break;
	  case 1:
		__HAL_RCC_USART2_CLK_ENABLE();
		__HAL_RCC_GPIOA_CLK_ENABLE();

	    GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
	    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	    GPIO_InitStruct.Pull = GPIO_NOPULL;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	    HAL_NVIC_SetPriority(USART2_IRQn, 1, 0);
	    HAL_NVIC_EnableIRQ(USART2_IRQn);
		break;
	  case 2:
		__HAL_RCC_USART3_CLK_ENABLE();
		__HAL_RCC_GPIOB_CLK_ENABLE();

	    GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11;
	    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	    GPIO_InitStruct.Pull = GPIO_NOPULL;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	    HAL_NVIC_SetPriority(USART3_IRQn, 1, 0);
	    HAL_NVIC_EnableIRQ(USART3_IRQn);
		break;
	  default :
		  break;
	  }

}

void USART1_IRQHandler(void)
{
	uint8_t data = 0;
	uart_SR_t  USART1_SR;
	USART1_SR.Byte = USART[0]->SR;
	if(USART1_SR.bit.RXNE == 1)
	{
		data = USART[0]->DR & 0x00FF;
	    if(UartRecieveByte != NULL)
	    {
	        UartRecieveByte(data);
	    }
	}
    else if(USART1_SR.bit.ORE == 1)
    {
    	data = USART[0]->DR;
    	data = USART[0]->DR;
    	USART[0]->SR   =   0x00;
    }
    else if(USART1_SR.bit.IDLE == 1)
    {
    	data = USART[0]->DR;
    }
}

void USART2_IRQHandler(void)
{
	uint8_t data = 0;
	uart_SR_t  USART1_SR;
	USART1_SR.Byte = USART[1]->SR;
	if(USART1_SR.bit.RXNE == 1)
	{
		data = USART[1]->DR & 0x00FF;
	    if(UartRecieveByte != NULL)
	    {
	        UartRecieveByte(data);
	    }
	}
    else if(USART1_SR.bit.ORE == 1)
    {
    	data = USART[1]->DR;
    	data = USART[1]->DR;
    	USART[1]->SR   =   0x00;
    }
    else if(USART1_SR.bit.IDLE == 1)
    {
    	data = USART[1]->DR;
    }
}

void USART3_IRQHandler(void)
{
	uint8_t data = 0;
	uart_SR_t  USART1_SR;
	USART1_SR.Byte = USART[2]->SR;
	if(USART1_SR.bit.RXNE == 1)
	{
		data = USART[2]->DR & 0x00FF;
	    if(UartRecieveByte != NULL)
	    {
	        UartRecieveByte(data);
	    }
	}
    else if(USART1_SR.bit.ORE == 1)
    {
    	data = USART[2]->DR;
    	data = USART[2]->DR;
    	USART[2]->SR   =   0x00;
    }
    else if(USART1_SR.bit.IDLE == 1)
    {
    	data = USART[2]->DR;
    }
}

