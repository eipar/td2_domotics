/*
 * @brief FreeRTOS Blinky example
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2014
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#include "board.h"
#include "FreeRTOS.h"
#include "task.h"

static ADC_CLOCK_SETUP_T ADCSetup;

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/* Sets up system hardware */
static void prvSetupHardware(void)
{
	SystemCoreClockUpdate();
	Board_Init();

	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 23, IOCON_FUNC0);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, 0, 16);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, 0, 23);

	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 25, IOCON_FUNC1);	//ADC0.2
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 26, IOCON_FUNC1);	//ADC0.3

	//Init
	Chip_ADC_Init(LPC_ADC, &ADCSetup);
	Chip_ADC_SetSampleRate(LPC_ADC, &ADCSetup, 100);
	Chip_ADC_EnableChannel(LPC_ADC, ADC_CH2, ENABLE);
	Chip_ADC_EnableChannel(LPC_ADC, ADC_CH3, ENABLE);

	//Habilito la interrupción
	Chip_ADC_Int_SetChannelCmd(LPC_ADC, ADC_CH2, ENABLE);
	Chip_ADC_Int_SetChannelCmd(LPC_ADC, ADC_CH3, ENABLE);

	Chip_ADC_SetBurstCmd(LPC_ADC, ENABLE);						//Habilito el burst

	//Al final habilito la interrupción
	NVIC_EnableIRQ(ADC_IRQn);

}


void ADC_IRQHandler(void){
	uint16_t dataADC[2];
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	Chip_ADC_ReadValue(LPC_ADC, ADC_CH2, &(dataADC[0]));
	Chip_ADC_ReadValue(LPC_ADC, ADC_CH3, &(dataADC[1]));

	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}


/* LED2 toggle thread */
static void vLEDTask2(void *pvParameters) {

	while (1) {

		Chip_GPIO_WritePortBit(LPC_GPIO, 0, 16, TRUE);
		Chip_GPIO_WritePortBit(LPC_GPIO, 0, 23, TRUE);

		/* About a 7Hz on/off toggle rate */
		vTaskDelay(configTICK_RATE_HZ / 14);

		Chip_GPIO_WritePortBit(LPC_GPIO, 0, 16, FALSE);
		Chip_GPIO_WritePortBit(LPC_GPIO, 0, 23, FALSE);

		/* About a 7Hz on/off toggle rate */
		vTaskDelay(configTICK_RATE_HZ / 14);
	}
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/**
 * @brief	main routine for FreeRTOS blinky example
 * @return	Nothing, function should not exit
 */
int main(void)
{
	prvSetupHardware();

	/* LED2 toggle thread */
	xTaskCreate(vLEDTask2, (signed char *) "vTaskLed2",
				configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),
				(xTaskHandle *) NULL);

	/* Start the scheduler */
	vTaskStartScheduler();

	/* Should never arrive here */
	return 1;
}

/**
 * @}
 */
