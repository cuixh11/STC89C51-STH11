/*********************************************************************************** 
;����˵��:DHT90��LCD1602����ʪ����ʾ 
;�ļ�����:DHT90.c    
 
;΢������:AT89S52
;���뻷��:Keil uVision V2.38a
 
;��    ��:Huang Yanxiang
;��    Ȩ:(c)Huang Yanxiang
;��������:2008.08.15    
;��    ��:V1.2  
;�޸�����:2008.08.26
;�޸�˵��:�����һ��LCD_disp_str()�������޸�����ʱ������ע�͡�����лouravr������wyb4993�����ѣ�
;***********************************************************************************/

/*************����ӿ�********************
         P0------DB0��DB7  (LCD1602)     
         P2.0------RS      (LCD1602)
         P2.1------RW      (LCD1602)
         P2.2------E       (LCD1602)
		 P2.6------SCK     (DHT90)
		 P2.7------DATA    (DHT90)
*****************************************/

#include <reg52.h>
#include <intrins.h> 
#include <math.h>    //Keil library   
#include <stdio.h>   //Keil library 

//*********************��һ����LCD1602���� START****************************************
#define LCD_DB        P0
        sbit         LCD_RS=P2^0;   //P2^0��p2.0����˼��LCD_RS��P2.0��Ч��������LCD_RS ��д�����Ƕ�P2.0��д �ô�����LCD_RS����ֱ�����ˣ�д������˾ͻ�֪���б�Ҫde
        sbit         LCD_RW=P2^1;   //P2^1��p2.1����˼
        sbit         LCD_E=P2^2;    //P2^2��p2.2����˼

/******���庯��****************/
#define uchar unsigned char
#define uint unsigned int
void LCD_init(void);                          //��ʼ������
void LCD_write_command(uchar command);        //дָ���
void LCD_write_data(uchar dat);               //д���ݺ���
void LCD_disp_char(uchar x,uchar y,uchar dat);//��ĳ����Ļλ������ʾһ���ַ�,X��0-15),y(1-2)
void LCD_disp_str(uchar x,uchar y,uchar *str); //LCD1602��ʾ�ַ�������
void delay_n10us(uint n);                     //��ʱ����


/*-------------------------------------- 
;ģ������:LCD_init(); 
;��    ��:��ʼ��LCD1602 
;ռ����Դ:--
;����˵��:--
;��������:2008.08.15 
;��    ��:FV1.0(�����汾Function Version)
;�޸�����:--
;�޸�˵��:--
;-------------------------------------*/ 
void LCD_init(void)
{
delay_n10us(10);
LCD_write_command(0x38);//����8λ��ʽ��2�У�5x7
delay_n10us(10);
LCD_write_command(0x0c);//������ʾ���ع�꣬����˸
delay_n10us(10);
LCD_write_command(0x06);//�趨���뷽ʽ����������λ
delay_n10us(10);
LCD_write_command(0x01);//�����Ļ��ʾ
delay_n10us(100);       //��ʱ��������ʱ��������ʱԼn��10us
}


/*-------------------------------------- 
;ģ������:LCD_write_command(); 
;��    ��:LCD1602дָ��� 
;ռ����Դ: P2.0--RS(LCD_RS),P2.1--RW(LCD_RW),P2.2--E(LCD_E).
;����˵��:datΪд�������
;��������:2008.08.15 
;��    ��:FV1.0(�����汾Function Version)
;�޸�����:--
;�޸�˵��:--
;-------------------------------------*/ 
void LCD_write_command(uchar dat)
{
delay_n10us(10);
LCD_RS=0;         //ָ��
LCD_RW=0;         //д��
LCD_E=1;          //����
LCD_DB=dat;
delay_n10us(10);  //ʵ��֤�����ҵ�LCD1602�ϣ���forѭ��1�ξ��������ͨдָ�
LCD_E=0;
delay_n10us(10);  //ʵ��֤�����ҵ�LCD1602�ϣ���forѭ��1�ξ��������ͨдָ�
}


