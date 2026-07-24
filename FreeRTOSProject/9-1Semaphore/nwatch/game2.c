/*
 * Project: N|Watch
 * Author: Zak Kemble, contact@zakkemble.co.uk
 * Copyright: (C) 2013 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/diy-digital-wristwatch/
 */

#include <stdlib.h>
#include <stdio.h>

#include "cmsis_os.h"
#include "FreeRTOS.h"                   // ARM.FreeRTOS::RTOS:Core
#include "task.h"                       // ARM.FreeRTOS::RTOS:Core
#include "event_groups.h"               // ARM.FreeRTOS::RTOS:Event Groups
#include "semphr.h"                     // ARM.FreeRTOS::RTOS:Core

#include "draw.h"
#include "resources.h"

#include "driver_lcd.h"
#include "driver_ir_receiver.h"
#include "driver_rotary_encoder.h"
#include "driver_mpu6050.h"

#define CAR_COUNT	3
#define CAR_WIDTH	12
#define CAR_LENGTH	15
#define ROAD_SPEED	6

#define NOINVERT	false
#define INVERT		true
	
static SemaphoreHandle_t g_xSemTicks;
	
struct car {
	int x;
	int y;
	int control_key;
};

struct car g_cars[3] = {
	{0, 0, IR_KEY_1},
	{0, 17, IR_KEY_2},
	{0, 34, IR_KEY_3},
};

static uint32_t g_xres, g_yres, g_bpp;
static uint8_t *g_framebuffer;

static const byte carImg[] PROGMEM ={
	0x40,0xF8,0xEC,0x2C,0x2C,0x38,0xF0,0x10,0xD0,0x30,0xE8,0x4C,0x4C,0x9C,0xF0,
	0x02,0x1F,0x37,0x34,0x34,0x1C,0x0F,0x08,0x0B,0x0C,0x17,0x32,0x32,0x39,0x0F,
};

static const byte roadMarking[] PROGMEM ={
	0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
};

static const byte clearImg[30] ={0};

#if 0
void car_test()
{
	g_framebuffer = LCD_GetFrameBuffer(&g_xres, &g_yres, &g_bpp);
    draw_init();
    draw_end();
	
	// Draw car
    draw_bitmap(0, 0, carImg, 15, 16, NOINVERT, 0);
    draw_flushArea(0, 0, 15, 16);
	
	// Draw marking
    draw_bitmap(0, 16, roadMarking, 8, 1, NOINVERT, 0);
    draw_flushArea(0, 16, 8, 1);
	
	while(1) {
		
	}
}
#endif

// 显示汽车
static void ShowCar(struct car *pcar)
{
	// Draw car
    draw_bitmap(pcar->x, pcar->y, carImg, 15, 16, NOINVERT, 0);
    draw_flushArea(pcar->x, pcar->y, 15, 16);
}

// 隐藏汽车
static void HideCar(struct car *pcar)
{
	// Draw car
    draw_bitmap(pcar->x, pcar->y, clearImg, 15, 16, NOINVERT, 0);
    draw_flushArea(pcar->x, pcar->y, 15, 16);
}

static void Car1Task(void *params)
{
	struct car *pcar = params;
	struct ir_data idata;
	
	/* 创建自己的队列 */
	QueueHandle_t xQueueIR = xQueueCreate(10, sizeof(struct ir_data));
	
	/* 注册队列 */
	RegisterQueueHandle(xQueueIR);

	/* 显示汽车 */
	ShowCar(pcar);
	
	/* 获得信号量 */
	xSemaphoreTake(g_xSemTicks, portMAX_DELAY);
	
	while (1)
	{
		/* 读取按键值:读队列 */
		//xQueueReceive(xQueueIR, &idata, portMAX_DELAY);
		
		/* 控制汽车往右移动 */
		//if (idata.val == pcar->control_key)
		{
			if (pcar->x < g_xres - CAR_LENGTH)
			{
				/* 隐藏汽车 */
				HideCar(pcar);
				
				/* 调整位置 */
				pcar->x += 1;
				if (pcar->x > g_xres - CAR_LENGTH)
				{
					pcar->x = g_xres - CAR_LENGTH;
				}
				
				/* 重新显示汽车 */
				ShowCar(pcar);
				
				vTaskDelay(50);
				
				if (pcar->x == g_xres - CAR_LENGTH)
				{
					/* 释放信号量 */
					xSemaphoreGive(g_xSemTicks);
					vTaskDelete(NULL);
				}
			}
		}
	}
}

