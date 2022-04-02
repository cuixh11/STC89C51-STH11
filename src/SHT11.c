/*
*******************************************************************************
*文 件 名：SHT11.c
*描    述：第二部分SHT11设置
*  版本号：v1.0.0  
;微处理器: STC89C51
;编译环境: Keil uVision V4.10 
;作    者: Cui Xinghai 
;创建日期: 2022.2.24   
; 备   注：
*******************************************************************************
*/
#define  _STH11_C
#include "STH11.h"
#include <intrins.h> //应用于程序设计，一般出C51单片机编程中，一般程序中需要使用到空指令_nop_();字符循环移位指令_crol_等时使用。

   //*********************第二部分SHT11设置   START**************************************** 

/*-------------------------------------- 
;模块名称:s_transstart(); 
;功    能:启动传输函数
;-------------------------------------*/  
void s_transstart(void) 
// generates a transmission start (SHT11发送命令，启动传输时序）  
//       _____         ________ 
// DATA:      |_______| 
//           ___     ___ 
// SCK : ___|   |___|   |______ 
{   
   DATA=1; SCK=0;                   //Initial state 
   _nop_(); 
   SCK=1; 
   _nop_(); 
   DATA=0; 
   _nop_(); 
   SCK=0;   
   _nop_();_nop_();_nop_(); 
   SCK=1; 
   _nop_(); 
   DATA=1;        
   _nop_(); 
   SCK=0;        
} 

/*-------------------------------------- 
;模块名称:s_connectionreset(); 
;功    能:连接复位函数
;-------------------------------------*/ 
void s_connectionreset(void) 
// 通讯复位时序：如果与 SHT11 通讯中断，下列信号时序可以复位串口：DATA 保持高电平时，触发 SCK 时钟 9 次或更多。 
//       _____________________________________________________         ________ 
// DATA:                                                      |_______| 
//          _    _    _    _    _    _    _    _    _        ___     ___ 
// SCK : __| |__| |__| |__| |__| |__| |__| |__| |__| |______|   |___|   |______ 
{   
  unsigned char i;  
  DATA=1; SCK=0;                    //Initial state 
  for(i=0;i<9;i++)                  //9 SCK cycles,DATA保持高，SCK时钟触发9次，发送启动传输，通迅即复位 
  { 
    SCK=1;
    SCK=0; 
  } 
  s_transstart();                   //transmission start 
} 


/*-------------------------------------- 
;模块名称:s_write_byte(); 
;功    能:SHT11写函数						
;-------------------------------------*/ 
char s_write_byte(unsigned char value) 
// 写入一个字节，并检查确认 
{  
  unsigned char i,error=0;
     
  for (i=0x80;i>0;i/=2)             //0x1000 0000用于掩码的移位,高位为1，循环右移 https://zhidao.baidu.com/question/2144112204880805988.html 
  {  
    if (i & value) DATA=1;          //和要发送的数相与，结果为发送的位
              else DATA=0;                         
    SCK=1;                           
    _nop_();_nop_();_nop_();             
    SCK=0; 
  } 

  DATA=1;                           //释放数据线
  SCK=1;                            //clk #9 for ack  
  error=DATA;                       //检查应答信号，确认通讯正常  
  _nop_();_nop_();_nop_();
  SCK=0;
  DATA=1;                           // 
  return error;                     //error=1 in case of no acknowledge //返回：0成功，1失败
} 			
 

/*-------------------------------------- 
;模块名称:s_read_byte(); 
;功    能:SHT11读函数
;-------------------------------------*/ 
char s_read_byte(unsigned char ack)   
{  
  unsigned char i,val=0; 
  DATA=1;                           //释放数据线 
  for (i=0x80;i>0;i/=2)              
  { 
    SCK=1;                          //clk for SENSI-BUS 
    if (DATA) val=(val | i);        //读一位数据线的值    
	_nop_();_nop_();_nop_();        //pulswith approx. 3 us
    SCK=0;              
  } 
  if(ack==1)DATA=0;                 //in case of "ack==1" pull down DATA-Line 
  else DATA=1;                      //如果是校验(ack==0)，读取完后结束通讯
  _nop_();_nop_();_nop_();          //pulswith approx. 3 us 
  SCK=1;                            //clk #9 for ack 
  _nop_();_nop_();_nop_();          //pulswith approx. 3 us  
  SCK=0;                 
  _nop_();_nop_();_nop_();          //pulswith approx. 3 us 
  DATA=1;                           //release DATA-line 
  return val; 
} 
 

 

/*-------------------------------------- 
;模块名称:s_measure(); 
;功    能:测量温湿度函数
;-------------------------------------*/ 
char s_measure(unsigned char *p_value, unsigned char *p_checksum, unsigned char mode) 
// 进行温度或者湿度转换，由参数mode决定转换内容 
{  
  unsigned error=0; 
  unsigned int i; 
 
  s_transstart();                   //transmission start 

  switch(mode){                     //选择发送命令 
    case TEMP  : error+=s_write_byte(MEASURE_TEMP); break; 
    case HUMI  : error+=s_write_byte(MEASURE_HUMI); break; 
    default     : break;    
  } 
  for (i=0;i<65535;i++) if(DATA==0) break; //等待测量结束
  if(DATA) error+=1;                // 如果长时间数据线没有拉低，说明测量错误
  *(p_value)  =s_read_byte(ACK);    //读第一个字节，高字节 (MSB) 
  *(p_value+1)=s_read_byte(ACK);    //读第二个字节，低字节 (LSB)
  *p_checksum =s_read_byte(noACK);  //read checksum 
   return error; 
} 
 

/*-------------------------------------- 
;模块名称:calc_SHT11(); 
;功    能:温湿度补偿函数
;备    注：SHT11湿度测量范围：0～100%RH；温度测量范围：-40～+123.8℃
;-------------------------------------*/ 
void calc_SHT11(float *p_humidity ,float *p_temperature)
// calculates temperature [C] and humidity [%RH] 
// input :  humi [Ticks] (12 bit) 
//          temp [Ticks] (14 bit)
// output:  humi [%RH]
//          temp [C]
                                    //const表示常量，不允许修改里面的内容
{ const float C1=-4.0;              // 12位湿度精度 修正公式
  const float C2=+0.0405;           // 12位湿度精度 修正公式
  const float C3=-0.0000028;        // 12位湿度精度 修正公式
  const float T1=+0.01;             // 14位温度精度 5V条件  修正公式
  const float T2=+0.00008;          // 14位温度精度 5V条件  修正公式

  float rh=*p_humidity;             // rh:      Humidity [Ticks] 12 Bit 
  float t=*p_temperature;           // t:       Temperature [Ticks] 14 Bit
  float rh_lin;                     // rh_lin:  Humidity linear
  float rh_true;                    // rh_true: Temperature compensated humidity
  float t_C;                        // t_C   :  Temperature [C]

  t_C=t*0.01 - 40;                  //补偿温度，14位温度精度 5V条件  修正公式
  
  rh_lin=C3*rh*rh + C2*rh + C1;     //相对湿度非线性补偿
  rh_true=(t_C-25)*(T1+T2*rh)+rh_lin-3;   //相对湿度对于温度依赖性补偿

  if(rh_true>100)rh_true=100;       //湿度最大修正
  if(rh_true<0.1)rh_true=0.1;       //湿度最小修正

  *p_temperature=t_C;               //返回温度结果
  *p_humidity=rh_true;              //返回湿度结果
}
//*********************第二部分STH11设置   END****************************************

