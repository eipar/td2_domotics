/*
 * gpio.h
 *
 *  Created on: 1 de ene. de 2016
 *      Author: Eugenia
 */

#ifndef INC_GPIO_H_
#define INC_GPIO_H_

/* Nota: Puerto, Pin */

//GPIO Motores
#define _GPIO_Pers_Izq		2,8
#define _GPIO_Pers_Der		2,10
#define _GPIO_Puerta_Izq	2,11
#define _GPIO_Puerta_Der	1,18

//Seteo On/Off de Motores
#define PERSIANA_CLOSE		Chip_GPIO_SetPinState(LPC_GPIO, _GPIO_Pers_Izq, FALSE); Chip_GPIO_SetPinState(LPC_GPIO, _GPIO_Pers_Der, TRUE);
#define PERSIANA_OPEN		Chip_GPIO_SetPinState(LPC_GPIO, _GPIO_Pers_Izq, TRUE); Chip_GPIO_SetPinState(LPC_GPIO, _GPIO_Pers_Der, FALSE);
#define PERSIANA_OFF		Chip_GPIO_SetPinState(LPC_GPIO, _GPIO_Pers_Izq, FALSE); Chip_GPIO_SetPinState(LPC_GPIO, _GPIO_Pers_Der, FALSE);
#define PUERTA_CLOSE		Chip_GPIO_SetPinState(LPC_GPIO, _GPIO_Puerta_Der, TRUE); Chip_GPIO_SetPinState(LPC_GPIO, _GPIO_Puerta_Izq, FALSE);
#define PUERTA_OPEN			Chip_GPIO_SetPinState(LPC_GPIO, _GPIO_Puerta_Izq, TRUE); Chip_GPIO_SetPinState(LPC_GPIO, _GPIO_Puerta_Der, FALSE);
#define PUERTA_OFF			Chip_GPIO_SetPinState(LPC_GPIO, _GPIO_Puerta_Izq, FALSE); Chip_GPIO_SetPinState(LPC_GPIO, _GPIO_Puerta_Der, FALSE);

//GPIO Fines de Carrera
#define _GPIO_FdC_1			2,7
#define _GPIO_FdC_2			2,6
#define _GPIO_FdC_3			2,5
#define _GPIO_FdC_4			2,4

//GPIO Relés
#define _GPIO_Rele_Luz		2,0
#define _GPIO_Rele_Vent		2,1
#define _GPIO_Rele_Calef	2,2
#define _GPIO_Rele_Extra	2,3

//Seteo On/Off de Relés
#define LUZ_ON				Chip_GPIO_SetPinState(LPC_GPIO, _GPIO_Rele_Luz, TRUE);
#define LUZ_OFF				Chip_GPIO_SetPinState(LPC_GPIO, _GPIO_Rele_Luz, FALSE);
#define VENT_ON				Chip_GPIO_SetPinState(LPC_GPIO, _GPIO_Rele_Vent, TRUE);
#define VENT_OFF			Chip_GPIO_SetPinState(LPC_GPIO, _GPIO_Rele_Vent, FALSE);
#define CALEF_ON			Chip_GPIO_SetPinState(LPC_GPIO, _GPIO_Rele_Calef, TRUE);
#define CALEF_OFF			Chip_GPIO_SetPinState(LPC_GPIO, _GPIO_Rele_Calef, FALSE);

//GPIO Buzzer y LEDs
#define _GPIO_Buzzer		0,24
#define _GPIO_Led_Rojo		0,16
#define _GPIO_Led_Verde		0,23

//Seteo On/Off de Buzzer y LEDs
#define BUZZER_ON			Chip_GPIO_SetPinState(LPC_GPIO, _GPIO_Buzzer, TRUE);
#define BUZZER_OFF			Chip_GPIO_SetPinState(LPC_GPIO, _GPIO_Buzzer, FALSE);
#define LED_ROJO_ON			Chip_GPIO_SetPinState(LPC_GPIO, _GPIO_Led_Rojo, TRUE);
#define LED_ROJO_OFF		Chip_GPIO_SetPinState(LPC_GPIO, _GPIO_Led_Rojo, FALSE);
#define LED_VERDE_ON		Chip_GPIO_SetPinState(LPC_GPIO, _GPIO_Led_Verde, TRUE);
#define LED_VERDE_OFF		Chip_GPIO_SetPinState(LPC_GPIO, _GPIO_Led_Verde, FALSE);

//GPIO Botones
#define _GPIO_SALIDA		0,17
#define _GPIO_ENROLL		0,15
#define _GPIO_LOGGUEO		0,18

//GPIO LCD
#define _GPIO_LCD_RST		0,3
#define _GPIO_LCD_ENABLE	0,21
#define _GPIO_LCD_DB4		0,22
#define _GPIO_LCD_DB5		0,27
#define _GPIO_LCD_DB6		0,28
#define _GPIO_LCD_DB7		0,13

#endif /* INC_GPIO_H_ */
