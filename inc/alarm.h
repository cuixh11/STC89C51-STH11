/*
*******************************************************************************
*文 件 名：alarm.h
*描    述：第三部分报警设置的头文件
*  版本号：v1.0.0  
;微处理器: STC89C51
;编译环境: Keil uVision V4.10 
;作    者: Cui Xinghai 
;创建日期: 2022.2.24   
; 备   注：
*******************************************************************************
*/
#include <reg52.h>

#ifndef _alarm_H
#define _alarm_H

#define N   0x00

sbit SET = 	P1^5;                   //定义调整键
sbit ADD =	P1^6;                   //定义增加键
sbit DEC = 	P1^7;                   //定义减少键
sbit ESC =  P3^7;                   //定义调整键
sbit BEEP=  P1^4;                   //定义蜂鸣器

sbit  t_green   =  P1^0;
sbit  t_red     =  P1^1;
sbit  rh_green  =  P1^2;
sbit  rh_red    =  P1^3;
/****************定义函数****************/
void led_control( float *WENDU,  float *SHIDU);
void key(void);
void delay(unsigned int j);







#endif


