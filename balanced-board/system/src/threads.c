#include <stm32f3xx.h>
#include <cmsis_os.h>

#include <string.h>
#include <stdio.h>

#include <threads.h>
#include <lsm303dlhc.h>
#include <l3gd20.h>
#include <uart_debug.h>

osThreadId acclrmThread_id;
osThreadId gyroThread_id;
osThreadId protoThread_id;

osMailQDef(imuMailBox, 8, imuData_t);
osMailQId imuMailBox_id;

//osMutexId acclrmBufferMutex;
//osMutexId gyroBufferMutex;

/* private functions */
static size_t serialize_imuData_t(imuData_t *s, char *buff_ptr);

void acclrmThread(void const *arg) {
	uint8_t inBuffer[6];
	imuData_t *acclrmData;
//	volatile int16_t x,y,z;
	
	imuMailBox_id = osMailCreate(osMailQ(imuMailBox), NULL);
	acclrmData = (imuData_t *) osMailAlloc(imuMailBox_id, osWaitForever);
	
	readAcclrmXYZ(inBuffer);
	
	while(1) {
		osSignalWait(acclrmSignal, osWaitForever);
		
		readAcclrmXYZ(inBuffer);
		
		acclrmData->type = tAcclrm;
		acclrmData->x = (float) (int16_t) (inBuffer[0] | (inBuffer[1] << 8));
		acclrmData->y = (float) (int16_t) (inBuffer[2] | (inBuffer[3] << 8));
		acclrmData->z = (float) (int16_t) (inBuffer[4] | (inBuffer[5] << 8));
		
		osMailPut(imuMailBox_id, acclrmData);
	}
}

void gyroThread(void const *arg) {
	uint8_t inBuffer[6];

	
	while(1) {	
		osSignalWait(gyroSignal, osWaitForever);
		
		readGyroXYZ(inBuffer);
		
//		x = (float) (inBuffer[0] | (inBuffer[1] << 8));
//		y = (float) (inBuffer[2] | (inBuffer[3] << 8));
//		z = (float) (inBuffer[4] | (inBuffer[5] << 8));
	}
}

void protoThread(void const *arg) {
	char uartBuffer[100];
	size_t len = 0;
	osEvent evt;
	imuData_t *imuData;
	
	while (1) {
		evt = osMailGet(imuMailBox_id, osWaitForever);
		
		imuData = (imuData_t *) evt.value.p;
		
//		sprintf(uartBuffer, "X: %5.2f, Y: %5.2f, Z: %5.2f\n", imuData->x, imuData->y, imuData->z);
//		uart_debug_sendString(uartBuffer, strlen(uartBuffer));
		len = serialize_imuData_t(imuData, uartBuffer);
		uart_debug_sendStream(uartBuffer, len);
		
		osMailFree(imuMailBox_id, imuData);
	}
}

static size_t serialize_imuData_t(imuData_t *s, char *buff_ptr) {
	const size_t float_len = sizeof(float);
	const size_t char_len = sizeof(char);
	
	*buff_ptr++ = (char) s->type;
	memcpy(buff_ptr, &s->x, float_len);
	buff_ptr += sizeof(s->x);
	memcpy(buff_ptr, &s->y, float_len);
	buff_ptr += sizeof(s->y);
	memcpy(buff_ptr, &s->z, float_len);
	buff_ptr += sizeof(s->z);
	
	return 1*char_len + 3*float_len; 
}

