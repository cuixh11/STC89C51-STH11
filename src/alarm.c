/*
*******************************************************************************
*�� �� ����alarm.c
*��    �����������ֱ�������
*  �汾�ţ�v1.0.0  
;΢������: STC89C51
;���뻷��: Keil uVision V4.10 
;��    ��: Cui Xinghai 
;��������: 2022.2.24   
; ��   ע��
*******************************************************************************
*/
#define  _alarm_C
#include "alarm.h"
#include "LCD1602.h"
#include "STH11.h"

#include <reg52.h>
#include <math.h>    //Keil library   

//*********************�������ֱ�������   START****************************************
sbit  t_green   =  P1^0;
sbit  t_red     =  P1^1;
sbit  rh_green  =  P1^2;
sbit  rh_red    =  P1^3;

int t_shangxian = 20;            //���ޱ����¶ȣ�Ĭ��ֵΪ 20C
int t_xiaxian   = 10;            //���ޱ����¶ȣ�Ĭ��ֵΪ 10C
int rh_shangxian= 50;            //���ޱ���ʪ�ȣ�Ĭ��ֵΪ50%
int rh_xiaxian  = 40;            //���ޱ���ʪ�ȣ�Ĭ��ֵΪ40%

unsigned char flag=0;

/*-------------------------------------- 
;ģ������:led_control(); 
;��    ��:LED״̬����
;-------------------------------------*/ 
void led_control( float *WENDU,  float *SHIDU)                   
{
//�¶ȹ��ͱ���ָʾ
  if(*WENDU <= t_xiaxian)    {t_green = 0;}
  else {t_green=1;}
//�¶ȹ��߱���ָʾ
  if(*WENDU >= t_shangxian)  {t_red   = 0;}
  else {t_red=1;}
//ʪ�ȹ��ͱ���ָʾ
  if(*SHIDU <= rh_xiaxian)   {rh_green= 0;}
  else {rh_green=1;}
//ʪ�ȹ��߱���ָʾ
  if(*SHIDU >= rh_shangxian) {rh_red  = 0;}
  else {rh_red=1;}
//��������ʾ
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
;ģ������:key(); 
;��    ��:��������
;-------------------------------------*/ 
void key()   
{
/****************���ü�����****************/
 if(SET == 0)
  {
    delay(50);               //��������
    if(SET == 0)
     {
      flag++;
      if (flag==5)flag = 0;  //û�з�������
	  while(SET==0);         //��һֱ���£�ѭ��

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
//SHT11ʪ�Ȳ�����Χ��0��100%RH���¶Ȳ�����Χ��-40��+123.8�棻
/****************�¶���������****************/
	if (flag==1)                                  
     {
      if(ADD==0)
	   {
	    delay(50);
	    t_xiaxian++;
	    if (t_xiaxian>(t_shangxian-1)) {t_xiaxian=(t_shangxian-1);}
		while(ADD==0);                                        //���ϴ˾�����ɰ����Ŵ���
	   }
	   if (DEC==0)
	   {
	    delay(50);		    
	    if(t_xiaxian<=-40)    {t_xiaxian=t_shangxian;}	      //����ʵ�ʷ�Χ�����¸�ֵ�¶�����
	    else 				   t_xiaxian--;
	    while(DEC==0);                                        //���ϴ˾�����ɰ����Ŵ���
	   }
       //�¶�������ʾ
		LCD_disp_char(3,1,t_xiaxian/100+'0');                 //����123/100=1 
		LCD_disp_char(4,1,abs(t_xiaxian%100/10)+'0');         //����123%100=23,23/10 =2 ,-12ȡ����ֵ12 	  
        LCD_disp_char(5,1,abs(t_xiaxian%10)+'0'); 			  //����123%10 =3
        if(t_xiaxian<0)    {LCD_disp_str(12,2,"nega");}	      //�¶�����Ϊ��������ʾnega
	 	else 			   {LCD_disp_str(12,2,"    ");}		  //ȥ����������nega
	 }

/****************�¶���������****************/
	if (flag==2)              
	  {
	   if(ADD==0)
	   {
	    delay(150);
	    t_shangxian++;
	    if (t_shangxian>123) {t_shangxian=123;}              //����ֵ�ﵽ���ֵ 123ʱ������ֵ����Ϊ123
	    while(ADD==0);                                       //���ϴ˾�����ɰ����Ŵ���
	   }
	   if (DEC==0)
	   {
	    delay(150);
	    t_shangxian--;
	    if (t_shangxian<(t_xiaxian+1)) {t_shangxian=(t_xiaxian+1);}//ȷ���趨������ֵ�������޵�
	    while(DEC==0);                                       //���ϴ˾�����ɰ����Ŵ���
	   }
       //�¶�������ʾ
		LCD_disp_char(7,1,t_shangxian/100+'0');
	    LCD_disp_char(8,1,(t_shangxian%100)/10+'0');          
	    LCD_disp_char(9,1,(t_shangxian%10)+'0');       
	  }

/****************ʪ����������****************/
	if (flag==3)             
	  {
	   if(ADD==0)
	   {
	    delay(150);
	    rh_xiaxian++;
	    if (rh_xiaxian>(rh_shangxian-1)) {rh_xiaxian=(rh_shangxian-1);}
	    while(ADD==0);                                      //���ϴ˾�����ɰ����Ŵ���
	   }
	   if (DEC==0)
	   {
	    delay(150);    
	    if (rh_xiaxian<=0) {rh_xiaxian=0;}					//����ֵ�ﵽ��Сֵ0ʱ������ֵ����Ϊ0
	    else rh_xiaxian--;
	    while(DEC==0);                                      //���ϴ˾�����ɰ����Ŵ���
	   }
       //ʪ��������ʾ
		LCD_disp_char(3,2,rh_xiaxian/100+'0');
	    LCD_disp_char(4,2,(rh_xiaxian%100)/10+'0');               
	    LCD_disp_char(5,2,(rh_xiaxian%10)+'0');
	  }


/****************ʪ����������****************/
    if (flag==4)              
	  {
	   if(ADD==0)
	   {
	    delay(150);
	    rh_shangxian++;
	    if (rh_shangxian>99) {rh_shangxian=99;}            //����ֵ�ﵽ���ֵ 99ʱ������ֵ����Ϊ 99
	    while(ADD==0);                                     //���ϴ˾�����ɰ����Ŵ���
	   }
	   if (DEC==0)
	   {
	    delay(150);
	    rh_shangxian--;
	    if (rh_shangxian<(rh_xiaxian+1)) {rh_shangxian=(rh_xiaxian+1);}//ȷ���趨������ֵ�������޵�
	    while(DEC==0);                                     //���ϴ˾�����ɰ����Ŵ���
	   }
       //ʪ��������ʾ
		LCD_disp_char(7,2,rh_shangxian/100+'0');         
	    LCD_disp_char(8,2,(rh_shangxian%100)/10+'0');          
	    LCD_disp_char(9,2,(rh_shangxian%10)+'0');     
	  }

/****************�˳�������****************/ 					     
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
;ģ������:delay(); 
;��    ��:�ӳٺ���
;-------------------------------------*/
void delay(uint j)                        //1ms��jȡ20
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

//*********************�������ֱ�������   END****************************************

