/*
 * hardware.c
 *
 *  Created on: 3 de ene. de 2016
 *      Author: Eugenia
 */

#include "hardware.h"

/** Inicialización ADC **/
void Init_ADC(void){

	//Configuración del pin P0.25 como AD0.2
	Chip_IOCON_PinMuxSet(LPC_IOCON, ADC_Port, ADC_Pin_Luz, IOCON_FUNC1);
	//Configuración del pin P0.26 como AD0.3
	Chip_IOCON_PinMuxSet(LPC_IOCON, ADC_Port, ADC_Pin_Temp, IOCON_FUNC1);

	//Init
	Chip_ADC_Init(LPC_ADC_ID, &ADCSetup);
	Chip_ADC_SetSampleRate(LPC_ADC_ID, &ADCSetup, ADC_Freq);		//Seteo la freq
	Chip_ADC_EnableChannel(LPC_ADC_ID, ADC_Luz, ENABLE);			//Habilito el canal
	Chip_ADC_EnableChannel(LPC_ADC_ID, ADC_Temp, ENABLE);

	//Habilito la interrupción
	Chip_ADC_Int_SetChannelCmd(LPC_ADC_ID, ADC_Luz, ENABLE);
	Chip_ADC_Int_SetChannelCmd(LPC_ADC_ID, ADC_Temp, ENABLE);

	Chip_ADC_SetBurstCmd(LPC_ADC_ID, ENABLE);						//Habilito el burst

	//Al final habilito la interrupción
	NVIC_EnableIRQ(ADC_IRQn);
}

/** Inicialización GPIO **/
void Init_GPIOs(void){

	/* Init de Hardware */
	Chip_GPIO_Init(LPC_GPIO);
	Chip_IOCON_Init(LPC_IOCON);

	Chip_IOCON_PinMuxSet(LPC_IOCON, _GPIO_SALIDA, IOCON_FUNC0);
	Chip_IOCON_PinMuxSet(LPC_IOCON, _GPIO_ENROLL, IOCON_FUNC0);

	/* Inputs */
	Chip_GPIO_SetPinDIR(LPC_GPIO, _GPIO_FdC_1, FALSE);
	Chip_GPIO_SetPinDIR(LPC_GPIO, _GPIO_FdC_2, FALSE);
	Chip_GPIO_SetPinDIR(LPC_GPIO, _GPIO_FdC_3, FALSE);
	Chip_GPIO_SetPinDIR(LPC_GPIO, _GPIO_FdC_4, FALSE);
	Chip_GPIO_SetPinDIR(LPC_GPIO, _GPIO_SALIDA, FALSE);
	Chip_GPIO_SetPinDIR(LPC_GPIO, _GPIO_ENROLL, FALSE);
	Chip_GPIO_SetPinDIR(LPC_GPIO, _GPIO_LOGGUEO, FALSE);

	/* Outputs */
	Chip_GPIO_SetPinDIR(LPC_GPIO, _GPIO_Pers_Izq, TRUE);
	Chip_GPIO_SetPinDIR(LPC_GPIO, _GPIO_Pers_Der, TRUE);
	Chip_GPIO_SetPinDIR(LPC_GPIO, _GPIO_Puerta_Izq, TRUE);
	Chip_GPIO_SetPinDIR(LPC_GPIO, _GPIO_Puerta_Der, TRUE);
	Chip_GPIO_SetPinDIR(LPC_GPIO, _GPIO_Rele_Luz, TRUE);
	Chip_GPIO_SetPinDIR(LPC_GPIO, _GPIO_Rele_Vent, TRUE);
	Chip_GPIO_SetPinDIR(LPC_GPIO, _GPIO_Rele_Calef, TRUE);
	//Chip_GPIO_SetPinDIR(LPC_GPIO, _GPIO_Rele_Extra, TRUE);
	//Chip_GPIO_SetPinDIR(LPC_GPIO, _GPIO_Buzzer, TRUE);
	//Chip_GPIO_SetPinDIR(LPC_GPIO, _GPIO_Led_Rojo, TRUE);
	//Chip_GPIO_SetPinDIR(LPC_GPIO, _GPIO_Led_Verde, TRUE);

	/* Empieza todoo apagado */
	PERSIANA_OFF;
	PUERTA_OFF;
	LUZ_OFF;	VENT_OFF;	CALEF_OFF;
	//BUZZER_OFF;	LED_ROJO_OFF;	LED_VERDE_OFF;

	/* Habilito las interrupciones por GPIO */
	Chip_GPIOINT_SetIntFalling(LPC_GPIOINT, GPIOINT_PORT2, (1<<4)|(1<<5)|(1<<6)|(1<<7));		//FdC
	Chip_GPIOINT_SetIntFalling(LPC_GPIOINT, GPIOINT_PORT0, (1<<15)|(1<<18)|(1<<17));			//Botones

	NVIC_EnableIRQ(EINT3_IRQn);
}

void Init_RTC(void){
	RTC_TIME_T FullTime;
	uint8_t aux[3]; int i;

	Chip_RTC_Init(LPC_RTC);

	FullTime.time[RTC_TIMETYPE_SECOND] = 0;
	FullTime.time[RTC_TIMETYPE_DAYOFMONTH]  = 7;
	FullTime.time[RTC_TIMETYPE_DAYOFWEEK]   = 7;
	FullTime.time[RTC_TIMETYPE_DAYOFYEAR]   = 38;
	FullTime.time[RTC_TIMETYPE_MONTH]   = 2;
	FullTime.time[RTC_TIMETYPE_YEAR]    = 2016;

	aux[0] = 250;

	UART2_Send_String(aux, sizeof(aux), portMAX_DELAY);

	for(i=0; i<3; i++){
		UART2_Getchar(&(aux[i]), portMAX_DELAY);
	}

	FullTime.time[RTC_TIMETYPE_MINUTE]  = aux[2];
	FullTime.time[RTC_TIMETYPE_HOUR]    = aux[1];

	Chip_RTC_SetFullTime(LPC_RTC, &FullTime);

	Chip_RTC_Enable(LPC_RTC, ENABLE);
}
