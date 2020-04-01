/*
 * hardware.h
 *
 *  Created on: 3 de ene. de 2016
 *      Author: Eugenia
 */

#ifndef INC_HARDWARE_H_
#define INC_HARDWARE_H_

/* LPCOpen */
#include "board.h"
#include "chip.h"

#include "gpio.h"
#include "uart2.h"

/* Defines */

//ADC
#define	ADC_Port			0
#define ADC_Pin_Luz			25
#define ADC_Pin_Temp		26
#define ADC_Luz				ADC_CH2
#define ADC_Temp			ADC_CH3
#define LPC_ADC_ID			LPC_ADC
#define ADC_Freq			100
#define ADC_LENGHT			5

//ADC - Preferencias
#define LUZ_MIN		500			//Menos de ésta cantidad de cuentas -> Hay mucha luz
#define LUZ_MAX		1500		//Más de ésta cantidad de cuentes	-> Hay poca luz
#define TEMP_MIN	20			//Menos de ésta temperatura			-> Frío
#define TEMP_MAX	25			//Más de éstá temperatura			-> Caluroso

//Estados Maquinas de Temp y Luz
#define AMBIENT	0				//Temp Ambiente
#define COOL	1				//Temp Fría
#define HEAT	2				//Temp Caluroso

#define NATURAL	3				//Luz Natural
#define LIGHT	4				//Luz On
#define OSCURA	5				//Luz Oscuridad
#define MIX		6				//Luz Natural + Luz On

#define ERROR	7
//Estados Máquina de Puerta
#define EMPTY	0				//No hay nadie - Estado Idle
#define LOGIN	1				//Espera a que pase el que se logueó
#define LOGOUT	2				//Cierro y doy semáforos
#define ONE_IN	3

//Defines para establecer el estado de la puerta y persiana
#define OPEN	0
#define CLOSED	1
#define UP		2
#define DOWN	3

/* Variables - Estructuras */
static ADC_CLOCK_SETUP_T ADCSetup;

/* Prototipos */
void Init_ADC(void);
void Init_GPIOs(void);
void Init_RTC(void);

#endif /* INC_HARDWARE_H_ */
