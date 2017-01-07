#ifndef _THREADS_H
#define _THREADS_H

#include <cmsis_os.h>

typedef enum {
	tAcclrm,
	tGyro
} imuTypes_t;

typedef struct {
	imuTypes_t type;
	float x;
	float y;
	float z;
} imuData_t;

typedef struct {
	float x_a;
	float y_a;
	float x_v;
	float y_v;
	float x;
	float y;
} physics_t;

enum bbSignals {
	acclrmSignal = 0x01,
	gyroSignal = 0x02
};

void kalman(float *z);
void acclrmThread(void const *);
void gyroThread(void const *);
void protoThread(void const *);

extern osThreadId acclrmThread_id;
extern osThreadId gyroThread_id;
extern osThreadId protoThread_id;

extern osMailQId imuMailBox_id;

#endif	// _THREADS_H