/*-------------------------------------- 
;ģ������:LCD_write_data(); 
;��    ��:LCD1602д���ݺ��� 
;ռ����Դ: P2.0--RS(LCD_RS),P2.1--RW(LCD_RW),P2.2--E(LCD_E).
;����˵��:datΪд���ݲ���
;��������:2008.08.15 
;��    ��:FV1.0(�����汾Function Version)
;�޸�����:--
;�޸�˵��:--
;-------------------------------------*/
void LCD_write_data(uchar dat)
{
delay_n10us(10);
LCD_RS=1;          //����
LCD_RW=0;          //д��
LCD_E=1;           //����
LCD_DB=dat;
delay_n10us(10);
LCD_E=0;
delay_n10us(10);
}


/*-------------------------------------- 
;ģ������:LCD_disp_char(); 
;��    ��:LCD1602��ʾһ���ַ���������ĳ����Ļλ������ʾһ���ַ�,X��0-15),y(1-2)��
;ռ����Դ:--
;����˵��:XΪ1602����ֵ(ȡֵ��Χ��0-15),yΪ1602����ֵ(ȡֵ��Χ��1-2)��datΪ��Ҫ��ʾ�ַ���Ӧ�ĵ�ַ������
;��������:2008.08.15 
;��    ��:FV1.0(�����汾Function Version)
;�޸�����:--
;�޸�˵��:--
;-------------------------------------*/
void LCD_disp_char(uchar x,uchar y,uchar dat)
{
  uchar address;
  if(y==1)
         address=0x80+x;
  else
         address=0xc0+x;
  LCD_write_command(address);
  LCD_write_data(dat);
}



/*-------------------------------------- 
;ģ������:LCD_disp_str(); 
;��    ��:LCD1602��ʾ�ַ�����������ĳ����Ļ��ʼλ��{X��0-15),y(1-2)}����ʾһ���ַ�����
;ռ����Դ:--
;����˵��:XΪ1602����ֵ(ȡֵ��Χ��0-15),yΪ1602����ֵ(ȡֵ��Χ��1-2)��strΪ��Ҫ��ʾ�ַ�����Ӧ��ָ�������
;��������:2008.08.16 
;��    ��:FV1.0(�����汾Function Version)
;�޸�����:--
;�޸�˵��:-- 
;-------------------------------------*/
void LCD_disp_str(uchar x,uchar y,uchar *str)
{
  uchar address;
  if(y==1)
         address=0x80+x;
  else
         address=0xc0+x;
  LCD_write_command(address);
  while(*str!='\0')
  { 
    LCD_write_data(*str);   
    str++;
  }
}


/*-------------------------------------- 
;ģ������:delay_n10us(); 
;��    ��:��ʱ��������ʱԼn��10us
;ռ����Դ:--
;����˵��:--
;��������:2008.08.15 
;��    ��:FV1.1(�����汾Function Version)
;�޸�����:2008.08.26
;�޸�˵��:�޸�Ϊ�Ͼ�ȷ����ʱ����,"_nop_()"��ʱ1us@12M����
;-------------------------------------*/
void delay_n10us(uint n)  //��ʱn��10us@12M����
{       
        uint i;           
        for(i=n;i>0;i--)    
        {
        _nop_();_nop_();_nop_();_nop_();_nop_();_nop_(); 
		}
}                                    

//*********************��һ����LCD1602���� END****************************************


//*********************�ڶ�����DHT90����   START**************************************** 
sbit SCK  = P2^6;      //����ͨѶʱ�Ӷ˿�
sbit DATA = P2^7;      //����ͨѶ���ݶ˿�

typedef union  
{ unsigned int i;      //����������������
  float f; 
} value; 

enum {TEMP,HUMI};      //TEMP=0,HUMI=1
 

#define noACK 0             //�����ж��Ƿ����ͨѶ
#define ACK   1             //�������ݴ���
                            //adr  command  r/w 
#define STATUS_REG_W 0x06   //000   0011    0 
#define STATUS_REG_R 0x07   //000   0011    1 
#define MEASURE_TEMP 0x03   //000   0001    1 
#define MEASURE_HUMI 0x05   //000   0010    1 
#define RESET        0x1e   //000   1111    0 

/****************���庯��****************/
void s_transstart(void);               //�������亯��
void s_connectionreset(void);          //���Ӹ�λ����
char s_write_byte(unsigned char value);//DHT90д����
char s_read_byte(unsigned char ack);   //DHT90������
char s_measure(unsigned char *p_value, unsigned char *p_checksum, unsigned char mode);//������ʪ�Ⱥ���
void calc_dht90(float *p_humidity ,float *p_temperature);//��ʪ�Ȳ���



