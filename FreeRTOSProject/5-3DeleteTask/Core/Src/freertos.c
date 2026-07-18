/* USER CODE BEGIN Header */
#include "driver_led.h"
#include "driver_lcd.h"
#include "driver_mpu6050.h"
#include "driver_timer.h"
#include "driver_ds18b20.h"
#include "driver_dht11.h"
#include "driver_active_buzzer.h"
#include "driver_passive_buzzer.h"
#include "driver_color_led.h"
#include "driver_ir_receiver.h"
#include "driver_ir_sender.h"
#include "driver_light_sensor.h"
#include "driver_ir_obstacle.h"
#include "driver_ultrasonic_sr04.h"
#include "driver_spiflash_w25q64.h"
#include "driver_rotary_encoder.h"
#include "driver_motor.h"
#include "driver_key.h"
#include "driver_uart.h"

/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
static StackType_t g_pucStackOfLightTask[128];
static StaticTask_t g_TCBOfLightTask;		// 光任务TCB
static TaskHandle_t xLightTaskHandle;			// 光任务的任务句柄，接收静态分配内存任务创建函数的返回值

static StackType_t g_pucStackOfColorTask[128];
static StaticTask_t g_TCBOfColorTask;		// 色任务TCB
static TaskHandle_t xColorTaskHandle;			// 色任务的任务句柄，接收静态分配内存任务创建函数的返回值

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
	
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
	defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* 创建任务：声：无源蜂鸣器播放音乐 */
//	extern void PlayMusic(void *params);
//	ret = xTaskCreate(PlayMusic, "SoundTask", 128, NULL, osPriorityNormal, &xSoundTaskHandle);
  
  /* 创建任务：光：板载LED闪烁 */
	xLightTaskHandle = xTaskCreateStatic(Led_Test, "LightTask", 128, NULL, osPriorityNormal, g_pucStackOfLightTask, &g_TCBOfLightTask);
  
  /* 创建任务：色：全彩LED变色 */
	xColorTaskHandle = xTaskCreateStatic(ColorLED_Test, "ColorTask", 128, NULL, osPriorityNormal, g_pucStackOfColorTask, &g_TCBOfColorTask);
	
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */

/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
	LCD_Init();
	LCD_Clear();
	LCD_PrintString(0, 0, "Waiting");
	
	PassiveBuzzer_Init();
	PassiveBuzzer_Control(0);		// 停止蜂鸣器
  
	uint8_t dev, data;
    uint8_t len;
	TaskHandle_t xSoundTaskHandle = NULL;			// 声音任务的任务句柄
	BaseType_t ret;		// 接收任务创建函数的返回值
	
    IRReceiver_Init();

    while (1)
    {
        // 读取红外遥控器
		if(IRReceiver_Read(&dev, &data) == 0) {
			if(data == 0xa8) {		// 按下Play
				// 创建播放音乐任务
				extern void PlayMusic(void *params);
				if(xSoundTaskHandle == NULL) {
					ret = xTaskCreate(PlayMusic, "SoundTask", 128, NULL, osPriorityNormal, &xSoundTaskHandle);
					LCD_Clear();
					LCD_PrintString(0, 0, "Task Created");
				}
			} else if(data == 0xa2) {		// 按下Power
				// 删除播放音乐任务
				if(xSoundTaskHandle != NULL) {
					vTaskDelete(xSoundTaskHandle);
					xSoundTaskHandle = NULL;
					PassiveBuzzer_Control(0);		// 停止蜂鸣器
					LCD_Clear();
					LCD_PrintString(0, 0, "Task Deleted");
				}
			}
		}
    }
}
  /* USER CODE END StartDefaultTask */

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

