/*
*******************************************************************************
*�� �� ����SHT11.c
*��    �����ڶ�����SHT11����
*  �汾�ţ�v1.0.0  
;΢������: STC89C51
;���뻷��: Keil uVision V4.10 
;��    ��: Cui Xinghai 
;��������: 2022.2.24   
; ��   ע��
*******************************************************************************
*/
#define  _STH11_C
#include "STH11.h"
#include <intrins.h> //Ӧ���ڳ�����ƣ�һ���C51��Ƭ������У�һ���������Ҫʹ�õ���ָ��_nop_();�ַ�ѭ����λָ��_crol_��ʱʹ�á�

   //*********************�ڶ�����SHT11����   START**************************************** 

/*-------------------------------------- 
;ģ������:s_transstart(); 
;��    ��:�������亯��
;-------------------------------------*/  
void s_transstart(void) 
// generates a transmission start (SHT11���������������ʱ��  
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
;ģ������:s_connectionreset(); 
;��    ��:���Ӹ�λ����
;-------------------------------------*/ 
void s_connectionreset(void) 
// ͨѶ��λʱ������� SHT11 ͨѶ�жϣ������ź�ʱ����Ը�λ���ڣ�DATA ���ָߵ�ƽʱ������ SCK ʱ�� 9 �λ���ࡣ 
//       _____________________________________________________         ________ 
// DATA:                                                      |_______| 
//          _    _    _    _    _    _    _    _    _        ___     ___ 
// SCK : __| |__| |__| |__| |__| |__| |__| |__| |__| |______|   |___|   |______ 
{   
  unsigned char i;  
  DATA=1; SCK=0;                    //Initial state 
  for(i=0;i<9;i++)                  //9 SCK cycles,DATA���ָߣ�SCKʱ�Ӵ���9�Σ������������䣬ͨѸ����λ 
  { 
    SCK=1;
    SCK=0; 
  } 
  s_transstart();                   //transmission start 
} 


/*-------------------------------------- 
;ģ������:s_write_byte(); 
;��    ��:SHT11д����						
;-------------------------------------*/ 
char s_write_byte(unsigned char value) 
// д��һ���ֽڣ������ȷ�� 
{  
  unsigned char i,error=0;
     
  for (i=0x80;i>0;i/=2)             //0x1000 0000�����������λ,��λΪ1��ѭ������ https://zhidao.baidu.com/question/2144112204880805988.html 
  {  
    if (i & value) DATA=1;          //��Ҫ���͵������룬���Ϊ���͵�λ
              else DATA=0;                         
    SCK=1;                           
    _nop_();_nop_();_nop_();             
    SCK=0; 
  } 

  DATA=1;                           //�ͷ�������
  SCK=1;                            //clk #9 for ack  
  error=DATA;                       //���Ӧ���źţ�ȷ��ͨѶ����  
  _nop_();_nop_();_nop_();
  SCK=0;
  DATA=1;                           // 
  return error;                     //error=1 in case of no acknowledge //���أ�0�ɹ���1ʧ��
} 			
 

/*-------------------------------------- 
;ģ������:s_read_byte(); 
;��    ��:SHT11������
;-------------------------------------*/ 
char s_read_byte(unsigned char ack)   
{  
  unsigned char i,val=0; 
  DATA=1;                           //�ͷ������� 
  for (i=0x80;i>0;i/=2)              
  { 
    SCK=1;                          //clk for SENSI-BUS 
    if (DATA) val=(val | i);        //��һλ�����ߵ�ֵ    
	_nop_();_nop_();_nop_();        //pulswith approx. 3 us
    SCK=0;              
  } 
  if(ack==1)DATA=0;                 //in case of "ack==1" pull down DATA-Line 
  else DATA=1;                      //�����У��(ack==0)����ȡ������ͨѶ
  _nop_();_nop_();_nop_();          //pulswith approx. 3 us 
  SCK=1;                            //clk #9 for ack 
  _nop_();_nop_();_nop_();          //pulswith approx. 3 us  
  SCK=0;                 
  _nop_();_nop_();_nop_();          //pulswith approx. 3 us 
  DATA=1;                           //release DATA-line 
  return val; 
} 
 

 

/*-------------------------------------- 
;ģ������:s_measure(); 
;��    ��:������ʪ�Ⱥ���
;-------------------------------------*/ 
char s_measure(unsigned char *p_value, unsigned char *p_checksum, unsigned char mode) 
// �����¶Ȼ���ʪ��ת�����ɲ���mode����ת������ 
{  
  unsigned error=0; 
  unsigned int i; 
 
  s_transstart();                   //transmission start 

  switch(mode){                     //ѡ�������� 
    case TEMP  : error+=s_write_byte(MEASURE_TEMP); break; 
    case HUMI  : error+=s_write_byte(MEASURE_HUMI); break; 
    default     : break;    
  } 
  for (i=0;i<65535;i++) if(DATA==0) break; //�ȴ���������
  if(DATA) error+=1;                // �����ʱ��������û�����ͣ�˵����������
  *(p_value)  =s_read_byte(ACK);    //����һ���ֽڣ����ֽ� (MSB) 
  *(p_value+1)=s_read_byte(ACK);    //���ڶ����ֽڣ����ֽ� (LSB)
  *p_checksum =s_read_byte(noACK);  //read checksum 
   return error; 
} 
 

/*-------------------------------------- 
;ģ������:calc_SHT11(); 
;��    ��:��ʪ�Ȳ�������
;��    ע��SHT11ʪ�Ȳ�����Χ��0��100%RH���¶Ȳ�����Χ��-40��+123.8��
;-------------------------------------*/ 
void calc_SHT11(float *p_humidity ,float *p_temperature)
// calculates temperature [C] and humidity [%RH] 
// input :  humi [Ticks] (12 bit) 
//          temp [Ticks] (14 bit)
// output:  humi [%RH]
//          temp [C]
                                    //const��ʾ�������������޸����������
{ const float C1=-4.0;              // 12λʪ�Ⱦ��� ������ʽ
  const float C2=+0.0405;           // 12λʪ�Ⱦ��� ������ʽ
  const float C3=-0.0000028;        // 12λʪ�Ⱦ��� ������ʽ
  const float T1=+0.01;             // 14λ�¶Ⱦ��� 5V����  ������ʽ
  const float T2=+0.00008;          // 14λ�¶Ⱦ��� 5V����  ������ʽ

  float rh=*p_humidity;             // rh:      Humidity [Ticks] 12 Bit 
  float t=*p_temperature;           // t:       Temperature [Ticks] 14 Bit
  float rh_lin;                     // rh_lin:  Humidity linear
  float rh_true;                    // rh_true: Temperature compensated humidity
  float t_C;                        // t_C   :  Temperature [C]

  t_C=t*0.01 - 40;                  //�����¶ȣ�14λ�¶Ⱦ��� 5V����  ������ʽ
  
  rh_lin=C3*rh*rh + C2*rh + C1;     //���ʪ�ȷ����Բ���
  rh_true=(t_C-25)*(T1+T2*rh)+rh_lin-3;   //���ʪ�ȶ����¶������Բ���

  if(rh_true>100)rh_true=100;       //ʪ���������
  if(rh_true<0.1)rh_true=0.1;       //ʪ����С����

  *p_temperature=t_C;               //�����¶Ƚ��
  *p_humidity=rh_true;              //����ʪ�Ƚ��
}
//*********************�ڶ�����STH11����   END****************************************

