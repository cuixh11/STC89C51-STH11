# STC89C51 STH11 temperature and humidity acquisition and measurement system
 *任务书-基于STC89C51和STH11温湿度采集和测量系统的Proteus仿真* 

## 1. Task content of the project: 课题的任务内容
  Using Protues simulation software design to low power STC89C51 microcontroller as the core, equipped with a new type of low power sensor, LCD display LED data collection, temperature, humidity two parameters of the collection, processing and display functions. It is required to complete the design of temperature and humidity measurement circuit, SCM peripheral circuit, software flow, assembly language source program, data acquisition system design process and simulation.
  
 >  采用 Protues仿真软件设计以低功耗STC89C51单片机为核心，配置新式的微型低功耗传感器，使用液晶显示LED采集到的数据，实现温度、湿度两个参数的釆集、处理和显示等功能。要求完成温、湿度测量电路、单片机外围电路、软件流程、汇编语言源程序的设计、数据采集系统的设计过程及仿真等。
   
## 2.Technical requirements of design: 设计的技术要求
The software part of the temperature and humidity measurement system is based on Keil as the development platform, C language as the development language of the software system, and adopts modular programming. It is divided into the following parts: master control, temperature and humidity acquisition program, temperature and humidity data processing program, LCD display program, key setting program and LED, buzzer alarm program. The system uses SHT11 temperature and humidity sensor to sense the temperature and humidity of the surrounding environment, reads and processes the collected data through the microcontroller, and displays the temperature and humidity data in real time through the LCD1602 display module. At the same time, the upper and lower limits of temperature and humidity alarm can be set through the key module. When the temperature and humidity value read by SHT11 is not within the set range, the LED light of the alarm module indicates the fault information, and the buzzer alarms at the same time. When the temperature and humidity readings are normal, the LED indicator is off and the buzzer is off

>温湿度测量系统的软件部分是以Keil为开发平台，C语言为软件系统的开发语言,同时采用模块化编程。具体分为以下几个部分：主控制、温湿度采集程序、温湿度数据处理程序、LCD显示程序、按键设置程序和LED，蜂鸣器报警程序。系统通过SHT11温湿度传感器感应周围的环境的温度和湿度，通过单片机对采集到的数据进行读取处理，经过LCD1602显示模块实时显示温湿度数据，同时可以通过按键模块对温湿度报警上、下限值进行设定。当SHT11读取的温湿度值不在设定范围内时，报警模块LED灯指示故障信息，同时蜂鸣器报警；当温湿度读取数据正常后，LED灯熄灭，蜂鸣器关闭

## 仿真文件和论文报告完整版：
[链接](https://pan.baidu.com/s/1fEXZ7fWfRJ3dYbX0LhA3Eg?pwd=qgwq),提取码： qgwq 复制这段内容后打开百度网盘手机App，操作更方便哦




