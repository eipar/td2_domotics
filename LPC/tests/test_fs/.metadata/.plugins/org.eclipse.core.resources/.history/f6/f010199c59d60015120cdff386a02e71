/*
 * fs.h
 *
 *  Created on: 5 de ene. de 2016
 *      Author: Eugenia
 */

#ifndef INC_FS_H_
#define INC_FS_H_

/** LPCOpen **/
#include "chip.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "uart3.h"
#include "lcd.h"

/** Comandos a enviar para el lector **/

#define FS_OPEN 					0x01		//Abre la comunicación
#define FS_CLOSE					0x02		//Cierra la comunicación - En realidad no hace nada
#define FS_USB_INTERNAL_CHECK		0x03		//Chequea si el USB conectado es válido
#define FS_CHANGE_BAUDRATE			0x04		//Cambia la vel del UART

#define FS_CMOS_LED					0x12		//Control del Led que tiene

#define FS_ENROLL_COUNT				0x20		//Devuelve el conteo de los dedos adheridos
#define FS_CHECK_ENROLLED			0x21		//Chequea si un ID especifico está adherido
#define FS_CMD_ENROLL_START			0x22		//Empieza adhisión de un nuevo dedo
#define FS_CMD_ENROLL1				0x23		//Primer template del nuevo dedo
#define FS_CMD_ENROLL2				0x24		//Segundo template del nuevo dedo
#define FS_CMD_ENROLL3				0x25		//Tercer template del nuevo dedo
#define FS_CMD_IS_PRESS_FINGER		0x26		//Verifica si hay un dedo presionado en el sensor

#define FS_CMD_DELETE				0x40		//Elimina un ID especifico
#define FS_CMD_DELETE_ALL			0x41		//Elimina todos los ID

#define FS_CMD_VERIFY				0x50		//Verifica la imagen del dedo capturado con un ID específico
#define FS_CMD_IDENTIFY				0x51		//Identifica el dedo capturado con la DB
#define FS_CMD_VERIFY_TEMPLATE		0x52		//Verifica el template de un dedo con un ID específico
#define FS_CMD_IDENTIFY_TEMPLATE	0x53		//Identifica el tempalte de un dedo con la dB

#define FS_CMD_CAPTURE				0x60		//Toma la imagen del dedo

#define FS_CMD_GETIMAGE				0x62		//Baja la imagen del dedo
#define FS_CMD_GET_RAWIMAGE			0x63		//Captura y baja la imagen del dedo

#define FS_CMD_GET_TEMPLATE			0x70		//Baja el template de un ID específico
#define FS_CMD_ADD_TEMPLATE			0x71		//Carga el template de un ID específico

#define FS_ACK_OK					0x30		//Acknowledge
#define FS_NACK_INFO				0x31		//Non-Acknowledge

/** Configuración del bicho **/

#define FS_MAX_USERS				200			//Capacidad máx que soporta

#define FS_CMD_START1				0x55		//Command Packet - Paquete de envío y recepción para el sensor
#define FS_CMD_START2				0xAA
#define FS_DEV_ID0					0x00
#define FS_DEV_ID1					0x01

#define FS_DATA_START1				0x5A		//Data Packet - Paquete de recepcion de info
#define FS_DATA_START2				0xA5

/** Error Codes **/

//De Datasheet
#define FS_CMD_ERROR_START			0x10
#define FS_NACK_INVALID_POS			0x03		//El ID no es válido
#define FS_NOT_USED					0x04		//ID sin uso
#define FS_ALREADY_USED				0x05		//ID usado
#define FS_COMM_ERR					0x06		//Error de comunicación
#define FS_VERIFY_FAILED			0x07		//Verificación fallida
#define FS_IDENTIFY_FAILED			0x08		//Identificación fallida
#define FS_DB_FULL					0x09		//DB llena
#define FS_DB_EMPTY					0x0A		//DB vacía
#define FS_BAD_FINGER				0x0C		//No se lee bien la huella
#define FS_ENROLL_FAILED			0x0D		//No se pudo agregar
#define FS_NOT_SUPPORTED			0x0E		//Comando inválido
#define FS_DEV_ERR					0x0F		//Error del sensor
#define FS_INV_PARAM				0x11		//Parámetro inválido
#define FS_FINGER_NOT				0x12		//No está puesto el dedo
//De funciones
#define FS_ERR_CHECKSUM				0x73
#define FS_NON_ERR					0x74

/* Máq de Estados del Enroll */
#define START		1
#define CAP			2
#define ENROLL		3
#define PRESS		4

/** Variables **/

/** Prototipos **/

void Init_FS(void);

uint8_t FS_GetLowByte(uint16_t);
uint8_t FS_GetHighByte(uint16_t);
void CrearPaquete(uint8_t, uint8_t, uint8_t*);

uint8_t TurnLed(uint8_t);			//Prende-Apaga el LED
uint32_t GetEnrolledCount(void);	//Devuelve la cant de IDs
uint8_t CheckEnrolled(uint8_t);		//Chequea  si un ID está
uint8_t StartEnroll(uint8_t);		//EnrollStart -> Enroll1 -> Enroll2 -> Enroll3
uint8_t CheckFinger(void);			//Verifica si un dedo está presionado
uint8_t DeleteFinger(uint8_t);		//Elimina el FP de un ID
uint8_t DeleteAllFinger(void);		//Elimina todos los FP
uint8_t Identify(uint32_t*);			//Identifica la huella digital devolviendo el ID
uint8_t CaptureFinger(uint8_t);		//Guarda en el sensor una imagen del FP

#endif /* INC_FS_H_ */
