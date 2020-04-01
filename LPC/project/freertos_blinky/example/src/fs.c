/*
 * fs.c
 *
 *  Created on: 5 de ene. de 2016
 *      Author: Eugenia
 */

#include "fs.h"

/* Objetivo > Retorna el LSB del comando
 * Recibe 	> Comando
 * Devuelve > LSB
 * */
uint8_t FS_GetLowByte(uint16_t cmd){
	return (uint8_t)cmd&0x00FF;
}

/* Objetivo > Retorna el MSB del comando
 * Recibe 	> Comando
 * Devuelve > MSB
 * */
uint8_t FS_GetHighByte(uint16_t cmd){
	return (uint8_t)(cmd>>8)&0x00FF;
}

/* Objetivo > Crea el command packet
 * Recibe 	> Comando, Parametro y paquete a llenar
 * Devuelve > Nada
 * */
void CrearPaquete(uint8_t comando, uint8_t param, uint8_t* paq){
	uint16_t cs = 0; int i;

	paq[0] = FS_CMD_START1;
	paq[1] = FS_CMD_START2;
	paq[2] = FS_DEV_ID1;
	paq[3] = FS_DEV_ID0;

	if(param != 0){
		paq[4] = param;									//Por si es una func que mande ID
		paq[5] = paq[6] = paq[7] = 0x00;
	}else{
		paq[4] = paq[5] = paq[6] = paq[7] = 0x00;		//Param ZERO
	}

	paq[8] = comando;
	paq[9] = 0x00;

	cs = 0;
	for(i=0; i<10; i++) cs += paq[i];

	paq[10] = FS_GetLowByte(cs);
	paq[11] = FS_GetHighByte(cs);

}

/* Objetivo > Inicializar el lector de FP. Envía el comando FS_OPEN
 * Recibe 	> Nada
 * Devuelve > ACK		-Inicializo bien
 * 			  NON_ACK	-No inicializo		//Todavía no hecho
 * */
void Init_FS(void){
	uint8_t Caracter[12];
	uint16_t cs = 0;
	uint8_t paq[12]; //{0x55, 0xAA, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01};
	int i = 0;

	paq[0] = FS_CMD_START1;
	paq[1] = FS_CMD_START2;
	paq[2] = FS_DEV_ID1;
	paq[3] = FS_DEV_ID0;
	paq[4] = paq[5] = paq[6] = 0;
	paq[7] = 0x00;
	paq[8] = FS_OPEN;
	paq[9] = 0x00;

	for(i=0; i<10; i++) cs += paq[i];

	paq[10] = FS_GetLowByte(cs);
	paq[11] = FS_GetHighByte(cs);

	UART3_Send_String(paq, sizeof(paq), portMAX_DELAY);

	for(i=0; i<12; i++){
		UART3_Getchar(&(Caracter[i]), portMAX_DELAY);
	}
	//Se debe enviar dos veces sino el bicho no responde
	UART3_Send_String(paq, sizeof(paq), portMAX_DELAY);

	for(i=0; i<12; i++){
		UART3_Getchar(&(Caracter[i]), portMAX_DELAY);
	}
}

/* Objetivo > Control del LED background
 * Recibe 	> 0			-Apagar LED
 * 			  NON-0		-Prender LED
 * Devuelve > ACK		-OK
 * */
uint8_t TurnLed(uint8_t param){
	uint8_t paq[12], res[12]; int i;

	CrearPaquete(FS_CMOS_LED, param, paq);

	if(UART3_Send_String(paq, sizeof(paq), portMAX_DELAY) == pdFALSE)
		return FS_NACK_INFO;							//No se envió

	for(i=0; i<12; i++){
		UART3_Getchar(&(res[i]), portMAX_DELAY);
		//res[i] = Chip_UART_ReadByte(LPC_UART3);
	}

	if(paq[8] == FS_ACK_OK)							//Recibió bien
		return FS_ACK_OK;							//Retorno ACK

	return FS_NACK_INFO;							//Recibio mal - Retorno NON-ACK
}

/* Objetivo > Verificar cuántas FP hay logueadas
 * Recibe 	> Nada
 * Devuelve > Parámetro con cant de FP		//Recibio bien
 * 			  NON-ACK
 * */
