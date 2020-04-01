/*
 * @brief Control de Domótica mediante un lector de Huellas Digitales
 *
 * @note
 * Catédra de TDII, Miércoles TN, 2015
 *
 * @author
 * González González, Francisco 	- Leg:
 * Ipar, Eugenia					- Leg: 143.381-7
 */

#include "main.h"

//Función que abre y cierra la puerta
uint8_t Mover_Puerta(void){
	uint8_t flag_finished = 0, aux;
	uint8_t State_Lock = LOGIN;

	while(flag_finished == 0){
		switch(State_Lock){
		case LOGIN:
			while(State_Door == CLOSED){				//Cambia el estado la interrupción x GPIO del Fin de Carrera
				PUERTA_OPEN; //BUZZER_ON;	LED_ROJO_ON;
			}
			PUERTA_OFF;	//BUZZER_OFF;	LED_ROJO_OFF;
			State_Lock = LOGOUT;
			vTaskDelay(10/portTICK_RATE_MS);			//Espero 5 seg a que la persona pase. Puse 10 ms para probarlo. DPS CAMBIAR!!
			break;
		case LOGOUT:									//Cierro la puerta una vez que entró
			while(State_Door == OPEN){					//Cambia el estado la interrupción x GPIO del Fin de Carrera
				PUERTA_CLOSE; //LED_ROJO_ON;
			}
			PUERTA_OFF; //LED_ROJO_OFF;
			flag_finished = 1;
			aux = 0;
			break;
		default:
			flag_finished = 1;
			aux = 1;
		}
	}

	return aux;
}

/** Interrupciones **/
//Interrupción del ADC0.2 y ADC0.3, respectivamente, luz y temperatura
void ADC_IRQHandler(void){
	uint16_t dataADC[2];
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	Chip_ADC_ReadValue(LPC_ADC_ID, ADC_Luz, &(dataADC[0]));				//Cuentas Luz
	Chip_ADC_ReadValue(LPC_ADC_ID, ADC_Temp, &(dataADC[1]));			//Cuentas Temp
	xQueueSendFromISR(qADC_Luz, &dataADC, &xHigherPriorityTaskWoken);

	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}

//Interrupción de los GPIO.
void EINT3_IRQHandler(void){
	uint32_t interrupt; uint8_t flag = 0;
	portBASE_TYPE xHigherPriorityTaskWoken;

	if(Chip_GPIOINT_IsIntPending(LPC_GPIOINT, GPIOINT_PORT0) == TRUE){			//Se presionó algún botón
		interrupt = Chip_GPIOINT_GetStatusFalling(LPC_GPIOINT, GPIOINT_PORT0);
		if(interrupt & (1<<18)){
			xQueueSendFromISR(lectorHD, &flag, &xHigherPriorityTaskWoken);		//Activo el logueo
			Chip_GPIOINT_ClearIntStatus(LPC_GPIOINT,GPIOINT_PORT0, 1 << 18);
		}else{
			if(interrupt & (1<<17)){
				xQueueSendFromISR(out, &flag, &xHigherPriorityTaskWoken);			//Activo la salida
				Chip_GPIOINT_ClearIntStatus(LPC_GPIOINT,GPIOINT_PORT0, 1 << 17);
			}else{
				if(interrupt & (1<<15)){											//Botón Enroll
					xQueueSendFromISR(enroll, &flag, &xHigherPriorityTaskWoken);
					Chip_GPIOINT_ClearIntStatus(LPC_GPIOINT,GPIOINT_PORT0, 1 << 15);
				}
			}
		}
	}else{
		interrupt = Chip_GPIOINT_GetStatusFalling(LPC_GPIOINT, GPIOINT_PORT2);	//Se activó un FdC
		if(interrupt & (1<<5)){			//Puerta Cerrada
			State_Door = CLOSED;
			Chip_GPIOINT_ClearIntStatus(LPC_GPIOINT,GPIOINT_PORT2, 1 << 5);
		}else{
			if(interrupt & (1<<4)){		//Puerta Abierta
				State_Door = OPEN;
				Chip_GPIOINT_ClearIntStatus(LPC_GPIOINT,GPIOINT_PORT2, 1 << 4);
			}else{
				if(interrupt & (1<<6)){	//Persiana Cerrada
					State_Blind = DOWN;
					Chip_GPIOINT_ClearIntStatus(LPC_GPIOINT,GPIOINT_PORT2, 1 << 6);
				}else{
					//Persiana Abierta
					State_Blind = UP;
					Chip_GPIOINT_ClearIntStatus(LPC_GPIOINT,GPIOINT_PORT2, 1 << 7);
				}
			}
		}
	}

	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}

