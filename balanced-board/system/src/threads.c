#include <stm32f3xx.h>
#include <cmsis_os.h>

#include <string.h>
#include <stdio.h>

#include <threads.h>
#include <lsm303dlhc.h>
#include <l3gd20.h>
#include <uart_debug.h>
#include <math.h>
#include <arm_math.h>
#include <ili9488.h>

#define M_PIf (float) 3.14159265359

/* Global thread ids */
osThreadId acclrmThread_id;
osThreadId gyroThread_id;
osThreadId protoThread_id;

/* Global Mailbox ids */
osMailQId imuMailBox_id;

/* */
physics_t phy;

/* private functions */
static size_t serialize_imuData_t(imuData_t *s, char *buff_ptr);

void acclrmThread(void const *arg) {
	uint8_t inBuffer[6];
	imuData_t *acclrmData;
	
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
	imuData_t *gyroData;
	
	gyroData = (imuData_t *) osMailAlloc(imuMailBox_id, osWaitForever);
	
	readGyroXYZ(inBuffer);
	
	while(1) {	
		osSignalWait(gyroSignal, osWaitForever);
		
		readGyroXYZ(inBuffer);
		
		gyroData->type = tGyro;
		gyroData->x = (float) (int16_t) (inBuffer[0] | (inBuffer[1] << 8));
		gyroData->y = (float) (int16_t) (inBuffer[2] | (inBuffer[3] << 8));
		gyroData->z = (float) (int16_t) (inBuffer[4] | (inBuffer[5] << 8));
		
		osMailPut(imuMailBox_id, gyroData);
	}
}

void protoThread(void const *arg) {
	const float dt = 0.01;
	const float g = 9.81;
	uint32_t i = 0;
	
	volatile float a,b, ad,bd;
	
	char uartBuffer[100];
	size_t len = 0;
	osEvent evt;
	imuData_t *imuData;
	
	while (1) {
		evt = osMailGet(imuMailBox_id, osWaitForever);
		
		imuData = (imuData_t *) evt.value.p;
		
//		sprintf(uartBuffer, "X: %5.2f, Y: %5.2f, Z: %5.2f\n", imuData->x, imuData->y, imuData->z);
//		uart_debug_sendString(uartBuffer, strlen(uartBuffer));
//		len = serialize_imuData_t(imuData, uartBuffer);
//		uart_debug_sendStream(uartBuffer, len);
		
		//if (imuData->type == tAcclrm) {
			//len = serialize_imuData_t(imuData, uartBuffer);
			//uart_debug_sendStream(uartBuffer, len);
			
			a = atanf(imuData->x/imuData->z);
			b = atanf(imuData->y/imuData->z);
			
			ad = a/2/M_PIf*360;
			bd = b/2/M_PIf*360;
			
			phy.x_a = (float)-1.0*arm_sin_f32(a);
			phy.y_a = arm_sin_f32(b);
			
			phy.x_v = phy.x_v + phy.x_a;
			phy.y_v = phy.y_v + phy.y_a;

			phy.x = phy.x + phy.x_v;
			phy.y = phy.y + phy.y_v;
			
			
			//printf("% 2.8f % 2.8f % 2.8f\n", phy.x_a, phy.x_v, phy.x);
			
			move_obj(&ball_bitmap, phy.x , phy.y);
		//}
		
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

