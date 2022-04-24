/*
*******************************************************************************
*文 件 名：LCD1602.h
*描    述：第一部分LCD1602设置的头文件
*  版本号：v1.0.0  
;微处理器: STC89C51
;编译环境: Keil uVision V4.10 
;作    者: Cui Xinghai 
;创建日期: 2022.2.24   
; 备   注：
*******************************************************************************
*/
#include <reg52.h>

#ifndef _LCD1602_H
#define _LCD1602_H

#define      LCD_DB P0	     //Data Bus数据总线,我们这里要做 1602 液晶的程序，因此先把用到的总线接口做一个统一声明
sbit         LCD_RS=P2^0;   
sbit         LCD_RW=P2^1;   
sbit         LCD_E =P2^2;  
		  
/******定义函数****************/
#define uchar unsigned char
#define uint unsigned int

void LCD_init(void);                           //初始化函数
void LCD_write_command(uchar command);         //写指令函数
void LCD_write_data(uchar dat);                //写数据函数
void LCD_disp_char(uchar x,uchar y,uchar dat); //在某个屏幕位置上显示一个字符
void LCD_disp_str(uchar x,uchar y,uchar *str); //LCD1602显示字符串函数
void delay_n10us(uint n);                      //延时函数

#endif


