/*********************************************************************************** 
;功能说明:SHT11和STC89C51温湿度采集和测量系统的Proteus仿真
;文件名称:DU.c     
;版本号：v1.0.0  
;微处理器:STC89C51
;编译环境:Keil uVision V4.10 
;作    者:Cui Xinghai 
;创建日期:2022.2.24   
;***********************************************************************************/

/*************定义接口********************
         P0------DB0～DB7  (LCD1602)     
         P2.0------RS      (LCD1602)
         P2.1------RW      (LCD1602)
         P2.2------E       (LCD1602)

		 P2.6------SCK     (STH11)
		 P2.7------DATA    (STH11)

		 P1.5------SET     //定义调整键
		 P1.6------ADD	   //定义增加键
		 P1.7------DEC     //定义减少键
         P3.7------ESC     //定义退出键
		 P1.4------BEEP    //定义蜂鸣器

		 P1.0------t_green		     //温度过低灯
		 P1.1------t_red		     //温度过高灯
		 P1.2------rh_green		     //湿度过低灯
		 P1.3------rh_red		     //湿度过高灯
*****************************************/

#include <reg52.h>	 
#include <math.h>    //Keil library   
#include <stdio.h>   //Keil library 

#include "LCD1602.h"
#include "STH11.h"
#include "alarm.h"
//#include "main.h"	 //用不到，main就一个函数

extern unsigned char flag;


//*********主函数*****************
void main(void)
{
       value humi_val,temp_val;				     //定义两个共同体，一个用于湿度，一个用于温度
       unsigned char error,checksum; 			 //用于检验是否出现错误
       unsigned int wendu,shidu;				 //最终,带一位小数的温湿度的值
	   
	   LCD_init();  
       s_connectionreset();
 
//*********初始化温度显示区*********
       LCD_disp_str(0,1,"TE:");
       LCD_disp_str(3,1,"TTT.TC");

//*********初始化湿度显示区*********
       LCD_disp_str(0,2,"RH:");
	   LCD_disp_str(3,2,"RRR.R%");
       delay_n10us(2000);     //延时0.2s

  while(1) 
        {
		  LCD_disp_char(13,1,flag+'0');

          key();
     
         if(flag==0)
          {
           error=0; 										 //初始化error=0，即没有错误
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

		     wendu=10*temp_val.f;                            //例如温度109.1→1091
			 
			 if(10*temp_val.f<0) {LCD_disp_str(12,2,"nega");}//实时温度为负数时，显示提示符nega	
		     else {LCD_disp_str(12,2,"    ");}		         //实时温度为正数时，去除负数提醒nega
			 
			 LCD_disp_char(3,1,abs(wendu)/1000+'0');         //显示温度百位,加“0”是为了将字符的ASCII码大于48（即字符0的ASCII值），一般是将数字0,1,2……，9转换为字符“0”，“1”……，“9”；			 
			 LCD_disp_char(4,1,abs(wendu)%1000/100+'0');     //显示温度十位
             LCD_disp_char(5,1,abs(wendu)%100/10+'0');       //显示温度个位
		     LCD_disp_char(7,1,abs(wendu)%10+'0');           //显示温度小数点后第一位

			 shidu=10*humi_val.f;
			 LCD_disp_char(3,2,shidu/1000+'0');               //显示湿度百位
             LCD_disp_char(4,2,(shidu%1000)/100+'0');         //显示湿度十位
             LCD_disp_char(5,2,(shidu%100)/10+'0');           //显示湿度个位
			 LCD_disp_char(7,2,(shidu%10)+'0');               //显示湿度小数点后第一位
          
             led_control(&temp_val.f,&humi_val.f);

           } 
          //----------wait approx. 0.8s to avoid heating up SHT11------------------------------       
		  delay_n10us(800);                                //延时约0.8s       
		  }

      
 
      }
}


