/*
 * uart2.c
 *
 *  Created on: 4 de ene. de 2016
 *      Author: Eugenia
 */

#include "uart2.h"

char TransmitiendoUART2 = NO;

xQueueHandle xQueueUART2_Send = NULL;
xQueueHandle xQueueUART2_Receive = NULL;
xSemaphoreHandle xSemaphoreUART2 = NULL;

/* Inicialización UART2 */
void UART2_Init(uint32_t baudrate){
	int pclk;
	unsigned long int Fdiv;

	//PCLK_UART3 es 1/4 del SystemCoreClok
	pclk = SystemCoreClock/4;

	//Prendo UART3
	LPC_SYSCTL->PCONP |= PCUART2_POWERON;

	//Prendo UART3 Peripheral Clock
	LPC_SYSCTL->PCLKSEL[0] &= ~(PCLK_UART2_MASK);
	LPC_SYSCTL->PCLKSEL[0] |= (0 << PCLK_UART2);	// PCLK_periph = CCLK/4

	//Seteo el P0.10 como TXD2 y P0.11 como RXD2
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 10, IOCON_FUNC1);
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 11, IOCON_FUNC1);
	//LPC_IOCON->PINSEL[0] &= ~0x0F;					//Revisar
	//LPC_IOCON->PINSEL[0] |= ((2 << 2) | (2 << 0));	//Revisar

	//8 bits, sin Paridad, 1 Stop Bit, DLAB=1
	LPC_UART2->LCR = BitsTX_8|BitsStop1|DLAB;

	Fdiv = (pclk/16)/baudrate;

	LPC_UART2->DLM = Fdiv/256;
	LPC_UART2->DLL = Fdiv%256;
	LPC_UART2->LCR &= (~DLAB);	//8 bits, sin Paridad, 1 Stop Bit, DLAB = 0

	/* Creo las colas que se usará para transmitir y recibir */
	xQueueUART2_Send = xQueueCreate(CANT_QUEUE_SEND*sizeof(unsigned char), sizeof(unsigned char));
	xQueueUART2_Receive = xQueueCreate(CANT_QUEUE_RECEIVE*sizeof(unsigned char), sizeof(unsigned char));

	/* Creo un semáforo por si quiero reservar el recurso */
	vSemaphoreCreateBinary(xSemaphoreUART2);

	NVIC_EnableIRQ(UART2_IRQn);

	LPC_UART2->IER = UART_IER_RBRINT | UART_IER_THREINT | UART_IER_RLSINT;

	LPC_UART2->TER1 = 0X80;	//eNABLE tRANSMITTER

}

/* Función para enviar char por la UART3 */
portBASE_TYPE UART2_Sendchar(uint8_t c, portTickType Ticks_Espera){
	uint8_t Dummy = 0;
	portBASE_TYPE Encolado = pdFALSE;

	Encolado = xQueueSend(xQueueUART2_Send, &c, (portTickType)Ticks_Espera);

	if(Encolado == pdFAIL) return(Encolado);

	if(TransmitiendoUART2 == NO){
		Encolado = xQueueReceive(xQueueUART2_Send, &Dummy, (portTickType)Ticks_Espera);
		TransmitiendoUART2 = SI;
		LPC_UART2->THR = Dummy;
	}

	return(Encolado);
}

/* Función para recibir char por la UART3 */
portBASE_TYPE UART2_Getchar(uint8_t *c, portTickType Ticks_Espera){
	portBASE_TYPE Encolado = pdFALSE;
	uint8_t Dummy = 0;

	Encolado = xQueueReceive(xQueueUART2_Receive, &Dummy, Ticks_Espera);

	if(Encolado == pdTRUE)
		(*c) = Dummy;
	else
		(*c) = 0;

	return(Encolado);
}

/* Funcion para enviar buffer por la UART3 */
portBASE_TYPE UART2_Send_String(uint8_t *Cadena, uint32_t size, portTickType Tiempo){
	uint32_t i = 0;
	portBASE_TYPE Encolado = pdFALSE;

	if(size){
		for(i=0; i<size; i++)
			Encolado = UART2_Sendchar(Cadena[i], (uint32_t)Tiempo);
	}else{
		for(i=0; (Cadena[i] != 0); i++)
			Encolado = UART2_Sendchar(Cadena[i], (uint32_t)Tiempo);
	}

	return(Encolado);
}

/* Funcion para recibir buffer por la UART3 */
portBASE_TYPE UART2_Receive_String(uint8_t *Cadena){
	uint32_t i = 0;
	portBASE_TYPE Encolado = pdFALSE;

	do{
		Encolado = UART2_Getchar(&(Cadena[i]), portMAX_DELAY);
		i++;
	//}while(Cadena[i-1] != '\0');
	}while(i<12);

	return(Encolado);
}

/** Interrupción UART3 **/
void UART2_IRQHandler(void){
	uint8_t IIRValue, LSRValue;
	uint8_t Dummy = 0;
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	portBASE_TYPE Encolado;

	IIRValue = LPC_UART2->IIR;

	//IIRValue >>= 1;			//Skip pending bit in IIR
	IIRValue &= 0x07;			//Interrupt ID

	switch(IIRValue & 0x07){
	case UART_IIR_INTID_RLS:		//Receive Line Status
		LSRValue = LPC_UART2->LSR;
		if(LSRValue & (UART_LSR_OE | UART_LSR_PE | UART_LSR_FE | UART_LSR_RXFE | UART_LSR_BI)){
			//Error. Leo LSR para limpiar la interrupción
			Dummy = LPC_UART2->RBR;
			return;
		}
		if(LSRValue & UART_LSR_RDR){	//Hay data
			LPC_UART2->RBR;
		}
		break;
	case UART_IIR_INTID_RDA: 		//Receive Data Available
		Dummy = LPC_UART2->RBR;
		Encolado = xQueueSendFromISR(xQueueUART2_Receive, &Dummy, &xHigherPriorityTaskWoken);
		break;
	case UART_IIR_INTID_CTI:		//Character Timeout
		break;
	case UART_IIR_INTID_THRE:		//Transmit holding register empty
		LSRValue = LPC_UART2->LSR;

		if(LSRValue & UART_LSR_THRE){
			if(uxQueueMessagesWaiting(xQueueUART2_Send)){
				xQueueReceiveFromISR(xQueueUART2_Send, &Dummy, &xHigherPriorityTaskWoken);
				LPC_UART2->THR = Dummy;
				TransmitiendoUART2 = SI;
			}else{
				TransmitiendoUART2 = NO;
			}
		}
		break;
	}
	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}
