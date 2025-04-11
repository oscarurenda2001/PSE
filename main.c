
#include <stdio.h>
#include <stdlib.h>

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "croutine.h"

#include "em_chip.h"
#include "bsp.h"
#include "bsp_trace.h"

#include "bsp_i2c.h"


#define STACK_SIZE_FOR_TASK	(configMINIMAL_STACK_SIZE + 10)
#define TASK_PRIORITY      	(tskIDLE_PRIORITY + 1)
QueueHandle_t xQueue;
/***************************************************************************//**
 * @brief Simple task which is blinking led
 * @param *pParameters pointer to parameters passed to the function
 ******************************************************************************/
struct Message
{
	int contador;
	int numero;
}Message;

static void LedBlink(void *pParameters)
{
  (void) pParameters;
  const portTickType delay = pdMS_TO_TICKS(500);
  for (;; ) {
	BSP_LedToggle(1);
	Message.contador += 1;
	Message.numero = 10;
	printf("Task 1\n");
	xQueueSend(xQueue, (void *) &Message, 1000);
	printf("Counter %d \n", Message.contador);
	printf("Lletra %d \n", Message.numero);
	vTaskDelay(delay);
  }
}

static void LedBlink2(void *pParameters)
{
  (void) pParameters;
  const portTickType delay = pdMS_TO_TICKS(1000);
  struct Message valor;
  for (;; ) {
	BSP_LedToggle(0);
	printf("Task 2\n");
	xQueueReceive(xQueue, &(valor), 1000);
	Message.contador = Message.contador + 1;
	xQueueSend(xQueue, (void *) &Message, 1000);
	vTaskDelay(delay);
  }
}
static void LedBlink3(void *pParameters)
{
  (void) pParameters;
  const portTickType delay = pdMS_TO_TICKS(2000);
  struct Message valor;
  for (;; ) {
	BSP_LedToggle(0);
	printf("Task 2\n");
	xQueueReceive(xQueue, &(valor), 1000);
	printf("Lecture %d \n", valor.contador );
	printf("Lecture lletra %d \n", valor.numero);
	vTaskDelay(delay);
  }
}

/***************************************************************************//**
 * @brief  Main function
 ******************************************************************************/
int main(void)
{
  /* Chip errata */
  CHIP_Init();
  /* If first word of user data page is non-zero, enable Energy Profiler trace */
  BSP_TraceProfilerSetup();

  /* Initialize LED driver */
  BSP_LedsInit();
  /* Setting state of leds*/
  BSP_LedSet(0);
  BSP_LedSet(1);
  xQueue = xQueueCreate( 10, sizeof( struct Message) );
  /*Create two task for blinking leds*/
  xTaskCreate(LedBlink, (const char *) "LedBlink1", STACK_SIZE_FOR_TASK, NULL, TASK_PRIORITY, NULL);
  xTaskCreate(LedBlink2, (const char *) "LedBlink2", STACK_SIZE_FOR_TASK, NULL, TASK_PRIORITY, NULL);
  xTaskCreate(LedBlink3, (const char *) "LedBlink3", STACK_SIZE_FOR_TASK, NULL, TASK_PRIORITY, NULL);
  /*Start FreeRTOS Scheduler*/
  vTaskStartScheduler();

  return 0;
}


