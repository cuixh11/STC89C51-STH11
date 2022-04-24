/*
*******************************************************************************
*�� �� ����LCD1602.h
*��    ������һ����LCD1602���õ�ͷ�ļ�
*  �汾�ţ�v1.0.0  
;΢������: STC89C51
;���뻷��: Keil uVision V4.10 
;��    ��: Cui Xinghai 
;��������: 2022.2.24   
; ��   ע��
*******************************************************************************
*/
#include <reg52.h>

#ifndef _LCD1602_H
#define _LCD1602_H

#define      LCD_DB P0	     //Data Bus��������,��������Ҫ�� 1602 Һ���ĳ�������Ȱ��õ������߽ӿ���һ��ͳһ����
sbit         LCD_RS=P2^0;   
sbit         LCD_RW=P2^1;   
sbit         LCD_E =P2^2;  
		  
/******���庯��****************/
#define uchar unsigned char
#define uint unsigned int

void LCD_init(void);                           //��ʼ������
void LCD_write_command(uchar command);         //дָ���
void LCD_write_data(uchar dat);                //д���ݺ���
void LCD_disp_char(uchar x,uchar y,uchar dat); //��ĳ����Ļλ������ʾһ���ַ�
void LCD_disp_str(uchar x,uchar y,uchar *str); //LCD1602��ʾ�ַ�������
void delay_n10us(uint n);                      //��ʱ����

#endif


