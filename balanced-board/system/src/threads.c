#include <stm32f3xx.h>
#include <cmsis_os.h>

#include <lsm303dlhc.h>
#include <l3gd20.h>
#include <threads.h>
#include <uart_debug.h>

osThreadId accelHandlerThread_id, gyroHandlerThread_id;
osThreadId visioThread_id;

osMutexId accelBuffer_mutex_id;
osMutexId gyroBuffer_mutex_id;

struct acceleration a;
struct orientation o;

void accelHandlerThread(void const *arg) {
	osEvent event;
	uint8_t buffer[6];
	
	while(1) {
		event = osSignalWait(0x01, osWaitForever);
		
		if (event.status == osOK) {
			// TODO: handle event, really?
		}
		
		readAccel(buffer);
		
		osMutexWait(accelBuffer_mutex_id, osWaitForever);
		a.x = buffer[0] | (buffer[1] << 8);
		a.y = buffer[2] | (buffer[3] << 8);
		a.z = buffer[4] | (buffer[5] << 8);
		osMutexRelease(accelBuffer_mutex_id);
	}
}

void gyroHandlerThread(void const *arg) {
	osEvent event;
	uint8_t buffer[6];
	
	while(1) {	
		event = osSignalWait(0x02, osWaitForever);
		
		if (event.status == osOK) {
			// TODO: handle event, really?
		}
		
		readGyro(buffer);
		
		osMutexWait(gyroBuffer_mutex_id, osWaitForever);
		o.x = buffer[0] | (buffer[1] << 8);
		o.y = buffer[2] | (buffer[3] << 8);
		o.z = buffer[4] | (buffer[5] << 8);
		osMutexRelease(gyroBuffer_mutex_id);
	}
}

void visioThread(void const *arg) {
	volatile static struct acceleration a;
	volatile static struct orientation o;
	
	a.x = 0;
	a.y = 0;
	a.z = 0;
	
	o.x = 0;
	o.y = 0;
	o.z = 0;
	
	while(1) {
		//event = osSignalWait(0x03, osWaitForever);
		/*
		osMutexWait(accelBuffer_mutex_id, osWaitForever);
		a.x = accelBuffer[0] | (accelBuffer[1] << 8);
		a.y = accelBuffer[2] | (accelBuffer[3] << 8);
		a.z = accelBuffer[4] | (accelBuffer[5] << 8);
		osMutexRelease(accelBuffer_mutex_id);
		
		osMutexWait(gyroBuffer_mutex_id, osWaitForever);
		o.x = gyroBuffer[0] | (gyroBuffer[1] << 8);
		o.y = gyroBuffer[2] | (gyroBuffer[3] << 8);
		o.z = gyroBuffer[4] | (gyroBuffer[5] << 8);
		osMutexRelease(gyroBuffer_mutex_id);
		*/
		printf("A%d,%d,%d\n", a.x, a.y, a.z);
		
		osThreadYield();
	}
}
