/*
 * uart2.h
 *
 *  Created on: 4 de ene. de 2016
 *      Author: Eugenia
 */

#ifndef INC_UART2_H_
#define INC_UART2_H_

#include "chip.h"
#include "board.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#define SI 	1
#define NO 	0

#define CANT_QUEUE_SEND		100
#define CANT_QUEUE_RECEIVE	100

//UART 2
#define PCUART2_POWERON		(1<<24)
#define PCLK_UART2			16
#define PCLK_UART2_MASK		(3<<PCLK_UART2)

#define BitsTX_5		((unsigned int)0)
#define BitsTX_6		((unsigned int)1)
#define BitsTX_7		((unsigned int)2)
#define BitsTX_8		((unsigned int)3)

#define BitsStop1		((unsigned int)0)
#define BitsStop2		((unsigned int)1<<2)

#define ParidadEn		((unsigned int)1<<3)
#define ParidadImpar	((unsigned int)0<<4)
#define ParidadPar		((unsigned int)1<<5)
#define ParidadForz1	((unsigned int)2<<5)
#define ParidadForz0	((unsigned int)3<<5)

#define BreakControl	((unsigned int)1<<6)
#define DLAB			((unsigned int)1<<7)


//Colas
extern xQueueHandle xQueueUART2_Send;
extern xQueueHandle xQueueUART2_Receive;

//Semáforos
extern xSemaphoreHandle xSemaphoreUART2;

/* Prototipos */
void UART2_Init(uint32_t);
portBASE_TYPE UART2_Sendchar(uint8_t, portTickType);
portBASE_TYPE UART2_Getchar(uint8_t*, portTickType);
portBASE_TYPE UART2_Send_String(uint8_t*, uint32_t, portTickType);
portBASE_TYPE UART2_Receive_String(uint8_t*);

#endif /* INC_UART2_H_ */
