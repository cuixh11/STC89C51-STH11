/*
*******************************************************************************
*文 件 名：alarm.c
*描    述：第三部分报警设置
*  版本号：v1.0.0  
;微处理器: STC89C51
;编译环境: Keil uVision V4.10 
;作    者: Cui Xinghai 
;创建日期: 2022.2.24   
; 备   注：
*******************************************************************************
*/
#define  _alarm_C
#include "alarm.h"
#include "LCD1602.h"
#include "STH11.h"

#include <reg52.h>
#include <math.h>    //Keil library   

//*********************第三部分报警设置   START****************************************
sbit  t_green   =  P1^0;
sbit  t_red     =  P1^1;
sbit  rh_green  =  P1^2;
sbit  rh_red    =  P1^3;

int t_shangxian = 20;            //上限报警温度，默认值为 20C
int t_xiaxian   = 10;            //下限报警温度，默认值为 10C
int rh_shangxian= 50;            //上限报警湿度，默认值为50%
int rh_xiaxian  = 40;            //下限报警湿度，默认值为40%

unsigned char flag=0;

/*-------------------------------------- 
;模块名称:led_control(); 
;功    能:LED状态控制
;-------------------------------------*/ 
void led_control( float *WENDU,  float *SHIDU)                   
{
//温度过低报警指示
  if(*WENDU <= t_xiaxian)    {t_green = 0;}
  else {t_green=1;}
//温度过高报警指示
  if(*WENDU >= t_shangxian)  {t_red   = 0;}
  else {t_red=1;}
//湿度过低报警指示
  if(*SHIDU <= rh_xiaxian)   {rh_green= 0;}
  else {rh_green=1;}
//湿度过高报警指示
  if(*SHIDU >= rh_shangxian) {rh_red  = 0;}
  else {rh_red=1;}
//蜂鸣器警示
  if(*WENDU <= t_xiaxian||*WENDU >= t_shangxian||*SHIDU <= rh_xiaxian||*SHIDU >= rh_shangxian)
  {
    BEEP = 0;
  }
  else 
  {
    BEEP = 1;
  }
}

           
/*-------------------------------------- 
;模块名称:key(); 
;功    能:按键控制
;-------------------------------------*/ 
void key()   
{
/****************设置键设置****************/
 if(SET == 0)
  {
    delay(50);               //按键消抖
    if(SET == 0)
     {
      flag++;
      if (flag==5)flag = 0;  //没有发生抖动
	  while(SET==0);         //若一直按下，循环

    LCD_init(); 
    s_connectionreset();     
    LCD_disp_str(0,1,"TE:");
    LCD_disp_str(0,2,"RH:"); 
	LCD_disp_str(6,1,"-"); 
	LCD_disp_str(6,2,"-");
	LCD_disp_str(10,1,"C");
	LCD_disp_str(10,2,"%");
    delay_n10us(20);      
    }

  }
//SHT11湿度测量范围：0～100%RH；温度测量范围：-40～+123.8℃；
/****************温度下限设置****************/
	if (flag==1)                                  
     {
      if(ADD==0)
	   {
	    delay(50);
	    t_xiaxian++;
	    if (t_xiaxian>(t_shangxian-1)) {t_xiaxian=(t_shangxian-1);}
		while(ADD==0);                                        //加上此句必须松按键才处理
	   }
	   if (DEC==0)
	   {
	    delay(50);		    
	    if(t_xiaxian<=-40)    {t_xiaxian=t_shangxian;}	      //超过实际范围，重新赋值温度下限
	    else 				   t_xiaxian--;
	    while(DEC==0);                                        //加上此句必须松按键才处理
	   }
       //温度下限显示
		LCD_disp_char(3,1,t_xiaxian/100+'0');                 //例如123/100=1 
		LCD_disp_char(4,1,abs(t_xiaxian%100/10)+'0');         //例如123%100=23,23/10 =2 ,-12取绝对值12 	  
        LCD_disp_char(5,1,abs(t_xiaxian%10)+'0'); 			  //例如123%10 =3
        if(t_xiaxian<0)    {LCD_disp_str(12,2,"nega");}	      //温度下限为负数，显示nega
	 	else 			   {LCD_disp_str(12,2,"    ");}		  //去除负数提醒nega
	 }

/****************温度上限设置****************/
	if (flag==2)              
	  {
	   if(ADD==0)
	   {
	    delay(150);
	    t_shangxian++;
	    if (t_shangxian>123) {t_shangxian=123;}              //上限值达到最大值 123时，上限值保持为123
	    while(ADD==0);                                       //加上此句必须松按键才处理
	   }
	   if (DEC==0)
	   {
	    delay(150);
	    t_shangxian--;
	    if (t_shangxian<(t_xiaxian+1)) {t_shangxian=(t_xiaxian+1);}//确保设定的上限值不比下限低
	    while(DEC==0);                                       //加上此句必须松按键才处理
	   }
       //温度上限显示
		LCD_disp_char(7,1,t_shangxian/100+'0');
	    LCD_disp_char(8,1,(t_shangxian%100)/10+'0');          
	    LCD_disp_char(9,1,(t_shangxian%10)+'0');       
	  }

/****************湿度下限设置****************/
	if (flag==3)             
	  {
	   if(ADD==0)
	   {
	    delay(150);
	    rh_xiaxian++;
	    if (rh_xiaxian>(rh_shangxian-1)) {rh_xiaxian=(rh_shangxian-1);}
	    while(ADD==0);                                      //加上此句必须松按键才处理
	   }
	   if (DEC==0)
	   {
	    delay(150);    
	    if (rh_xiaxian<=0) {rh_xiaxian=0;}					//下限值达到最小值0时，下限值保持为0
	    else rh_xiaxian--;
	    while(DEC==0);                                      //加上此句必须松按键才处理
	   }
       //湿度下限显示
		LCD_disp_char(3,2,rh_xiaxian/100+'0');
	    LCD_disp_char(4,2,(rh_xiaxian%100)/10+'0');               
	    LCD_disp_char(5,2,(rh_xiaxian%10)+'0');
	  }


/****************湿度上限设置****************/
    if (flag==4)              
	  {
	   if(ADD==0)
	   {
	    delay(150);
	    rh_shangxian++;
	    if (rh_shangxian>99) {rh_shangxian=99;}            //上限值达到最大值 99时，上限值保持为 99
	    while(ADD==0);                                     //加上此句必须松按键才处理
	   }
	   if (DEC==0)
	   {
	    delay(150);
	    rh_shangxian--;
	    if (rh_shangxian<(rh_xiaxian+1)) {rh_shangxian=(rh_xiaxian+1);}//确保设定的上限值不比下限低
	    while(DEC==0);                                     //加上此句必须松按键才处理
	   }
       //湿度上限显示
		LCD_disp_char(7,2,rh_shangxian/100+'0');         
	    LCD_disp_char(8,2,(rh_shangxian%100)/10+'0');          
	    LCD_disp_char(9,2,(rh_shangxian%10)+'0');     
	  }

/****************退出键设置****************/ 					     
if(ESC==0)                     
 {					   
  delay(10);
  if(ESC==0)
    {           
	 while(ESC==0);
	 flag=0;
     LCD_init(); 
     s_connectionreset();     
     }

  }
}  


/*-------------------------------------- 
;模块名称:delay(); 
;功    能:延迟函数
;-------------------------------------*/
void delay(uint j)                        //1ms，j取20
{
        uchar i=250;
        for(;j>0;j--)
        {
                while(--i);
                i=249;
                while(--i);
                i=250;
        }
}

//*********************第三部分报警设置   END****************************************

