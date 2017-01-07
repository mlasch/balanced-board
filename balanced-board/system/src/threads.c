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

/* globals */
physics_t phy;
float x[4];		//state vector


/* private functions */
static size_t serialize_imuData_t(imuData_t *s, char *buff_ptr);

void kalman_filter(float *z) {
	memcpy(x, z, sizeof(float)*4);
}

void acclrmThread(void const *arg) {
	uint8_t inBuffer[6];
	imuData_t *acclrmData;
	
	readAcclrmXYZ(inBuffer);
	
	while(1) {
		osSignalWait(acclrmSignal, osWaitForever);
		
		readAcclrmXYZ(inBuffer);
		
		acclrmData = (imuData_t *) osMailAlloc(imuMailBox_id, osWaitForever);
		
		if (acclrmData == NULL) {
			// memory allocation error
			for(;;);
		}
		
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
	
	
	
	readGyroXYZ(inBuffer);
	
	while(1) {	
		osSignalWait(gyroSignal, osWaitForever);
		
		readGyroXYZ(inBuffer);
		
		gyroData = (imuData_t *) osMailAlloc(imuMailBox_id, osWaitForever);
		
		if (gyroData == NULL) {
			// memory allocation error
			for(;;);
		}
		
		gyroData->type = tGyro;
		gyroData->x = (float) (int16_t) (inBuffer[0] | (inBuffer[1] << 8));
		gyroData->y = (float) (int16_t) (inBuffer[2] | (inBuffer[3] << 8));
		gyroData->z = (float) (int16_t) (inBuffer[4] | (inBuffer[5] << 8));
		
		osMailPut(imuMailBox_id, gyroData);
	}
}

void protoThread(void const *arg) {
	const float dt = 1/(float)95.0;	// freq of gyro
	const float g = 9.81;
	
	float a = 0.0,b = 0.0;
	volatile float ad,bd;
	
	volatile float a_dt, b_dt;
	
	// Kalman variables
	static float z[4];		//measure vector
	
	char uartBuffer[100];
	size_t len = 0;
	
	osEvent evt;
	imuData_t *imuData;
	
	// initial position
	phy.x = WIDTH/2;
	phy.y = HEIGHT/2;
	
	// draw border and background
	draw_rect(0,0,WIDTH, HEIGHT, 0xcc40);
	draw_rect(BORDER, BORDER, WIDTH-BORDER, HEIGHT-BORDER, 0xffff);
	
	while (1) {
		evt = osMailGet(imuMailBox_id, osWaitForever);
		
		imuData = (imuData_t *) evt.value.p;
		
//		sprintf(uartBuffer, "X: %5.2f, Y: %5.2f, Z: %5.2f\n", imuData->x, imuData->y, imuData->z);
//		uart_debug_sendString(uartBuffer, strlen(uartBuffer));
//		len = serialize_imuData_t(imuData, uartBuffer);
//		uart_debug_sendStream(uartBuffer, len);
		
		if (imuData->type == tAcclrm) {
			
			z[0] = atanf(imuData->x/imuData->z);		// alpha
			z[1] = atanf(imuData->y/imuData->z);		// beta
			
			ad = z[0]/2/M_PIf*360;
			bd = z[1]/2/M_PIf*360;
			
			osMailFree(imuMailBox_id, imuData);
			
			continue;
			
		} else if (imuData->type == tGyro) {
			
			// read sensor data and normalize
			z[2] = imuData->x / (float)32768.0 * 2000;
			z[3] = imuData->y / (float)32768.0 * 2000;
			
			printf("% 2.4f % 2.4f\n", z[2], z[3]);
			
			kalman_filter(z);
			
			// ball physics calculation
			phy.x_a =  1*arm_sin_f32(x[0]) * g * 250;
			phy.y_a = -1*arm_sin_f32(x[1]) * g * 250;
			
			phy.x_v = phy.x_v + phy.x_a * dt;
			phy.y_v = phy.y_v + phy.y_a * dt;

			phy.x = phy.x + phy.x_v * dt;
			phy.y = phy.y + phy.y_v * dt;
			
			check_border(&phy);
			
			move_obj(&ball_bitmap, roundf(phy.x) , roundf(phy.y));
			
			osMailFree(imuMailBox_id, imuData);
			
			continue;
		}
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

