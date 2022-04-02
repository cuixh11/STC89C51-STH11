/*********************************************************************************** 
;����˵��:SHT11��STC89C51��ʪ�Ȳɼ��Ͳ���ϵͳ��Proteus����
;�ļ�����:DU.c     
;�汾�ţ�v1.0.0  
;΢������:STC89C51
;���뻷��:Keil uVision V4.10 
;��    ��:Cui Xinghai 
;��������:2022.2.24   
;***********************************************************************************/

/*************����ӿ�********************
         P0------DB0��DB7  (LCD1602)     
         P2.0------RS      (LCD1602)
         P2.1------RW      (LCD1602)
         P2.2------E       (LCD1602)

		 P2.6------SCK     (STH11)
		 P2.7------DATA    (STH11)

		 P1.5------SET     //���������
		 P1.6------ADD	   //�������Ӽ�
		 P1.7------DEC     //������ټ�
         P3.7------ESC     //�����˳���
		 P1.4------BEEP    //���������

		 P1.0------t_green		     //�¶ȹ��͵�
		 P1.1------t_red		     //�¶ȹ��ߵ�
		 P1.2------rh_green		     //ʪ�ȹ��͵�
		 P1.3------rh_red		     //ʪ�ȹ��ߵ�
*****************************************/

#include <reg52.h>	 
#include <math.h>    //Keil library   
#include <stdio.h>   //Keil library 

#include "LCD1602.h"
#include "STH11.h"
#include "alarm.h"
//#include "main.h"	 //�ò�����main��һ������

extern unsigned char flag;


//*********������*****************
void main(void)
{
       value humi_val,temp_val;				     //����������ͬ�壬һ������ʪ�ȣ�һ�������¶�
       unsigned char error,checksum; 			 //���ڼ����Ƿ���ִ���
       unsigned int wendu,shidu;				 //����,��һλС������ʪ�ȵ�ֵ
	   
	   LCD_init();  
       s_connectionreset();
 
//*********��ʼ���¶���ʾ��*********
       LCD_disp_str(0,1,"TE:");
       LCD_disp_str(3,1,"TTT.TC");

//*********��ʼ��ʪ����ʾ��*********
       LCD_disp_str(0,2,"RH:");
	   LCD_disp_str(3,2,"RRR.R%");
       delay_n10us(2000);     //��ʱ0.2s

  while(1) 
        {
		  LCD_disp_char(13,1,flag+'0');

          key();
     
         if(flag==0)
          {
           error=0; 										 //��ʼ��error=0����û�д���
           error+=s_measure((unsigned char*) &humi_val.i,&checksum,HUMI);  //measure humidity 
           error+=s_measure((unsigned char*) &temp_val.i,&checksum,TEMP);  //measure temperature 
           if(error!=0) s_connectionreset();                 //in case of an error: connection reset 
           else 
            { 
             humi_val.f=(float)humi_val.i;                   //converts integer to float
             temp_val.f=(float)temp_val.i;                   //converts integer to float
             calc_SHT11(&humi_val.f,&temp_val.f);            //calculate humidity, temperature
			       
             LCD_disp_str(0,1,"TE:");
             LCD_disp_str(0,2,"RH:");
             LCD_disp_str(6,1,".");
             LCD_disp_str(6,2,".");
             LCD_disp_str(8,1,"C   ");
             LCD_disp_str(8,2,"%   ");

		     wendu=10*temp_val.f;                            //�����¶�109.1��1091
			 
			 if(10*temp_val.f<0) {LCD_disp_str(12,2,"nega");}//ʵʱ�¶�Ϊ����ʱ����ʾ��ʾ��nega	
		     else {LCD_disp_str(12,2,"    ");}		         //ʵʱ�¶�Ϊ����ʱ��ȥ����������nega
			 
			 LCD_disp_char(3,1,abs(wendu)/1000+'0');         //��ʾ�¶Ȱ�λ,�ӡ�0����Ϊ�˽��ַ���ASCII�����48�����ַ�0��ASCIIֵ����һ���ǽ�����0,1,2������9ת��Ϊ�ַ���0������1����������9����			 
			 LCD_disp_char(4,1,abs(wendu)%1000/100+'0');     //��ʾ�¶�ʮλ
             LCD_disp_char(5,1,abs(wendu)%100/10+'0');       //��ʾ�¶ȸ�λ
		     LCD_disp_char(7,1,abs(wendu)%10+'0');           //��ʾ�¶�С������һλ

			 shidu=10*humi_val.f;
			 LCD_disp_char(3,2,shidu/1000+'0');               //��ʾʪ�Ȱ�λ
             LCD_disp_char(4,2,(shidu%1000)/100+'0');         //��ʾʪ��ʮλ
             LCD_disp_char(5,2,(shidu%100)/10+'0');           //��ʾʪ�ȸ�λ
			 LCD_disp_char(7,2,(shidu%10)+'0');               //��ʾʪ��С������һλ
          
             led_control(&temp_val.f,&humi_val.f);

           } 
          //----------wait approx. 0.8s to avoid heating up SHT11------------------------------       
		  delay_n10us(800);                                //��ʱԼ0.8s       
		  }

      
 
      }
}


