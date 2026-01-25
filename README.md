# STM32 项目 - 传感器与外设驱动



## 简介
基于 STM32F10x 的嵌入式项目，实现 LED 点灯、DHT11 温湿度、MQ135 空气质量、OLED 显示、ESP8266 WiFi 等功能。使用 Keil MDK + STM32 标准库 / HAL。

## 硬件平台
- MCU：STM32F103C8T6 (Blue Pill)
- 外设：DHT11, MQ135, OLED (SSD1306), Buzzer, ESP8266 UART

## 目录结构
├── Hardware/         # 硬件驱动层
├── libraries/        # CMSIS + StdPeriph 库
├── project/          # Keil 工程文件 (.uvprojx)
├── user/             # 应用层代码 (main.c, app_*.c)
├── public/           # 公共头文件
└── .gitignore