/* Sets up system hardware */
static void prvSetupHardware(void)
{
	SystemCoreClockUpdate();

	/* Init de GPIOs */
	Init_GPIOs();

	/* Init de UARTs */
	UART3_Init(9600);
	UART2_Init(9600);

	/* Init de ADC Luz y Temp */
	Init_ADC();

	//Por default
	Estado_Usuario.luz = NATURAL;
	Estado_Usuario.temp = 20;
	Estado_Actual.luz = NATURAL;
	Estado_Actual.temp = 20;
}

/** Tareas de Sensores **/
/* Tarea que recibirá los datos del ADC y los cargará en la variable global Estado_Actual */
void prvADC_Sensors(void *pvParameters){
	uint16_t dato[2];

	while (1){

		if(xQueueReceive(qADC_Luz, &dato, portMAX_DELAY) == pdPASS){
			 //Hago la verificación antes de cargar a la global, dado que capaz un ADC terminó y el otro no,
			// * entonces no cargo 0 en la global. Se podría verificar con Chip_ReadStatus, para ver en el registro,
			 //* pero debería hacerse en la interrupción y tardaría mucho
			if(dato[1] != 0){
				dato[1] = (double)(dato[1]/41);					//Convierto a Fahrenheit
				dato[1] = (double)((dato[1] - 32)/1.8);			//Paso a Celsius

				Estado_Actual.temp = dato[1];					//Lo cargo en la variable global
			}

			if(dato[0] != 0){									//Identifico en qué estado estaría la luz
				if(dato[0] <= LUZ_MAX || dato[0] >= LUZ_MIN){
					Estado_Actual.luz = NATURAL;
				}
				if(dato[0] <= LUZ_MIN){
					Estado_Actual.luz = OSCURA;
				}else{
					Estado_Actual.luz = LIGHT;
				}
			}
		}

		vTaskDelay(10/portTICK_RATE_MS);
	}

}

/* Tarea que espera a que una persona desee entrar a la habitación, identifica la huella digital y procede a accionar */
void prvUART_FP(void *pvParameters){
	uint8_t i;
	uint32_t id; uint8_t aux; uint8_t send[3];
	uint8_t cont = 0, retorno = 0;

	/* Inicialización del lector */
	Init_FS();

	/* Init LCD */
	LCD_Init();
	LCD_GoToxy(0,0);
	LCD_SendInstruction(LCD_DISPLAY_CLEAR);
	LCD_Print("Desocupado");

	Init_RTC();

	while(1){
		//flag ATRODEN lo cambia la interrupcion x GPIO del boton de salida
		if((xQueueReceive(lectorHD, &aux, portMAX_DELAY) == pdPASS)&&(flag_ATRODEN == OFF)){
			/* Para que el lector tome el dedo, debe tener la luz prendida */
			TurnLed(0x01);
			LCD_GoToxy(0,0);
			LCD_Print("Coloque el dedo sobre el lector");

			while((retorno != FS_ACK_OK)&&(cont < 20)){
				cont++;
				retorno = CheckFinger();
			}
			cont = 0;
			LCD_SendInstruction(LCD_DISPLAY_CLEAR);

			if(retorno == FS_ACK_OK){
				LCD_Print("Identificando...");
				aux = Identify(&id);

				if(aux == FS_ACK_OK){
					LCD_SendInstruction(LCD_DISPLAY_CLEAR);
					LCD_Print("Login correcto !");


					Estado_Usuario.id = id;
					send [0] = id;
					send[1] = send[2] = 200;
					//Solicito a la PC que me envie las preferencias del usuario
					UART2_Send_String(send, sizeof(send), portMAX_DELAY);

					for(i=0; i<3; i++){
						UART2_Getchar(&(send[i]), portMAX_DELAY);
					}

					Estado_Usuario.luz = send[2];
					Estado_Usuario.temp = send[1];

					flag_ATRODEN = ON;				//Sé si hay alguien adentro o no
					flag_ACTION = ON;				//Activa la máq de est accionadora
					flag_SD = ON;					//Activo guardar en la SD el logueo
					TurnLed(0x00);

					LCD_SendInstruction(LCD_DISPLAY_CLEAR);
					LCD_Print("Puede pasar");
					Mover_Puerta();
					LCD_SendInstruction(LCD_DISPLAY_CLEAR);
					LCD_Print("Ocupado");
					//LED_ROJO_ON; LED_VERDE_OFF;
				}else{
					//BUZZER_ON;
					LCD_SendInstruction(LCD_DISPLAY_CLEAR);
					LCD_Print("Login incorrecto");
					PUERTA_OFF;
					PERSIANA_OFF; LUZ_OFF; VENT_OFF; CALEF_OFF;
					LCD_SendInstruction(LCD_DISPLAY_CLEAR);
					LCD_Print("Desocupado");
					//BUZZER_OFF;
					//LED_ROJO_OFF; LED_VERDE_ON;
				}
			}else{
				//BUZZER_ON;
				LCD_SendInstruction(LCD_DISPLAY_CLEAR);
				LCD_Print("Dedo faltante. Vuelva a empezar");
				PUERTA_OFF;
				PERSIANA_OFF; LUZ_OFF; VENT_OFF; CALEF_OFF;
				LCD_SendInstruction(LCD_DISPLAY_CLEAR);
				LCD_Print("Desocupado");
				//BUZZER_OFF;
				//LED_ROJO_OFF; LED_VERDE_ON;
			}

			TurnLed(0x00);
		}
		vTaskDelay(1/portTICK_RATE_MS);
	}
}