uint32_t GetEnrolledCount(void){
	uint8_t paq[12]; int i;
	uint32_t aux;

	CrearPaquete(FS_ENROLL_COUNT, 0, paq);

	if(UART3_Send_String(paq, sizeof(paq), portMAX_DELAY) == pdFALSE)
		return FS_NACK_INFO;							//No se envió

	for(i=0; i<12; i++){
		UART3_Getchar(&(paq[i]), portMAX_DELAY);
	}

	if(paq[8] == FS_ACK_OK){								//Recibió bien
		aux = (paq[7]<<24)|(paq[6]<<16)|(paq[5]<<8)|paq[4];	//ID devuelvo en un solo uint
		return aux;
	}

	return FS_NACK_INFO;							//Recibio mal - Retorno NON-ACK
}

/* Objetivo > Verifica si está logueada un ID específico
 * Recibe 	> ID a verificar
 * Devuelve > ACK 				-ID enrolled
 * 			  NACK_INVALID_POS	-ID inválido
 * 			  NACK_IS_NOT_USED	-ID sin usar
 * */
uint8_t CheckEnrolled(uint8_t id){
	uint8_t paq[12]; int i;

	CrearPaquete(FS_CHECK_ENROLLED, id, paq);

	if(UART3_Send_String(paq, sizeof(paq), portMAX_DELAY) == pdFALSE)
		return FS_NACK_INFO;							//No se envió

	for(i=0; i<12; i++){
		UART3_Getchar(&(paq[i]), portMAX_DELAY);
	}

	if(paq[8] == FS_ACK_OK)							//Recibió bien
		return FS_ACK_OK;							//Retorno ACK

	if(paq[4] == FS_NACK_INVALID_POS)		//Me fijo qué error es
		return FS_NACK_INVALID_POS;			//Lo retorno

	if(paq[4] == FS_NOT_USED)
		return FS_NOT_USED;

	return FS_NACK_INFO;					//Sino devuelvo error
}

/* Objetivo > Comienza el logueo de una FP.
 * Realiza el comando EnrollStart -> Enroll1 -> Enroll2 -> Enroll3
 * Recibe 	> ID con que lo quiero loguear. Si es -1 es un Enroll sin guardar
 * 			  Puntero a un TEMPLATE_DATA dónde se guardará el template de la FP
 * Devuelve > ACK					-OK
 * 			  NACK_DB_IS_FULL		-Base de datos interna del lector llena
 * 			  NACK_INVALID_POS		-ID inválido
 * 			  NACK_IS_ALREADY_USED	-ID usado
 * 			  NACK_ENROLL_FAILED	-No pudo				(E1 / E2 / E3)
 * 			  NACK_BAD_FINGER		-Dedo mal puesto		(E1 / E2 / E3)
 * 			  DUPLICATED ID			-Tirá el ID duplicado	(E1 / E2 / E3)
 * Enroll3 devuelve un data packet con el enrolled template
 *
 * PROCEDIMIENTO: EnrollStart
 * 				  CaptureFinger
 * 				  Enroll1
 * 				  IsPressFinger hasta que lo saquen
 * 				  CaptureFinger
 * 				  Enroll2
 * 				  IsPressFinger hasta que lo saquen
 * 				  CaptureFinger
 * 				  Enroll3
 * OJO QUE ES BLOQUEANTE
 * */
