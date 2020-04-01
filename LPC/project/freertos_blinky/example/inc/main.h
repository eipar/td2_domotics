/*
 * main.h
 *
 *  Created on: 31 de dic. de 2015
 *      Author: Eugenia
 */

#ifndef INC_MAIN_H_
#define INC_MAIN_H_

/* LPCOpen */
#include "board.h"
#include "chip.h"

/* FreeRTOS */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* Propias */
#include "uart3.h"
#include "uart2.h"
#include "fs.h"
#include "hardware.h"
#include "lcd.h"		//Prestada

/* SD */
#include "ff.h"
#include "diskio.h"
#include "sd.h"
#include <stdio.h>		//Por el sprintf
#include <string.h>		//Por el strlen

/** Defines **/
#define tskACTION_priority tskIDLE_PRIORITY+1
#define tskSTATE_priority  tskIDLE_PRIORITY+2
#define tskSENSOR_priority tskIDLE_PRIORITY+3
#define tskFP_priority	   tskIDLE_PRIORITY+3

#define SECURITY_FACTOR		2

#define	ON		1
#define OFF		0

/** Variables - Estructuras **/

typedef struct{
	uint32_t id;					//ID del usuario
	uint16_t luz;
	uint16_t temp;
}Info_Amb;

Info_Amb Estado_Actual;				//Sensores
Info_Amb Estado_Usuario;			//Info del Usuario
Info_Amb Estado_Sistema;			//Máq de Estados

uint8_t State_Lock = EMPTY;

uint8_t State_Blind = DOWN;			//Estado Persiana
uint8_t State_Door = CLOSED;		//Estado Puerta

uint8_t flag_ATRODEN = OFF;			//Alguien está adentro
uint8_t flag_ACTION = OFF;			//Habilito la maq de estados
uint8_t flag_SD = OFF;				//Flag para guardar en la SD

/** Colas **/
xQueueHandle qADC_Luz;				//Cola dónde guardo la info leída del ADC
xQueueHandle lectorHD;				//Semáforo para activar el logueo de alguien por el lector de HD
xQueueHandle out;					//Semáforo para activar la salida de alguien
xQueueHandle enroll;				//Semáforo para activar el enroll

/* Prototipos */
uint8_t Mover_Puerta(void);

#endif /* INC_MAIN_H_ */
