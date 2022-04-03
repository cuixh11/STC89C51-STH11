/*********************************************************************************** 
;功能说明:DHT90与LCD1602的温湿度显示 
;文件名称:DHT90.c    
 
;微处理器:AT89S52
;编译环境:Keil uVision V2.38a
 
;作    者:Huang Yanxiang
;版    权:(c)Huang Yanxiang
;创建日期:2008.08.15    
;版    本:V1.2  
;修改日期:2008.08.26
;修改说明:添加了一个LCD_disp_str()函数，修改了延时函数的注释。（感谢ouravr的网友wyb4993的提醒）
;***********************************************************************************/

/*************定义接口********************
         P0------DB0～DB7  (LCD1602)     
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

//*********************第一部分LCD1602设置 START****************************************
#define LCD_DB        P0
        sbit         LCD_RS=P2^0;   //P2^0是p2.0的意思；LCD_RS与P2.0等效起来，对LCD_RS 读写，就是对P2.0读写 好处在于LCD_RS含义直接明了，写程序多了就会知道有必要de
        sbit         LCD_RW=P2^1;   //P2^1是p2.1的意思
        sbit         LCD_E=P2^2;    //P2^2是p2.2的意思

/******定义函数****************/
#define uchar unsigned char
#define uint unsigned int
void LCD_init(void);                          //初始化函数
void LCD_write_command(uchar command);        //写指令函数
void LCD_write_data(uchar dat);               //写数据函数
void LCD_disp_char(uchar x,uchar y,uchar dat);//在某个屏幕位置上显示一个字符,X（0-15),y(1-2)
void LCD_disp_str(uchar x,uchar y,uchar *str); //LCD1602显示字符串函数
void delay_n10us(uint n);                     //延时函数


/*-------------------------------------- 
;模块名称:LCD_init(); 
;功    能:初始化LCD1602 
;占用资源:--
;参数说明:--
;创建日期:2008.08.15 
;版    本:FV1.0(函数版本Function Version)
;修改日期:--
;修改说明:--
;-------------------------------------*/ 
void LCD_init(void)
{
delay_n10us(10);
LCD_write_command(0x38);//设置8位格式，2行，5x7
delay_n10us(10);
LCD_write_command(0x0c);//整体显示，关光标，不闪烁
delay_n10us(10);
LCD_write_command(0x06);//设定输入方式，增量不移位
delay_n10us(10);
LCD_write_command(0x01);//清除屏幕显示
delay_n10us(100);       //延时清屏，延时函数，延时约n个10us
}


/*-------------------------------------- 
;模块名称:LCD_write_command(); 
;功    能:LCD1602写指令函数 
;占用资源: P2.0--RS(LCD_RS),P2.1--RW(LCD_RW),P2.2--E(LCD_E).
;参数说明:dat为写命令参数
;创建日期:2008.08.15 
;版    本:FV1.0(函数版本Function Version)
;修改日期:--
;修改说明:--
;-------------------------------------*/ 
void LCD_write_command(uchar dat)
{
delay_n10us(10);
LCD_RS=0;         //指令
LCD_RW=0;         //写入
LCD_E=1;          //允许
LCD_DB=dat;
delay_n10us(10);  //实践证明，我的LCD1602上，用for循环1次就能完成普通写指令。
LCD_E=0;
delay_n10us(10);  //实践证明，我的LCD1602上，用for循环1次就能完成普通写指令。
}


/*-------------------------------------- 
;模块名称:LCD_write_data(); 
;功    能:LCD1602写数据函数 
;占用资源: P2.0--RS(LCD_RS),P2.1--RW(LCD_RW),P2.2--E(LCD_E).
;参数说明:dat为写数据参数
;创建日期:2008.08.15 
;版    本:FV1.0(函数版本Function Version)
;修改日期:--
;修改说明:--
;-------------------------------------*/
void LCD_write_data(uchar dat)
{
delay_n10us(10);
LCD_RS=1;          //数据
LCD_RW=0;          //写入
LCD_E=1;           //允许
LCD_DB=dat;
delay_n10us(10);
LCD_E=0;
delay_n10us(10);
}


/*-------------------------------------- 
;模块名称:LCD_disp_char(); 
;功    能:LCD1602显示一个字符函数，在某个屏幕位置上显示一个字符,X（0-15),y(1-2)。
;占用资源:--
;参数说明:X为1602的列值(取值范围是0-15),y为1602的行值(取值范围是1-2)，dat为所要显示字符对应的地址参数。
;创建日期:2008.08.15 
;版    本:FV1.0(函数版本Function Version)
;修改日期:--
;修改说明:--
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
;模块名称:LCD_disp_str(); 
;功    能:LCD1602显示字符串函数，在某个屏幕起始位置{X（0-15),y(1-2)}上显示一个字符串。
;占用资源:--
;参数说明:X为1602的列值(取值范围是0-15),y为1602的行值(取值范围是1-2)，str为所要显示字符串对应的指针参数。
;创建日期:2008.08.16 
;版    本:FV1.0(函数版本Function Version)
;修改日期:--
;修改说明:-- 
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
;模块名称:delay_n10us(); 
;功    能:延时函数，延时约n个10us
;占用资源:--
;参数说明:--
;创建日期:2008.08.15 
;版    本:FV1.1(函数版本Function Version)
;修改日期:2008.08.26
;修改说明:修改为较精确的延时函数,"_nop_()"延时1us@12M晶振
;-------------------------------------*/
void delay_n10us(uint n)  //延时n个10us@12M晶振
{       
        uint i;           
        for(i=n;i>0;i--)    
        {
        _nop_();_nop_();_nop_();_nop_();_nop_();_nop_(); 
		}
}                                    