/*-------------------------------------- 
;ģ������:s_transstart(); 
;��    ��:�������亯��
;ռ����Դ:--
;����˵��:--
;��������:2008.08.15 
;��    ��:FV1.0(�����汾Function Version)
;�޸�����:--
;�޸�˵��:--
;-------------------------------------*/  
void s_transstart(void) 
// generates a transmission start  
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
;ռ����Դ:--
;����˵��:--
;��������:2008.08.15 
;��    ��:FV1.0(�����汾Function Version)
;�޸�����:--
;�޸�˵��:--
;-------------------------------------*/ 
void s_connectionreset(void) 
// communication reset: DATA-line=1 and at least 9 SCK cycles followed by transstart 
//       _____________________________________________________         ________ 
// DATA:                                                      |_______| 
//          _    _    _    _    _    _    _    _    _        ___     ___ 
// SCK : __| |__| |__| |__| |__| |__| |__| |__| |__| |______|   |___|   |______ 
{   
  unsigned char i;  
  DATA=1; SCK=0;                    //Initial state 
  for(i=0;i<9;i++)                  //9 SCK cycles 
  { 
    SCK=1;
    SCK=0; 
  } 
  s_transstart();                   //transmission start 
} 


/*-------------------------------------- 
;ģ������:s_write_byte(); 
;��    ��:DHT90д����
;ռ����Դ:--
;����˵��:--
;��������:2008.08.15 
;��    ��:FV1.0(�����汾Function Version)
;�޸�����:--
;�޸�˵��:--
;-------------------------------------*/ 
char s_write_byte(unsigned char value) 
//---------------------------------------------------------------------------------- 
// writes a byte on the Sensibus and checks the acknowledge  
{  
  unsigned char i,error=0;   
  for (i=0x80;i>0;i/=2)             //shift bit for masking 
  {  
    if (i & value) DATA=1;          //masking value with i , write to SENSI-BUS 
    else DATA=0;                         
    SCK=1;                          //clk for SENSI-BUS 
    _nop_();_nop_();_nop_();        //pulswith approx. 3 us     
    SCK=0; 
  } 
  DATA=1;                           //release DATA-line 
  SCK=1;                            //clk #9 for ack  
  error=DATA;                       //check ack (DATA will be pulled down by DHT90),DATA�ڵ�9�������ؽ���DHT90�Զ�����Ϊ�͵�ƽ�� 
  _nop_();_nop_();_nop_();
  SCK=0;
  DATA=1;                           //release DATA-line 
  return error;                     //error=1 in case of no acknowledge //���أ�0�ɹ���1ʧ��
} 
 

