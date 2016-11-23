/* 
* Implement a periodic timer using CMSIS-RTOS RTX to 
* monitor the system's health
*/
#include <stm32f3xx.h>
#include <cmsis_os.h>
#include <threads.h>

extern FILE __stdout;

static void Timer2_Callback (void const *arg); 

static uint32_t counter;
static osTimerId id2;                                           // timer id
static uint32_t  exec2;                                         // argument for the timer call back function
static osTimerDef (Timer2, Timer2_Callback);

static void Timer2_Callback (void const *arg) {
  HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_9);
	counter++;
	
	//osMutexWait(accelBuffer_mutex_id, osWaitForever);
	//osMutexWait(gyroBuffer_mutex_id, osWaitForever);
	ITM_SendChar('A');
	ITM_SendChar('\n');
	//printf("%d,%d,%d,%d,%d,%d\n", a.x, a.y, a.z, o.x, o.y, o.z);
	
	//osMutexRelease(gyroBuffer_mutex_id);
	//osMutexRelease(accelBuffer_mutex_id);
}

void Init_Timers (void) {
	osStatus status;
	// Create periodic timer
  exec2 = 2;
  id2 = osTimerCreate(osTimer(Timer2), osTimerPeriodic, &exec2);
  if (id2 != NULL) {    // Periodic timer created
    // start timer with periodic 1000ms interval
    status = osTimerStart (id2, 500);            
    if (status != osOK) {
      // Timer could not be started
    }
  }
}