/** Tareas de Estados **/
/* Tarea que según los datos del ADC, modifica el estado que deberá tener el sistema, modificando la variable Estado_Sistema */
void prvTask_States(void *pvParameters){
	while(1){
		if(Estado_Actual.temp == 0 || Estado_Actual.luz == 0){
			//Pongo de error que apagará to do, dado que algo paso.
			Estado_Sistema.luz = Estado_Sistema.temp = ERROR;
		}else{

			//Check de Temperatura
			if(Estado_Actual.temp >= ((Estado_Usuario.temp)+SECURITY_FACTOR)){
				Estado_Sistema.temp = COOL;
			}else{
				if(Estado_Actual.temp <= ((Estado_Usuario.temp)-SECURITY_FACTOR)){
					Estado_Sistema.temp = HEAT;
				}else{
					Estado_Sistema.temp = AMBIENT;
				}
			}

			//Check de Luz
			switch(Estado_Usuario.luz){
			case NATURAL:
				if(Estado_Actual.luz == OSCURA){
					Estado_Sistema.luz = MIX;		//Prendo la luz y abro la persiana
				}else{
					Estado_Sistema.luz = NATURAL;	//Como lo pidio. Si hay mucha luz (LIGHT), mejor!
				}
				break;
			case LIGHT:
				if(Estado_Actual.luz == OSCURA){
					Estado_Sistema.luz = LIGHT;
				}else{
					if(Estado_Actual.luz == NATURAL){
						Estado_Sistema.luz = MIX;
					}else{
						Estado_Sistema.luz = LIGHT;
					}
				}
				break;
			case OSCURA:
				//Le gusta la oscuridad asi que no me importa el estado de la habitacion, apago todoo
				Estado_Sistema.luz = OSCURA;
				break;
			case MIX:
				if(Estado_Actual.luz == LIGHT){
					Estado_Sistema.luz = NATURAL;
				}else{
					Estado_Sistema.luz = MIX;
				}
				break;
			default:
				Estado_Sistema.luz = OSCURA;		//Donde se apaga todoo por las dudas
			}

		}

		vTaskDelay(20/portTICK_RATE_MS);
	}
}

/* Tarea que se activa al presionar el botón de salida. Mueve la puerta, y activa el flag correspondiente para actualizar el estado del cuarto */
void prvTask_Salida(void *pvParameters){
	uint8_t aux;

	while(1){
		if(xQueueReceive(out, &aux, portMAX_DELAY) == pdPASS){

			flag_ATRODEN = OFF;
			flag_ACTION = OFF;
			flag_SD = ON;

			Mover_Puerta();
			LCD_SendInstruction(LCD_DISPLAY_CLEAR);
			LCD_Print("Desocupado");
			CALEF_OFF; VENT_OFF; LUZ_OFF;
			//LED_ROJO_OFF; LED_VERDE_ON;

		}
		vTaskDelay(20/portTICK_RATE_MS);
	}
}

