/*
 * uart3.c
 *
 *  Created on: 4 de ene. de 2016
 *      Author: Eugenia
 */

#include "uart3.h"

char TransmitiendoUART3 = NO;

xQueueHandle xQueueUART3_Send = NULL;
xQueueHandle xQueueUART3_Receive = NULL;
xSemaphoreHandle xSemaphoreUART3 = NULL;

/* Inicialización UART3 */
void UART3_Init(uint32_t baudrate){
	int pclk;
	unsigned long int Fdiv;

	//PCLK_UART3 es 1/4 del SystemCoreClok
	pclk = SystemCoreClock/4;

	//Prendo UART3
	LPC_SYSCTL->PCONP |= PCUART3_POWERON;

	//Prendo UART3 Peripheral Clock
	LPC_SYSCTL->PCLKSEL[0] &= ~(PCLK_UART3_MASK);
	LPC_SYSCTL->PCLKSEL[0] |= (0 << PCLK_UART3);	// PCLK_periph = CCLK/4

	//Seteo el P0.0 como TXD3 y P0.1 como RXD3
	LPC_IOCON->PINSEL[0] &= ~0x0F;
	LPC_IOCON->PINSEL[0] |= ((2 << 2) | (2 << 0));

	//8 bits, sin Paridad, 1 Stop Bit, DLAB=1
	LPC_UART3->LCR = BitsTX_8|BitsStop1|DLAB;

	Fdiv = (pclk/16)/baudrate;

	LPC_UART3->DLM = Fdiv/256;
	LPC_UART3->DLL = Fdiv%256;
	LPC_UART3->LCR &= (~DLAB);	//8 bits, sin Paridad, 1 Stop Bit, DLAB = 0

	/* Creo las colas que se usará para transmitir y recibir */
	xQueueUART3_Send = xQueueCreate(CANT_QUEUE_SEND*sizeof(unsigned char), sizeof(unsigned char));
	xQueueUART3_Receive = xQueueCreate(CANT_QUEUE_RECEIVE*sizeof(unsigned char), sizeof(unsigned char));

	/* Creo un semáforo por si quiero reservar el recurso */
	vSemaphoreCreateBinary(xSemaphoreUART3);

	//NVIC_SetPriority(UART3_IRQn, 10);
	NVIC_EnableIRQ(UART3_IRQn);

	LPC_UART3->IER = UART_IER_RBRINT | UART_IER_THREINT | UART_IER_RLSINT;

	LPC_UART3->TER1 = 0X80;	//eNABLE tRANSMITTER

}

/* Función para enviar char por la UART3 */
portBASE_TYPE UART3_Sendchar(uint8_t c, portTickType Ticks_Espera){
	uint8_t Dummy = 0;
	portBASE_TYPE Encolado = pdFALSE;

	Encolado = xQueueSend(xQueueUART3_Send, &c, (portTickType)Ticks_Espera);

	if(Encolado == pdFAIL) return(Encolado);

	if(TransmitiendoUART3 == NO){
		Encolado = xQueueReceive(xQueueUART3_Send, &Dummy, (portTickType)Ticks_Espera);
		TransmitiendoUART3 = SI;
		LPC_UART3->THR = Dummy;
	}

	return(Encolado);
}

/* Función para recibir char por la UART3 */
portBASE_TYPE UART3_Getchar(uint8_t *c, portTickType Ticks_Espera){
	portBASE_TYPE Encolado = pdFALSE;
	uint8_t Dummy = 0;

	Encolado = xQueueReceive(xQueueUART3_Receive, &Dummy, Ticks_Espera);

	if(Encolado == pdTRUE)
		(*c) = Dummy;
	else
		(*c) = 0;

	return(Encolado);
}

/* Funcion para enviar buffer por la UART3 */
portBASE_TYPE UART3_Send_String(uint8_t *Cadena, uint32_t size, portTickType Tiempo){
	uint32_t i = 0;
	portBASE_TYPE Encolado = pdFALSE;

	if(size){
		for(i=0; i<size; i++)
			Encolado = UART3_Sendchar(Cadena[i], (uint32_t)Tiempo);
	}else{
		for(i=0; (Cadena[i] != 0); i++)
			Encolado = UART3_Sendchar(Cadena[i], (uint32_t)Tiempo);
	}

	return(Encolado);
}

/* Funcion para recibir buffer por la UART3 */
portBASE_TYPE UART3_Receive_String(uint8_t *Cadena){
	uint32_t i = 0;
	portBASE_TYPE Encolado = pdFALSE;

	do{
		Encolado = UART3_Getchar(&(Cadena[i]), portMAX_DELAY);
		i++;
	//}while(Cadena[i-1] != '\0');
	}while(i<12);

	return(Encolado);
}

/** Interrupción UART3 **/
void UART3_IRQHandler(void){
	uint8_t IIRValue, LSRValue;
	uint8_t Dummy = 0;
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	portBASE_TYPE Encolado;

	IIRValue = LPC_UART3->IIR;

	//IIRValue >>= 1;				//Skip pending bit in IIR
	IIRValue &= 0x07;			//Interrupt ID

	switch(IIRValue & 0x07){
	case UART_IIR_INTID_RLS:		//Receive Line Status
		LSRValue = LPC_UART3->LSR;
		if(LSRValue & (UART_LSR_OE | UART_LSR_PE | UART_LSR_FE | UART_LSR_RXFE | UART_LSR_BI)){
			//Error. Leo LSR para limpiar la interrupción
			Dummy = LPC_UART3->RBR;
			return;
		}
		if(LSRValue & UART_LSR_RDR){	//Hay data
			LPC_UART3->RBR;
		}
		break;
	case UART_IIR_INTID_RDA: 		//Receive Data Available
		Dummy = LPC_UART3->RBR;
		Encolado = xQueueSendFromISR(xQueueUART3_Receive, &Dummy, &xHigherPriorityTaskWoken);
		break;
	case UART_IIR_INTID_CTI:		//Character Timeout
		break;
	case UART_IIR_INTID_THRE:		//Transmit holding register empty
		LSRValue = LPC_UART3->LSR;

		if(LSRValue & UART_LSR_THRE){
			if(uxQueueMessagesWaiting(xQueueUART3_Send)){
				xQueueReceiveFromISR(xQueueUART3_Send, &Dummy, &xHigherPriorityTaskWoken);
				LPC_UART3->THR = Dummy;
				TransmitiendoUART3 = SI;
			}else{
				TransmitiendoUART3 = NO;
			}
		}
		break;
	}
	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}