uint8_t StartEnroll(uint8_t id){
	int flag_FINISHED = 0; uint8_t STATE_ENROLL = START;
	uint8_t cont_CAP = 0, cont_ENROLL = 0, cont_PRESS = 0, cmd;
	uint8_t paq[12], i;

	while(flag_FINISHED == 0){
		switch(STATE_ENROLL){
		case START:											//Empieza
			LCD_SendInstruction(LCD_DISPLAY_CLEAR);
			LCD_Print("Coloque el dedo");

			CrearPaquete(FS_CMD_ENROLL_START, id, paq);

			if(UART3_Send_String(paq, sizeof(paq), portMAX_DELAY) == pdFALSE){
				return FS_NACK_INFO;							//No se envió
			}

			for(i=0; i<12; i++){
				UART3_Getchar(&(paq[i]), portMAX_DELAY);
			}

			if(paq[4] != 0){								//Hubo un error, lo devuelvo
				flag_FINISHED = 1; STATE_ENROLL = START;
				return paq[4];
			}

			if(paq[8] == FS_ACK_OK){	//Paso al otro estado
				STATE_ENROLL = CAP;
			}

			break;
		case CAP:
			vTaskDelay(20/portTICK_RATE_MS);
			LCD_SendInstruction(LCD_DISPLAY_CLEAR);
			LCD_Print("Coloque el dedo");
			if(cont_CAP > 2){
				flag_FINISHED = 1; STATE_ENROLL = START; cont_CAP = 0;
				return FS_NACK_INFO;
			}else{

				i = CaptureFinger(0x01);	//Param NON-ZERO, Mejor imagen

				if(i == FS_ACK_OK){				//Paso al otro estado
					STATE_ENROLL = ENROLL; cont_CAP++;
				}else{
					if(i == FS_NACK_INFO){		//Hubo un error
						flag_FINISHED = 1; STATE_ENROLL = START; cont_CAP = 0;
						return FS_NACK_INFO;
					}else
						STATE_ENROLL = CAP;	//No está el dedo puesto
				}
			}

			break;
		case ENROLL:
			if(cont_ENROLL > 2){
				flag_FINISHED = 1; STATE_ENROLL = START; cont_ENROLL = 0;
				return FS_NACK_INFO;
			}else{
				if(cont_ENROLL == 0) cmd = FS_CMD_ENROLL1;
				if(cont_ENROLL == 1) cmd = FS_CMD_ENROLL2;
				if(cont_ENROLL == 2) cmd = FS_CMD_ENROLL3;

				CrearPaquete(cmd, 0, paq);

				if(UART3_Send_String(paq, sizeof(paq), portMAX_DELAY) == pdFALSE){
					flag_FINISHED = 1; STATE_ENROLL = START; cont_ENROLL = 0;
					return FS_NACK_INFO;							//No se envió
				}

				for(i=0; i<12; i++){
					UART3_Getchar(&(paq[i]), portMAX_DELAY);
				}

				if(paq[4] != 0){			//Hubo un error, lo devuelvo
					flag_FINISHED = 1; STATE_ENROLL = START; cont_ENROLL = 0;
					return paq[4];
				}

				if(paq[8] == FS_ACK_OK){		//Paso al otro estado
					if(cmd == FS_CMD_ENROLL3){
						flag_FINISHED = 1; cont_CAP = cont_ENROLL = cont_PRESS = 0;
						return FS_ACK_OK;
					}else{
						STATE_ENROLL = PRESS; cont_ENROLL++;
					}
				}
			}

			break;
		case PRESS:
			vTaskDelay(20/portTICK_RATE_MS);
			LCD_SendInstruction(LCD_DISPLAY_CLEAR);
			LCD_Print("Retire el dedo");

			if(cont_PRESS >= 2){
				flag_FINISHED = 1; STATE_ENROLL = START; cont_PRESS = 0;
				return FS_NACK_INFO;
			}else{
				i = CheckFinger();

				if(i == FS_FINGER_NOT){		//Si saco el dedo, paso al otro estado
					STATE_ENROLL = CAP; cont_PRESS++;
				}else{
					if(i == FS_NACK_INFO){	//Hubo un error, lo devuelvo
						flag_FINISHED = 1; STATE_ENROLL = START; cont_PRESS = 0;
						return FS_NACK_INFO;
					}else						//Espero a que saque el dedo
						STATE_ENROLL = PRESS;
					}
				}

			break;
		default:
			return FS_NACK_INFO;
		}
	}
	return FS_NACK_INFO;		//No deber;ia llegar acá
}

/* Objetivo > Verifica el estado de un dediño
 * Recibe 	> Nada
 * Devuelve > ACK				-Dedo puesto
 * 			  FS_FINGER_NOT		-Está el dedo puesto
 * 			  FS_NACK_INFO		-Error
 * */
uint8_t CheckFinger(void){
	uint8_t paq[12]; int i;

	CrearPaquete(FS_CMD_IS_PRESS_FINGER, 0, paq);

	if(UART3_Send_String(paq, sizeof(paq), portMAX_DELAY) == pdFALSE)
		return FS_NACK_INFO;							//No se envió

	for(i=0; i<12; i++){
		UART3_Getchar(&(paq[i]), portMAX_DELAY);
	}

	if(paq[4] == FS_FINGER_NOT){
		return FS_FINGER_NOT;		//Retorno dedo no puesto
	}else{
		return FS_ACK_OK;			//Retorno OK - Si puesto
	}

	return FS_NACK_INFO;		//ERROR
}

/* Objetivo > Elimina una FP con el ID asociado
 * Recibe 	> ID a eliminar
 * Devuelve > ACK				-OK
 * 			  NACK_INVALID_POS	-ID inválido
 * */
