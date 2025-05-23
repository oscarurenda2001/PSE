
#include <stdio.h>
#include <stdlib.h>

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "croutine.h"
#include "em_device.h"
#include "em_chip.h"
#include "em_gpio.h"
#include "bsp.h"
#include "bsp_trace.h"

#include "bsp_i2c.h"


#define STACK_SIZE_FOR_TASK	(configMINIMAL_STACK_SIZE + 10)
#define TASK_PRIORITY      	(tskIDLE_PRIORITY + 1)
#define distancia			17
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
	//printf("Task 1\n");
	xQueueSend(xQueue, (void *) &Message, 1000);
	/*printf("Counter %d \n", Message.contador);
	printf("Lletra %d \n", Message.numero);*/
	vTaskDelay(delay);
  }
}

static void ReadData(void *pParameters)
{
  (void) pParameters;
  const portTickType delay = pdMS_TO_TICKS(500);
  for (;; ) {
	  uint16_t range = 0;
	  //
	  xQueueSend(xQueue, (void *) &range, 1000);
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
	//printf("Task 2\n");
	xQueueReceive(xQueue, &(valor), 1000);
	Message.contador = Message.contador + 1;
	xQueueSend(xQueue, (void *) &Message, 1000);
	vTaskDelay(delay);
  }
}
static void DataWork(void *pParameters)
{
  (void) pParameters;
  const portTickType delay = pdMS_TO_TICKS(1000);
  uint16_t range;
  char led = "";
  for (;; ) {
	BSP_LedToggle(0);

	xQueueReceive(xQueue, &range, 1000);
	if(range < 25000){
		led = "r";
	}else{
		if(range >= 25000 && range < 35000){
			led = "y";
		}
		else{
			led = "g";
		}
	}
	xQueueSend(xQueue, (void *) &led, 1000);
	vTaskDelay(delay);
  }
}
/*static void LedBlink3(void *pParameters)
{
  (void) pParameters;
  const portTickType delay = pdMS_TO_TICKS(2000);
  struct Message valor;
  for (;; ) {
	BSP_LedToggle(0);
	//printf("Task 2\n");
	xQueueReceive(xQueue, &(valor), 1000);
	/*printf("Lecture %d \n", valor.contador );
	printf("Lecture lletra %d \n", valor.numero);
	vTaskDelay(delay);
  }
}*/
/*
static void readMetric(void *pParameters)
{
	(void) pParameters;
	const portTickType delay = pdMS_TO_TICKS(1000);
	uint8_t distance;
	for(;;){
	BSP_I2C_ReadRegister(0xC0, &distance);
	printf("%d\n",distance);
	vTaskDelay(delay);
	}
} Hauria de ser aixi pero no va el readRegister.
*/
/***************************************************************************//**
 * @brief  Main function
 ******************************************************************************/


int main(void)
{
  /* Chip errata */
  CHIP_Init();

  /* If first word of user data page is non-zero, enable Energy Profiler trace */
  BSP_TraceProfilerSetup();

  /* Inicializa I2C si lo necesitas */
  BSP_I2C_Init(0x52);

  /* Inicializa driver de LEDs del kit */
  BSP_LedsInit();

  /* Configura GPIOs de salida para los LEDs del semáforo */
  GPIO_PinModeSet(gpioPortD, 0, gpioModePushPull, 0); // Rojo
  GPIO_PinModeSet(gpioPortD, 1, gpioModePushPull, 0); // Amarillo
  GPIO_PinModeSet(gpioPortD, 2, gpioModePushPull, 0); // Verde

  /*Enciende LED rojo y amarillo al inicio */
  GPIO_PinOutSet(gpioPortD, 0); // Enciende Rojo
  GPIO_PinOutSet(gpioPortD, 1); // Enciende Amarillo
  GPIO_PinOutClear(gpioPortD, 2); // Asegura que el verde está apagado

  /* Crea cola de mensajes */
  xQueue = xQueueCreate(10, sizeof(struct Message));

  /* Crea tareas de FreeRTOS */
  xTaskCreate(LedBlink, "LedBlink1", STACK_SIZE_FOR_TASK, NULL, TASK_PRIORITY, NULL);
  xTaskCreate(LedBlink2, "LedBlink2", STACK_SIZE_FOR_TASK, NULL, TASK_PRIORITY, NULL);
  // xTaskCreate(readMetric, "readMetric", STACK_SIZE_FOR_TASK, NULL, TASK_PRIORITY, NULL); // Revisa esta línea
  xTaskCreate(LedBlink2, "LedBlink2", STACK_SIZE_FOR_TASK, NULL, TASK_PRIORITY, NULL);

  /* Inicia el planificador de FreeRTOS */
  vTaskStartScheduler();

  return 0;
}