static void Car2Task(void *params)
{
	struct car *pcar = params;
	struct ir_data idata;
	
	/* 创建自己的队列 */
	QueueHandle_t xQueueIR = xQueueCreate(10, sizeof(struct ir_data));
	
	/* 注册队列 */
	RegisterQueueHandle(xQueueIR);

	/* 显示汽车 */
	ShowCar(pcar);
	
	vTaskDelay(1000);
	
	/* 获得信号量 */
	//xSemaphoreTake(g_xSemTicks, portMAX_DELAY);
	
	while (1)
	{
		/* 读取按键值:读队列 */
		//xQueueReceive(xQueueIR, &idata, portMAX_DELAY);
		
		/* 控制汽车往右移动 */
		//if (idata.val == pcar->control_key)
		{
			if (pcar->x < g_xres - CAR_LENGTH)
			{
				/* 隐藏汽车 */
				HideCar(pcar);
				
				/* 调整位置 */
				pcar->x += 1;
				if (pcar->x > g_xres - CAR_LENGTH)
				{
					pcar->x = g_xres - CAR_LENGTH;
				}
				
				/* 重新显示汽车 */
				ShowCar(pcar);
				
				//vTaskDelay(50);
				mdelay(50);
				
				if (pcar->x == g_xres - CAR_LENGTH)
				{
					/* 释放信号量 */
					//xSemaphoreGive(g_xSemTicks);
					vTaskDelete(NULL);
				}
			}
		}
	}
}


static void Car3Task(void *params)
{
	struct car *pcar = params;
	struct ir_data idata;
	
	
	/* 创建自己的队列 */
	QueueHandle_t xQueueIR = xQueueCreate(10, sizeof(struct ir_data));
	
	/* 注册队列 */
	RegisterQueueHandle(xQueueIR);

	/* 显示汽车 */
	ShowCar(pcar);

	vTaskDelay(2000);
	
	/* 获得信号量 */
	xSemaphoreTake(g_xSemTicks, portMAX_DELAY);
	
	while (1)
	{
		/* 读取按键值:读队列 */
		//xQueueReceive(xQueueIR, &idata, portMAX_DELAY);
		
		/* 控制汽车往右移动 */
		//if (idata.val == pcar->control_key)
		{
			if (pcar->x < g_xres - CAR_LENGTH)
			{
				/* 隐藏汽车 */
				HideCar(pcar);
				
				/* 调整位置 */
				pcar->x += 1;
				if (pcar->x > g_xres - CAR_LENGTH)
				{
					pcar->x = g_xres - CAR_LENGTH;
				}
				
				/* 重新显示汽车 */
				ShowCar(pcar);
				
				//vTaskDelay(50);
				mdelay(50);
				
				if (pcar->x == g_xres - CAR_LENGTH)
				{
					/* 释放信号量 */
					xSemaphoreGive(g_xSemTicks);
					vTaskDelete(NULL);
				}
			}
		}
	}
}

static void CarTask(void *params)
{
	struct car *pcar = params;
	struct ir_data idata;	
	
	// 创建自己的队列
	QueueHandle_t xQueueIR = xQueueCreate(10, sizeof(struct ir_data));
	// 注册队列：把队列加到IRReceiver中的队列数组里，方便数据分发
	RegisterQueueHandle(xQueueIR);
	
	// 显示汽车
	ShowCar(pcar);
	
	// 获得信号量
	xSemaphoreTake(g_xSemTicks, portMAX_DELAY);
	
	while(1) {		
		if(pcar->x < g_xres - CAR_LENGTH) {
				// 隐藏汽车
				HideCar(pcar);
				
				// 调整位置
				pcar->x += 1;
				if(pcar->x > g_xres - CAR_LENGTH) {
					pcar->x = g_xres - CAR_LENGTH;
				}
				
				// 重新显示
				ShowCar(pcar);	
				
				vTaskDelay(50);
				
				if(pcar->x == g_xres - CAR_LENGTH) {
					xSemaphoreGive(g_xSemTicks);
					vTaskDelete(NULL);
				}
		}
	}
}

void car_game()
{
	int x, i, j;
	g_framebuffer = LCD_GetFrameBuffer(&g_xres, &g_yres, &g_bpp);
    draw_init();
    draw_end();
	
	// 创建计数型信号量
	// g_xSemTicks = xSemaphoreCreateCounting(3, 1);
	// 创建二进制信号量
	// g_xSemTicks = xSemaphoreCreateBinary();
	// 由于初始值为0，先进行一次Give
	// xSemaphoreGive(g_xSemTicks);
	// 创建互斥量
	g_xSemTicks =  xSemaphoreCreateMutex();
	
	// 绘制路标
	for(i = 0; i < 3; i ++) {
		for(j = 0; j < 8; j ++) {
			draw_bitmap(16 * j, 16 + 17 * i, roadMarking, 8, 1, NOINVERT, 0);
			draw_flushArea(16 * j, 16 + 17 * i, 8, 1);
		}
	}

#if 0	
	// Draw car
	for(i = 0; i < 3; i ++) {
		draw_bitmap(g_cars[i].x, g_cars[i].y, carImg, 15, 16, NOINVERT, 0);
		draw_flushArea(g_cars[i].x, g_cars[i].y, 15, 16);
	}
#endif
	
	// 创建三个汽车任务
	xTaskCreate(Car1Task, "car1", 128, &g_cars[0], osPriorityNormal, NULL);
	xTaskCreate(Car2Task, "car2", 128, &g_cars[1], osPriorityNormal + 2, NULL);
	xTaskCreate(Car3Task, "car3", 128, &g_cars[2], osPriorityNormal + 3, NULL);
	
	
}
