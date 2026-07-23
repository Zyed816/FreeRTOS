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

// 任务函数：计算前10000000个数字之和
static uint32_t g_sum = 0;
static volatile uint8_t g_CalEnd = 0;
static uint64_t g_CalTime = 0;
void CalTask(void *param)
{
	uint32_t i = 0;
	g_CalTime = system_get_ns();
	
	for(i = 0; i < 10000000; i ++) {
		g_sum += i;
	}
	g_CalEnd = 1;
	g_CalTime = system_get_ns() - g_CalTime;
	vTaskDelete(NULL);
}

// 任务函数：在OLED指定位置打印
static uint8_t g_LCDCanUse = 1;

void OLEDPrintTask(void *param)
{
	uint8_t len;
	
	while(1) {
		// 等待计算完成
		LCD_PrintString(0, 0, "Waiting");
		vTaskDelay(2000);
		while(g_CalEnd == 0);
		
		/* 打印信息 */
		if(g_LCDCanUse) {
			g_LCDCanUse = 0;
			
			LCD_ClearLine(0, 0);
			len = LCD_PrintString(0, 0, "Sum:");
			LCD_PrintHex(len, 0, g_sum, 1);
			
			LCD_ClearLine(0, 2);
			len = LCD_PrintString(0, 2, "Time(ms):");
			LCD_PrintSignedVal(len, 2, g_CalTime / 1000000);
			
			g_LCDCanUse = 1;
		}
		vTaskDelete(NULL);
	}
}


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
	TaskHandle_t xSoundTaskHandle;			// 声音任务的任务句柄
	BaseType_t ret;		// 接收任务创建函数的返回值
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
//	defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* 创建任务：声：无源蜂鸣器播放音乐 */
	extern void PlayMusic(void *params);
//	ret = xTaskCreate(PlayMusic, "SoundTask", 128, NULL, osPriorityNormal, &xSoundTaskHandle);
  
  /* 创建任务：光：板载LED闪烁 */
//	xLightTaskHandle = xTaskCreateStatic(Led_Test, "LightTask", 128, NULL, osPriorityNormal, g_pucStackOfLightTask, &g_TCBOfLightTask);
  
  /* 创建任务：色：全彩LED变色 */
//	xColorTaskHandle = xTaskCreateStatic(ColorLED_Test, "ColorTask", 128, NULL, osPriorityNormal, g_pucStackOfColorTask, &g_TCBOfColorTask);
	
  /* 使用同一个任务函数创建不同任务 */
	LCD_Init();
	LCD_Clear(); 
	xTaskCreate(CalTask, "CalTask", 128, NULL, osPriorityNormal, NULL);
	xTaskCreate(OLEDPrintTask, "Task2", 128, NULL, osPriorityNormal, NULL);
	
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
  
  for(;;)
  {
    //Led_Test();
    //LCD_Test();
	//MPU6050_Test(); 
	//DS18B20_Test();
	//DHT11_Test();
	//ActiveBuzzer_Test();
	//PassiveBuzzer_Test();
	//ColorLED_Test();
	  IRReceiver_Test();  /* 默认任务执行监控遥控器并在OLED上显示 */
	//IRSender_Test();
	//LightSensor_Test();
	//IRObstacle_Test();
	//SR04_Test();
	//W25Q64_Test();
	//RotaryEncoder_Test();
	//Motor_Test();
	//Key_Test();
	//UART_Test();
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

