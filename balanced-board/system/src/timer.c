/* 
* Implement a periodic timer using CMSIS-RTOS RTX to 
* monitor the system's health
*/
#include <stm32f3xx.h>
#include <cmsis_os.h>
#include <timer.h>

uint32_t exec2;

static void LED_Timer_Callback (void const *arg);

static osTimerId LED_Timer_ID;                    // timer id
static osTimerDef (Timer2, LED_Timer_Callback);		// when the timer expires LED_Timer_Callback is exectued

static void LED_Timer_Callback (void const *arg) {
  HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_9);
	
}

void systemTimers_Init(void) {
	exec2 = 2;
	// Create periodic timer
  LED_Timer_ID = osTimerCreate(osTimer(Timer2), osTimerPeriodic, &exec2);
	
  if (LED_Timer_ID != NULL) {
		// Periodic timer created
    if(osTimerStart (LED_Timer_ID, 500) != osOK) {
      // Timer could not be started, handle error
    }
  }
}