/*-------------------------------------- 
;ģ������:s_read_byte(); 
;��    ��:DHT11������
;ռ����Դ:--
;����˵��:--
;��������:2008.08.15 
;��    ��:FV1.0(�����汾Function Version)
;�޸�����:--
;�޸�˵��:--
;-------------------------------------*/ 
char s_read_byte(unsigned char ack)  
// reads a byte form the Sensibus and gives an acknowledge in case of "ack=1"  
{  
  unsigned char i,val=0; 
  DATA=1;                           //release DATA-line 
  for (i=0x80;i>0;i/=2)             //shift bit for masking 
  { SCK=1;                          //clk for SENSI-BUS 
    if (DATA) val=(val | i);        //read bit   
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
;ռ����Դ:--
;����˵��:--
;��������:2008.08.15 
;��    ��:FV1.0(�����汾Function Version)
;�޸�����:--
;�޸�˵��:--
;-------------------------------------*/ 
char s_measure(unsigned char *p_value, unsigned char *p_checksum, unsigned char mode) 
// makes a measurement (humidity/temperature) with checksum 
{  
  unsigned error=0; 
  unsigned int i; 
 
  s_transstart();                   //transmission start 
  switch(mode){                     //send command to sensor 
    case TEMP  : error+=s_write_byte(MEASURE_TEMP); break; 
    case HUMI  : error+=s_write_byte(MEASURE_HUMI); break; 
    default     : break;    
  } 
  for (i=0;i<65535;i++) if(DATA==0) break; //wait until sensor has finished the measurement 
  if(DATA) error+=1;                // or timeout (~2 sec.) is reached 
  *(p_value)  =s_read_byte(ACK);    //read the first byte (MSB) 
  *(p_value+1)=s_read_byte(ACK);    //read the second byte (LSB) 
  *p_checksum =s_read_byte(noACK);  //read checksum 
  return error; 
} 
 

/*-------------------------------------- 
;ģ������:calc_dht90(); 
;��    ��:��ʪ�Ȳ�������
;ռ����Դ:--
;����˵��:--
;��������:2008.08.15 
;��    ��:FV1.0(�����汾Function Version)
;�޸�����:--
;�޸�˵��:--
;-------------------------------------*/ 
void calc_dht90(float *p_humidity ,float *p_temperature)
// calculates temperature [C] and humidity [%RH] 
// input :  humi [Ticks] (12 bit) 
//          temp [Ticks] (14 bit)
// output:  humi [%RH]
//          temp [C]
{ const float C1=-4.0;              // for 12 Bit
  const float C2=+0.0405;           // for 12 Bit
  const float C3=-0.0000028;        // for 12 Bit
  const float T1=+0.01;             // for 14 Bit @ 5V
  const float T2=+0.00008;           // for 14 Bit @ 5V 

  float rh=*p_humidity;             // rh:      Humidity [Ticks] 12 Bit 
  float t=*p_temperature;           // t:       Temperature [Ticks] 14 Bit
  float rh_lin;                     // rh_lin:  Humidity linear
  float rh_true;                    // rh_true: Temperature compensated humidity
  float t_C;                        // t_C   :  Temperature [C]

  t_C=t*0.01 - 40;                  //calc. temperature from ticks to [C]
  rh_lin=C3*rh*rh + C2*rh + C1;     //calc. humidity from ticks to [%RH]
  rh_true=(t_C-25)*(T1+T2*rh)+rh_lin-3;   //calc. temperature compensated humidity [%RH]
  if(rh_true>100)rh_true=100;       //cut if the value is outside of
  if(rh_true<0.1)rh_true=0.1;       //the physical possible range

  *p_temperature=t_C;               //return temperature [C]
  *p_humidity=rh_true;              //return humidity[%RH]
}




//*********************�ڶ�����DHT90����   END****************************************

//*********������*****************
void main(void)
{
		value humi_val,temp_val;
        unsigned char error,checksum; 
        unsigned int wendu,shidu;
        LCD_init();	
        s_connectionreset(); 
        LCD_disp_str(0,1,"TE");
		LCD_disp_str(0,2,"RH");


//*********��ʼ���¶���ʾ��*********
        LCD_disp_str(2,1,"TTT.TC");

//*********��ʼ��ʪ����ʾ��*********
        LCD_disp_str(2,2,"RRR.R%");

        delay_n10us(20000);     //��ʱ0.2s

        while(1) 
        { error=0; 
          error+=s_measure((unsigned char*) &humi_val.i,&checksum,HUMI);  //measure humidity 
          error+=s_measure((unsigned char*) &temp_val.i,&checksum,TEMP);  //measure temperature 
          if(error!=0) s_connectionreset();                 //in case of an error: connection reset 
          else 
          { humi_val.f=(float)humi_val.i;                   //converts integer to float
            temp_val.f=(float)temp_val.i;                   //converts integer to float
            calc_dht90(&humi_val.f,&temp_val.f);            //calculate humidity, temperature
			wendu=10*temp_val.f;
			LCD_disp_char(2,1,wendu/1000+'0');              //��ʾ�¶Ȱ�λ
            LCD_disp_char(3,1,(wendu%1000)/100+'0');        //��ʾ�¶�ʮλ
            LCD_disp_char(4,1,(wendu%100)/10+'0');          //��ʾ�¶ȸ�λ
			LCD_disp_char(6,1,(wendu%10)+'0');              //��ʾ�¶�С������һλ

			shidu=10*humi_val.f;
			LCD_disp_char(2,2,shidu/1000+'0');               //��ʾʪ�Ȱ�λ
            LCD_disp_char(3,2,(shidu%1000)/100+'0');         //��ʾʪ��ʮλ
            LCD_disp_char(4,2,(shidu%100)/10+'0');           //��ʾʪ�ȸ�λ
			LCD_disp_char(6,2,(shidu%10)+'0');               //��ʾʪ��С������һλ
          } 
          //----------wait approx. 0.8s to avoid heating up SHTxx------------------------------       
		  delay_n10us(80000);                                //��ʱԼ0.8s
        }
}