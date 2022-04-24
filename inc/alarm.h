/*
*******************************************************************************
*�� �� ����alarm.h
*��    �����������ֱ������õ�ͷ�ļ�
*  �汾�ţ�v1.0.0  
;΢������: STC89C51
;���뻷��: Keil uVision V4.10 
;��    ��: Cui Xinghai 
;��������: 2022.2.24   
; ��   ע��
*******************************************************************************
*/
#include <reg52.h>

#ifndef _alarm_H
#define _alarm_H

#define N   0x00

sbit SET = 	P1^5;                   //���������
sbit ADD =	P1^6;                   //�������Ӽ�
sbit DEC = 	P1^7;                   //������ټ�
sbit ESC =  P3^7;                   //���������
sbit BEEP=  P1^4;                   //���������

sbit  t_green   =  P1^0;
sbit  t_red     =  P1^1;
sbit  rh_green  =  P1^2;
sbit  rh_red    =  P1^3;
/****************���庯��****************/
void led_control( float *WENDU,  float *SHIDU);
void key(void);
void delay(unsigned int j);







#endif