//*********************第一部分LCD1602设置 END****************************************


//*********************第二部分DHT90设置   START**************************************** 
sbit SCK  = P2^6;      //定义通讯时钟端口
sbit DATA = P2^7;      //定义通讯数据端口

typedef union  
{ unsigned int i;      //定义了两个共用体
  float f; 
} value; 

enum {TEMP,HUMI};      //TEMP=0,HUMI=1
 

#define noACK 0             //用于判断是否结束通讯
#define ACK   1             //结束数据传输
                            //adr  command  r/w 
#define STATUS_REG_W 0x06   //000   0011    0 
#define STATUS_REG_R 0x07   //000   0011    1 
#define MEASURE_TEMP 0x03   //000   0001    1 
#define MEASURE_HUMI 0x05   //000   0010    1 
#define RESET        0x1e   //000   1111    0 

/****************定义函数****************/
void s_transstart(void);               //启动传输函数
void s_connectionreset(void);          //连接复位函数
char s_write_byte(unsigned char value);//DHT90写函数
char s_read_byte(unsigned char ack);   //DHT90读函数
char s_measure(unsigned char *p_value, unsigned char *p_checksum, unsigned char mode);//测量温湿度函数
void calc_dht90(float *p_humidity ,float *p_temperature);//温湿度补偿



/*-------------------------------------- 
;模块名称:s_transstart(); 
;功    能:启动传输函数
;占用资源:--
;参数说明:--
;创建日期:2008.08.15 
;版    本:FV1.0(函数版本Function Version)
;修改日期:--
;修改说明:--
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
;模块名称:s_connectionreset(); 
;功    能:连接复位函数
;占用资源:--
;参数说明:--
;创建日期:2008.08.15 
;版    本:FV1.0(函数版本Function Version)
;修改日期:--
;修改说明:--
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
;模块名称:s_write_byte(); 
;功    能:DHT90写函数
;占用资源:--
;参数说明:--
;创建日期:2008.08.15 
;版    本:FV1.0(函数版本Function Version)
;修改日期:--
;修改说明:--
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
  error=DATA;                       //check ack (DATA will be pulled down by DHT90),DATA在第9个上升沿将被DHT90自动下拉为低电平。 
  _nop_();_nop_();_nop_();
  SCK=0;
  DATA=1;                           //release DATA-line 
  return error;                     //error=1 in case of no acknowledge //返回：0成功，1失败
} 
 

/*-------------------------------------- 
;模块名称:s_read_byte(); 
;功    能:DHT11读函数
;占用资源:--
;参数说明:--
;创建日期:2008.08.15 
;版    本:FV1.0(函数版本Function Version)
;修改日期:--
;修改说明:--
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
;占用资源:--
;参数说明:--
;创建日期:2008.08.15 
;版    本:FV1.0(函数版本Function Version)
;修改日期:--
;修改说明:--
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
;模块名称:calc_dht90(); 
;功    能:温湿度补偿函数
;占用资源:--
;参数说明:--
;创建日期:2008.08.15 
;版    本:FV1.0(函数版本Function Version)
;修改日期:--
;修改说明:--
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




//*********************第二部分DHT90设置   END****************************************

//*********主函数*****************
void main(void)
{
		value humi_val,temp_val;
        unsigned char error,checksum; 
        unsigned int wendu,shidu;
        LCD_init();	
        s_connectionreset(); 
        LCD_disp_str(0,1,"TE");
		LCD_disp_str(0,2,"RH");


//*********初始化温度显示区*********
        LCD_disp_str(2,1,"TTT.TC");

//*********初始化湿度显示区*********
        LCD_disp_str(2,2,"RRR.R%");

        delay_n10us(20000);     //延时0.2s

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
			LCD_disp_char(2,1,wendu/1000+'0');              //显示温度百位
            LCD_disp_char(3,1,(wendu%1000)/100+'0');        //显示温度十位
            LCD_disp_char(4,1,(wendu%100)/10+'0');          //显示温度个位
			LCD_disp_char(6,1,(wendu%10)+'0');              //显示温度小数点后第一位

			shidu=10*humi_val.f;
			LCD_disp_char(2,2,shidu/1000+'0');               //显示湿度百位
            LCD_disp_char(3,2,(shidu%1000)/100+'0');         //显示湿度十位
            LCD_disp_char(4,2,(shidu%100)/10+'0');           //显示湿度个位
			LCD_disp_char(6,2,(shidu%10)+'0');               //显示湿度小数点后第一位
          } 
          //----------wait approx. 0.8s to avoid heating up SHTxx------------------------------       
		  delay_n10us(80000);                                //延时约0.8s
        }
}