/** Tareas Accionadoras **/
/* Tarea que acciona los relés según la variable Estado_Sistema. Es activada por el logueo de un ID */
void prvTask_Action(void *pvParameters){

	while(1){
		if(flag_ACTION == ON){
			switch(Estado_Sistema.temp){
			case AMBIENT:					//Con la temp amb está bien
				VENT_OFF; CALEF_OFF;
				break;
			case HEAT:						//Hace frío prendo el calefacto
				VENT_OFF; CALEF_ON;
				break;
			case COOL:						//Hace calor prendo el ventilador
				VENT_ON; CALEF_OFF;
				break;
			default:
				Estado_Sistema.temp = AMBIENT;	//Si hay un error apago todoo
			}

			switch(Estado_Sistema.luz){
			case NATURAL:						//Luz ambiente, abro la persiana
				while(State_Blind == DOWN){
					PERSIANA_OPEN;
				}
				PERSIANA_OFF; LUZ_OFF;
				break;
			case LIGHT:							//Persiana cerrada, luz prendida
				while(State_Blind == UP){
					PERSIANA_CLOSE;
				}
				PERSIANA_OFF; LUZ_ON;
				break;
			case OSCURA:						//Toodo apagado
				while(State_Blind == UP){
					PERSIANA_CLOSE;
				}
				PERSIANA_OFF; LUZ_OFF;
				break;
			case MIX:							//Ambas cosas
				while(State_Blind == DOWN){
					PERSIANA_OPEN;
				}
				PERSIANA_OFF; LUZ_ON;
				break;
			default:
				Estado_Sistema.luz = OSCURA;	//Por las dudas apago todoo
			}
		}
		vTaskDelay(20/portTICK_RATE_MS);
	}
}

/* Tarea que se activa al presionar el botón para enrolar. Ingresa una nueva HD, y la envía a la PC para que cree un usuario nuevo */
void prvTask_Enroll(void *pvParameters){
	uint8_t id = 0, aux;
	uint8_t send[3];
	char aux2[2];

	while(1){

		if(xQueueReceive(enroll, &aux, portMAX_DELAY) == pdPASS){

			LCD_SendInstruction(LCD_DISPLAY_CLEAR);
			LCD_GoToxy(0,0);
			LCD_Print("Enrolando");

			do{
				aux = CheckEnrolled(id);
				id++;
			}while(id<200 && aux == FS_ACK_OK);

			if(aux == FS_NOT_USED){
				TurnLed(0x01);
				id--;
				aux = StartEnroll(id);

				if(aux == FS_ACK_OK){
					LCD_SendInstruction(LCD_DISPLAY_CLEAR);
					LCD_GoToxy(0,1);
					LCD_Print("Exito! ID:");
					aux2[0] = id + '0';
					aux2[1] = '\0';
					LCD_Print(aux2);
					vTaskDelay(30/portTICK_RATE_MS);
					//Mandar por puerto serie

					send[0] = id;
					send[1] = 255;
					send[2] = 255;

					UART2_Send_String(send, sizeof(send), portMAX_DELAY);
				}
			}else{
				if(id == 200){
					LCD_SendInstruction(LCD_DISPLAY_CLEAR);
					LCD_Print("Error. Memoria llena");
				}else{
					LCD_SendInstruction(LCD_DISPLAY_CLEAR);
					LCD_Print("Error. Vuelva a comenzar");
				}
			}
			TurnLed(0x00);
			vTaskDelay(30/portTICK_RATE_MS);

			LCD_SendInstruction(LCD_DISPLAY_CLEAR);
			if(flag_ATRODEN == ON){
				LCD_Print("Ocupado");
			}else{
				LCD_Print("Desocupado");
			}
		}
		vTaskDelay(20/portTICK_RATE_MS);
	}
}

/** Tareas de Comunicación **/
/* Tarea que actualiza la luz y temp de la PC para mostrar al usuario */
void prvTask_Send_State(void *pvParameters){
	uint8_t send[3];

	while(1){
		send[0] = 255;
		send[1] = Estado_Actual.temp;
		send[2] = Estado_Actual.luz;

		UART2_Send_String(send, sizeof(send), portMAX_DELAY);

		vTaskDelay(50/portTICK_RATE_MS);
	}
}

/* Tarea que al llegarle el comando de eliminar un usuario, lo elimina */
void prvTask_Delete(void *pvParameters){
	uint8_t i, send[3];

	while(1){
		if(UART2_Getchar(&(send[0]), portMAX_DELAY) == pdTRUE){

			for(i=1; i<3; i++){
				UART2_Getchar(&(send[i]), portMAX_DELAY);
			}

			if(send[1] == 250 && send[2] == 250){
				if(CheckEnrolled(send[0]) == FS_ACK_OK){
					DeleteFinger(send[0]);
				}
			}
		}

		vTaskDelay(50/portTICK_RATE_MS);
	}
}