uint8_t DeleteFinger(uint8_t id){
	uint8_t paq[12]; int i;

	CrearPaquete(FS_CMD_DELETE, id, paq);

	if(UART3_Send_String(paq, sizeof(paq), portMAX_DELAY) == pdFALSE)
		return FS_NACK_INFO;							//No se envió

	for(i=0; i<12; i++){
		UART3_Getchar(&(paq[i]), portMAX_DELAY);
	}

	if(paq[8] == FS_ACK_OK)
		return FS_ACK_OK;				//OK

	if(paq[4] == FS_NACK_INVALID_POS)
		return FS_NACK_INVALID_POS;		//ID invalido

	return FS_NACK_INFO;			//ERROR
}

/* Objetivo > Elimina todas las FP
 * Recibe 	> NAda
 * Devuelve > ACK				-OK
 * 			  NACK_DB_IS_EMPTY	-Se entiende
 * */
uint8_t DeleteAllFinger(void){
	uint8_t paq[12]; int i;

	CrearPaquete(FS_CMD_DELETE_ALL, 0, paq);

	if(UART3_Send_String(paq, sizeof(paq), portMAX_DELAY) == pdFALSE)
		return FS_NACK_INFO;							//No se envió

	for(i=0; i<12; i++){
		UART3_Getchar(&(paq[i]), portMAX_DELAY);
	}

	if(paq[8] == FS_ACK_OK)
		return FS_ACK_OK;		//OK

	if(paq[4] == FS_DB_EMPTY)
		return FS_DB_EMPTY;		//No hay nada que eliminar

	return FS_NACK_INFO;		//ERROR
}

/* Objetivo > Identifica una FP en la DB del lector
 * Recibe 	> Nada
 * Devuelve > ACK + Parámetro c/ID		-OK
 * 			  NACK_DB_IS_EMPTY			-DB vacia
 * 			  NACK_IDENTIFY_FAILED		-No se identificó el dedo
 * */
//IMPORTANTE! Antes de usar ésta función, se debe llamar a CaptureFinger
uint8_t Identify(uint32_t* id){
	uint8_t paq[12]; int i = 0;

	CrearPaquete(FS_CMD_IDENTIFY, 0, paq);

	while(i != FS_NACK_INFO){	//Quiere decir que no está el dedo puesto
		i = CaptureFinger(0x00);

		if(i == FS_ACK_OK)		//Listo, sigo de largo
			break;
	}

	if(i == FS_NACK_INFO)		//Hubo un error en el CaptureFinger
		return i;

	if(UART3_Send_String(paq, sizeof(paq), portMAX_DELAY) == pdFALSE)
		return FS_NACK_INFO;							//No se envió

	for(i=0; i<12; i++){
		UART3_Getchar(&(paq[i]), portMAX_DELAY);
	}

	if(paq[8] == FS_ACK_OK){				//Recibió bien
		(*id) = (paq[7]<<24)|(paq[6]<<16)|(paq[5]<<8)|paq[4];		//Devuelvo el ID
		return FS_ACK_OK;
	}

	if(paq[4] == FS_DB_EMPTY)
		return FS_DB_EMPTY;		//No hay DB

	if(paq[4] == FS_IDENTIFY_FAILED)
		return FS_IDENTIFY_FAILED;		//No se identificó

	return FS_NACK_INFO;
}

/* Objetivo > Captura la imagen de un dedo
 * Recibe 	> Parámetro.
 * Si es 0 	  -> No la mejor imagen, pero rápido	//Identify
 * Si no es 0 -> Mejor imagen, pero lento			//Enrollment
 * Devuelve > ACK							-OK
 * 			  NACK_FINGER_IS_NOT_PRESSED	-No está el dedo puesto
 * OJO QUE ES BLOQUEANTE */
uint8_t CaptureFinger(uint8_t param){
	uint8_t paq[12]; int i;

	CrearPaquete(FS_CMD_CAPTURE, param, paq);

	while(CheckFinger() != FS_ACK_OK){
		//Espero que esté el dedo puesto
		Chip_GPIO_SetPinState(LPC_GPIO, 0, 9, TRUE);
	}

	Chip_GPIO_SetPinState(LPC_GPIO, 0, 9, FALSE);

	if(UART3_Send_String(paq, sizeof(paq), portMAX_DELAY) == pdFALSE)
		return FS_NACK_INFO;							//No se envió

	for(i=0; i<12; i++){
		UART3_Getchar(&(paq[i]), portMAX_DELAY);
	}

	if(paq[8] == FS_ACK_OK)					//Recibió bien
		return FS_ACK_OK;					//Retorno ACK

	if(paq[4] == FS_FINGER_NOT)				//Si no apoyo el dedo
		return FS_FINGER_NOT;

	return FS_NACK_INFO;
}
