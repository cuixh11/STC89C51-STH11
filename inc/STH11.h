/*
*******************************************************************************
*文 件 名：STH11.h
*描    述：第二部分SHT11设置的头文件
*  版本号：v1.0.0  
;微处理器: STC89C51
;编译环境: Keil uVision V4.10 
;作    者: Cui Xinghai 
;创建日期: 2022.2.24   
; 备   注：
*******************************************************************************
*/
#include <reg52.h>

#ifndef _STH11_H
#define _STH11_H

sbit SCK  = P2^6;      //定义通讯时钟端口
sbit DATA = P2^7;      //定义通讯数据端口

typedef union  		   //定义了两个共用体:如果没有typedef那么就是普通的定义了匿名联合的一个变量value.加了typedef后, 定义的就是类型别名, 当类型一样用
{ 
int i;      		   //i表示测量得到的温湿度数据（int 形式保存的数据）
float f; 			   //f表示测量得到的温湿度数据（float 形式保存的数据）
} value; 

enum {TEMP,HUMI};      //enum　枚举名　{枚举元素1,枚举元素2,……};TEMP=0,HUMI=1
 

#define noACK        0      //用于判断是否结束通讯
#define ACK          1      //结束数据传输
                            //adr  command  r/w 
#define STATUS_REG_W 0x06   //000   0011    0 
#define STATUS_REG_R 0x07   //000   0011    1 
#define MEASURE_TEMP 0x03   //000   0001    1 
#define MEASURE_HUMI 0x05   //000   0010    1 
#define RESET        0x1e   //000   1111    0 


/****************定义函数****************/
void s_transstart(void);               //启动传输函数
void s_connectionreset(void);          //连接复位函数
char s_write_byte(unsigned char value);//写函数
char s_read_byte(unsigned char ack);   //读函数
char s_measure(unsigned char *p_value, unsigned char *p_checksum, unsigned char mode);//测量温湿度函数
void calc_SHT11(float *p_humidity ,float *p_temperature);//温湿度补偿

#endif