/* Tarea que recibe en una cola el ID del usuario que entro/salió al/del cuarto, y lo guarda en la SD */
void prvTask_SD_Log(void *pvParameters){
	FIL 	fp;
	FILINFO	fs;
	FATFS 	Fatfs[1];
	int 	bw;
	int 	len;
	char 	datos[16];
	FRESULT res; RTC_TIME_T clock_stuff;

	taskDISABLE_INTERRUPTS();
		res = f_mount(0, &Fatfs[0]);
		res = f_open(&fp,"0:log.txt",FA_OPEN_ALWAYS | FA_WRITE);
		res = f_stat("0:log.txt",&fs);
		res = f_lseek(&fp,fs.fsize);
		res = f_sync(&fp);
	taskENABLE_INTERRUPTS();

	while(1){
		if(flag_SD == 1){
			Chip_RTC_GetFullTime(LPC_RTC, &clock_stuff);
			sprintf(datos, "%lu:%lu:%lu--ID:%lu\r\n", clock_stuff.time[RTC_TIMETYPE_HOUR], clock_stuff.time[RTC_TIMETYPE_MINUTE], clock_stuff.time[RTC_TIMETYPE_SECOND], Estado_Sistema.id);
			len = strlen(datos);
			taskDISABLE_INTERRUPTS();
			res = f_write(&fp,(const void*)datos,len,(UINT*)&bw);
			res = f_sync(&fp);
			taskENABLE_INTERRUPTS();
			flag_SD = 0;
		}
		vTaskDelay(80/portTICK_RATE_MS);
	}

	taskDISABLE_INTERRUPTS();
		f_close(&fp);
		f_mount(0, NULL);
	taskENABLE_INTERRUPTS();

}

int main(void)
{
	prvSetupHardware();

	qADC_Luz = xQueueCreate(ADC_LENGHT, sizeof(int));
	lectorHD = xQueueCreate(ADC_LENGHT, sizeof(char));
	out		 = xQueueCreate(ADC_LENGHT, sizeof(char));
	enroll	 = xQueueCreate(ADC_LENGHT, sizeof(char));

	/* Tareas de Sensores */
	xTaskCreate(prvADC_Sensors, (signed char*)"Luz&Temp",
				configMINIMAL_STACK_SIZE, NULL, tskSENSOR_priority,
				(xTaskHandle *) NULL);

	xTaskCreate(prvUART_FP, (signed char*)"Lector",
				configMINIMAL_STACK_SIZE, NULL, tskFP_priority,
				(xTaskHandle *) NULL);

	/* Máquinas de Estados */
	xTaskCreate(prvTask_States, (signed char*)"States",
				configMINIMAL_STACK_SIZE, NULL, tskSTATE_priority,
				(xTaskHandle*) NULL);

	xTaskCreate(prvTask_Salida, (signed char*)"Salida",
				configMINIMAL_STACK_SIZE, NULL, tskSTATE_priority,
				(xTaskHandle*) NULL);

	/* Tareas Accionadoras */
	xTaskCreate(prvTask_Action, (signed char*)"Action",
				configMINIMAL_STACK_SIZE, NULL, tskACTION_priority,
				(xTaskHandle*) NULL);

	xTaskCreate(prvTask_Enroll, (signed char*)"Enroll",
				configMINIMAL_STACK_SIZE, NULL, tskACTION_priority,
				(xTaskHandle*) NULL);

	/* Comunicación (Puerto Serie, USB, SD y Ethernet */
	xTaskCreate(prvTask_Send_State, (signed char*)"SendState",
				configMINIMAL_STACK_SIZE, NULL, tskACTION_priority,
				(xTaskHandle*) NULL);

	xTaskCreate(prvTask_Delete, (signed char*)"SendState",
				configMINIMAL_STACK_SIZE, NULL, tskACTION_priority,
				(xTaskHandle*) NULL);

	xTaskCreate(prvTask_SD_Log, (signed char *) "SDLog",
				configMINIMAL_STACK_SIZE*4, NULL, tskACTION_priority,
				(xTaskHandle *) NULL);

	/* Start the scheduler */
	vTaskStartScheduler();

	/* Should never arrive here */
	return 1;
}
