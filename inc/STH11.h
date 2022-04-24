/*
*******************************************************************************
*�� �� ����STH11.h
*��    �����ڶ�����SHT11���õ�ͷ�ļ�
*  �汾�ţ�v1.0.0  
;΢������: STC89C51
;���뻷��: Keil uVision V4.10 
;��    ��: Cui Xinghai 
;��������: 2022.2.24   
; ��   ע��
*******************************************************************************
*/
#include <reg52.h>

#ifndef _STH11_H
#define _STH11_H

sbit SCK  = P2^6;      //����ͨѶʱ�Ӷ˿�
sbit DATA = P2^7;      //����ͨѶ���ݶ˿�

typedef union  		   //����������������:���û��typedef��ô������ͨ�Ķ������������ϵ�һ������value.����typedef��, ����ľ������ͱ���, ������һ����
{ 
int i;      		   //i��ʾ�����õ�����ʪ�����ݣ�int ��ʽ��������ݣ�
float f; 			   //f��ʾ�����õ�����ʪ�����ݣ�float ��ʽ��������ݣ�
} value; 

enum {TEMP,HUMI};      //enum��ö������{ö��Ԫ��1,ö��Ԫ��2,����};TEMP=0,HUMI=1
 

#define noACK        0      //�����ж��Ƿ����ͨѶ
#define ACK          1      //�������ݴ���
                            //adr  command  r/w 
#define STATUS_REG_W 0x06   //000   0011    0 
#define STATUS_REG_R 0x07   //000   0011    1 
#define MEASURE_TEMP 0x03   //000   0001    1 
#define MEASURE_HUMI 0x05   //000   0010    1 
#define RESET        0x1e   //000   1111    0 


/****************���庯��****************/
void s_transstart(void);               //�������亯��
void s_connectionreset(void);          //���Ӹ�λ����
char s_write_byte(unsigned char value);//д����
char s_read_byte(unsigned char ack);   //������
char s_measure(unsigned char *p_value, unsigned char *p_checksum, unsigned char mode);//������ʪ�Ⱥ���
void calc_SHT11(float *p_humidity ,float *p_temperature);//��ʪ�Ȳ���

#endif


