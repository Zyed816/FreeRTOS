# [FreeRTOS]Day1

## 创建FreeRTOS项目

运行STM32CubeMX，选择“Access to MCU Selector”

![image-20260712211411364](./assets/image-20260712211411364.png)

出现报错，点击OK即可

![image-20260712211547856](./assets/image-20260712211547856.png)

搜索并选择STM32F103C8T6

![image-20260712211720652](./assets/image-20260712211720652.png)

使用晶振作为外部高速时钟源

![image-20260712211858236](./assets/image-20260712211858236.png)

FreeRTOS的时基使用的是Systick，而STM32CubeMX中默认的HAL库时基也是Systick，为了避免可能的冲突，最好将HAL库的时基换做其它的硬件定时器

![image-20260712212100951](./assets/image-20260712212100951.png)

设置系统时钟频率为72MHz，在HCLK部分输入72

![image-20260712212203868](./assets/image-20260712212203868.png)

点击确定，完成系统时钟频率配置

![image-20260712212255401](./assets/image-20260712212255401.png)

配置FreeRTOS，参数使用默认参数即可

![image-20260712212545136](./assets/image-20260712212545136.png)

生成Keil MDK工程，首先设置工程名称和路径![image-20260712212801067](./assets/image-20260712212801067.png)

然后设置Code Generator部分选项

![image-20260712212914763](./assets/image-20260712212914763.png)

生成项目完成后，点击“Open Project”即可在Keil 5中打开工程，其分组结构为

![image-20260712214024383](./assets/image-20260712214024383.png)

各分组的作用分别为：

- `Application/User/Core`：用户代码和核心初始化代码；

- `Drivers/STM32F1xx_HAL_Driver`：HAL 驱动；

- `Drivers/CMSIS`：CMSIS 和芯片设备文件；

- `Middlewares/FreeRTOS`：FreeRTOS 中间件；

- `Application/MDK-ARM`：与 Keil 工具链有关的启动文件等。

后续使用STM32CubeMX+Keil+FreeRTOS开发时，通常只需要修改`Application/User/Core`分组，该分组下包括文件

```
main.c
freertos.c
stm32f1xx_it.c
stm32f1xx_hal_msp.c
gpio.c
usart.c
tim.c
adc.c
```

`main.c`主要负责：HAL初始化，系统时钟初始化，外设初始化，启动FreeRTOS调度器前的初始化工作

`freertos.c`是开发中最常修改的文件，通常包含：任务句柄，任务属性，任务创建，队列创建，信号量创建，互斥锁创建，软件定时器创建，默认任务函数

## FreeRTOS项目测试

### 目标一：板载LED闪烁

#### 配置GPIO

板载LED的IO口为PC13，在STM32CubeMx中设置PC13为开漏输出输出模式

![image-20260712220003612](./assets/image-20260712220003612.png)

点击GENERATE CODE，回到keil可以发现`gpio.c`中已经更新了对PC13的配置

![image-20260712220258121](./assets/image-20260712220258121.png)

#### 添加驱动代码

在Drivers/下新建DShanMCU-103存放驱动代码

![image-20260712220512936](./assets/image-20260712220512936.png)

在Keil中添加组并包含路径

![image-20260712220702182](./assets/image-20260712220702182.png)

![image-20260712220820841](./assets/image-20260712220820841.png)

设置烧录后立即执行

![image-20260712220850909](./assets/image-20260712220850909.png)

#### 修改`freeRTOS.c`

包含驱动头文件

![image-20260712221024158](./assets/image-20260712221024158.png)

修改`StartDefaultTask()`

![image-20260712221134474](./assets/image-20260712221134474.png)

烧录后可观察到板载LED闪烁

### 目标二：测试OLED显示屏

#### 配置GPIO

OLED使用I2C1通道，使用PB6，PB7作为SCL，SDA引脚，在STM32CubeMX中完成设置

![image-20260712222208996](./assets/image-20260712222208996.png)

![image-20260712222231244](./assets/image-20260712222231244.png)

`main.c`中增加了对I2C1的配置

![image-20260712222514753](./assets/image-20260712222514753.png)

#### 添加OLED驱动

![image-20260712221507178](./assets/image-20260712221507178.png)

#### 修改`freeRTOS.c`

`driver_oled.c`中包含字库头文件`ascii_font.h`

![image-20260713075144108](./assets/image-20260713075144108.png)

`freertos.c`中包含OLED驱动

![image-20260713075237536](./assets/image-20260713075237536.png)

修改`StartDefaultTask()`

![image-20260713075307848](./assets/image-20260713075307848.png)

烧录后可观察到OLED打印

```
AY
Hello World!
```

