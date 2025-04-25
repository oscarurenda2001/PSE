#include <stdio.h>
#include <stdlib.h>

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "em_chip.h"
#include "em_cmu.h"
#include "bsp.h"
#include "bsp_trace.h"
#include "segmentlcd.h"

// --------------------------------------------------
// Definiciones FreeRTOS
// --------------------------------------------------
#define STACK_SIZE_FOR_TASK   (configMINIMAL_STACK_SIZE + 50)
#define TASK_PRIORITY         (tskIDLE_PRIORITY + 1)

// Struct para enviar por cola
typedef struct {
    uint32_t valor;
} Mensaje_t;

static QueueHandle_t xQueue;

// --------------------------------------------------
// Variable global de lectura continua
// --------------------------------------------------
volatile uint32_t valorActual = 0;

// --------------------------------------------------
// Prototipos
// --------------------------------------------------
void mostrarNumero(uint32_t valor);
uint32_t leerValor(void);
static void vTaskLeer(void *pvParameters);
static void vTaskMostrar(void *pvParameters);
static void vTaskBlink(void *pvParameters);

// --------------------------------------------------
// Función principal
// --------------------------------------------------
int main(void)
{
    // Inicializaciones básicas
    CHIP_Init();
    BSP_TraceProfilerSetup();
    CMU_ClockEnable(cmuClock_HFPER, true);
    CMU_ClockEnable(cmuClock_LCD,  true);
    BSP_LedsInit();
    SegmentLCD_Init(false);

    // Crea la cola para paso de mensajes
    xQueue = xQueueCreate( 10, sizeof(Mensaje_t) );

    // Crear tasks
    xTaskCreate(vTaskLeer,   "Leer",   STACK_SIZE_FOR_TASK, NULL, TASK_PRIORITY, NULL);
    xTaskCreate(vTaskMostrar,"Mostrar",STACK_SIZE_FOR_TASK, NULL, TASK_PRIORITY, NULL);
    xTaskCreate(vTaskBlink,  "Blink",  STACK_SIZE_FOR_TASK, NULL, TASK_PRIORITY, NULL);

    // Arranca el scheduler
    vTaskStartScheduler();

    // Nunca debe llegar aquí
    for(;;);
    return 0;
}

// --------------------------------------------------
// Task: lee un valor cada 1 s y lo envía por cola
// --------------------------------------------------
static void vTaskLeer(void *pvParameters)
{
    (void) pvParameters;
    const TickType_t xDelay = pdMS_TO_TICKS(1000);
    Mensaje_t msg;

    for (;;)
    {
        // Simular lectura: aquí podrías leer ADC, UART, sensor, etc.
        valorActual = leerValor();
        msg.valor = valorActual;

        // Enviar a la cola (espera hasta 100 ms si está llena)
        xQueueSend(xQueue, &msg, pdMS_TO_TICKS(100));

        vTaskDelay(xDelay);
    }
}

// --------------------------------------------------
// Task: recibe valor de la cola y actualiza la LCD
// --------------------------------------------------
static void vTaskMostrar(void *pvParameters)
{
    (void) pvParameters;
    Mensaje_t msg;

    for (;;)
    {
        // Espera bloqueo hasta recibir un mensaje
        if (xQueueReceive(xQueue, &msg, portMAX_DELAY) == pdPASS)
        {
            mostrarNumero(msg.valor);
        }
    }
}

// --------------------------------------------------
// Task: parpadea LED0 cada 500 ms
// --------------------------------------------------
static void vTaskBlink(void *pvParameters)
{
    (void) pvParameters;
    const TickType_t xDelay = pdMS_TO_TICKS(500);

    for (;;)
    {
        BSP_LedToggle(0);
        vTaskDelay(xDelay);
    }
}

// --------------------------------------------------
// Lee el valor de la fuente (aquí simulado)
// --------------------------------------------------
uint32_t leerValor(void)
{
    static uint32_t contador = 0;
    // Ejemplo: incrementa de 0 a 2000 y vuelve a 0
    contador = (contador < 2000) ? contador + 1 : 0;
    return contador;
}

// --------------------------------------------------
// Muestra un número en la LCD de segmentos
// --------------------------------------------------
void mostrarNumero(uint32_t valor)
{
    if (valor > 2000) {
        SegmentLCD_Write("Err");
    } else {
        SegmentLCD_Number((int)valor);
    }
}
