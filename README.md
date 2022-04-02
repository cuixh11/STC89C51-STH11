# STC89C51 STH11 temperature and humidity acquisition and measurement system
 *任务书-基于STC89C51和STH11温湿度采集和测量系统的Proteus仿真* 

## 1. Task content of the project: 课题的任务内容
  Using Protues simulation software design to low power STC89C51 microcontroller as the core, equipped with a new type of low power sensor, LCD display LED data collection, temperature, humidity two parameters of the collection, processing and display functions. It is required to complete the design of temperature and humidity measurement circuit, SCM peripheral circuit, software flow, assembly language source program, data acquisition system design process and simulation.
  
 >  采用 Protues仿真软件设计以低功耗STC89C51单片机为核心，配置新式的微型低功耗传感器，使用液晶显示LED采集到的数据，实现温度、湿度两个参数的釆集、处理和显示等功能。要求完成温、湿度测量电路、单片机外围电路、软件流程、汇编语言源程序的设计、数据采集系统的设计过程及仿真等。
   
## 2.Technical requirements of design: 设计的技术要求
The software part of the temperature and humidity measurement system is based on Keil as the development platform, C language as the development language of the software system, and adopts modular programming. It is divided into the following parts: master control, temperature and humidity acquisition program, temperature and humidity data processing program, LCD display program, key setting program and LED, buzzer alarm program. The system uses SHT11 temperature and humidity sensor to sense the temperature and humidity of the surrounding environment, reads and processes the collected data through the microcontroller, and displays the temperature and humidity data in real time through the LCD1602 display module. At the same time, the upper and lower limits of temperature and humidity alarm can be set through the key module. When the temperature and humidity value read by SHT11 is not within the set range, the LED light of the alarm module indicates the fault information, and the buzzer alarms at the same time. When the temperature and humidity readings are normal, the LED indicator is off and the buzzer is off

>温湿度测量系统的软件部分是以Keil为开发平台，C语言为软件系统的开发语言,同时采用模块化编程。具体分为以下几个部分：主控制、温湿度采集程序、温湿度数据处理程序、LCD显示程序、按键设置程序和LED，蜂鸣器报警程序。系统通过SHT11温湿度传感器感应周围的环境的温度和湿度，通过单片机对采集到的数据进行读取处理，经过LCD1602显示模块实时显示温湿度数据，同时可以通过按键模块对温湿度报警上、下限值进行设定。当SHT11读取的温湿度值不在设定范围内时，报警模块LED灯指示故障信息，同时蜂鸣器报警；当温湿度读取数据正常后，LED灯熄灭，蜂鸣器关闭

## 仿真文件和论文报告完整版：
[链接](https://pan.baidu.com/s/1fEXZ7fWfRJ3dYbX0LhA3Eg?pwd=qgwq),提取码： qgwq 复制这段内容后打开百度网盘手机App，操作更方便哦




## 代码实现如下：
```
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
#include <intrins.h> //应用于程序设计，一般出C51单片机编程中，一般程序中需要使用到空指令_nop_();字符循环移位指令_crol_等时使用。
#include <math.h>    //Keil library   
#include <stdio.h>   //Keil library 


//*********************第一部分LCD1602设置 START****************************************
#define      LCD_DB P0	     //Data Bus数据总线,我们这里要做 1602 液晶的程序，因此先把用到的总线接口做一个统一声明
sbit         LCD_RS=P2^0;   
sbit         LCD_RW=P2^1;   
sbit         LCD_E =P2^2;  
		  
/******定义函数****************/
#define uchar unsigned char
#define uint unsigned int

void LCD_init(void);                           //初始化函数
void LCD_write_command(uchar command);         //写指令函数
void LCD_write_data(uchar dat);                //写数据函数
void LCD_disp_char(uchar x,uchar y,uchar dat); //在某个屏幕位置上显示一个字符,X（0-15),y(1-2)
void LCD_disp_str(uchar x,uchar y,uchar *str); //LCD1602显示字符串函数
void delay_n10us(uint n);                      //延时函数

/*-------------------------------------- 
;模块名称:LCD_init(); 
;功    能:初始化LCD1602 
;-------------------------------------*/ 
void LCD_init(void)		//括号中的void意思该函数的参数为空
{
delay_n10us(10);
LCD_write_command(0x38);//指令6：工作方式设置指令，0x0011 1000设置8位格式数据接口，16*2行显示，5x7点阵
delay_n10us(10);
LCD_write_command(0x0c);//指令4:
						//整体显示，关光标，不闪烁，BCD0x00001 100
						//整体显示，开光标，  闪烁，BCD0x00001 111
delay_n10us(10);
LCD_write_command(0x06);//指令3：设定输入方式增量不移位,    0x00000110
delay_n10us(10);
LCD_write_command(0x01);//指令1：清除屏幕显示,光标复位到地址00H位置
delay_n10us(100);       //延时清屏，延时函数，延时约n个10us
}


/*-------------------------------------- 
;模块名称:LCD_write_command(); 
;功    能:LCD1602写指令函数 
;占用资源: P2.0--RS(LCD_RS),P2.1--RW(LCD_RW),P2.2--E(LCD_E).
;参数说明:dat为写命令参数
;-------------------------------------*/ 
void LCD_write_command(uchar dat)
{
delay_n10us(10);
LCD_RS=0;         //指令
LCD_RW=0;         //写入
LCD_E=1;          //允许
LCD_DB=dat;
delay_n10us(10);  //实践证明，我的LCD1602上，用for循环1次就能完成普通写指令。
LCD_E=0;		  //读完撤销使能，防止液晶输出数据干扰 P0 总线 
delay_n10us(10);  //实践证明，LCD1602上，用for循环1次就能完成普通写指令。
}


/*-------------------------------------- 
;模块名称:LCD_write_data(); 
;功    能:LCD1602写数据函数 
;占用资源: P2.0--RS(LCD_RS),P2.1--RW(LCD_RW),P2.2--E(LCD_E).
;参数说明:dat为写数据参数
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
;功    能:光标位置，LCD1602显示一个字符函数，在某个屏幕位置上显示一个字符,X（0-15),y(1-2)。
;参数说明:X为1602的列值(取值范围是0-15),y为1602的行值(取值范围是1-2)，dat为所要显示字符对应的地址参数。
;-------------------------------------*/
void LCD_disp_char(uchar x,uchar y,uchar dat)
{
  uchar address;
  if(y==1)				 //0x80和00H的区别：指令7
         address=0x80+x; //二进制码中第一行首地址为10000000转换16进制为0x80,第二行首地址为11000000转换16进制为0xC0 加首地址是为了让LCD显示从第一位开始显示。
  else
         address=0xc0+x;
  LCD_write_command(address);
  LCD_write_data(dat);
}



/*-------------------------------------- 
;模块名称:LCD_disp_str(); 
;功    能:LCD1602显示字符串函数，在某个屏幕起始位置{X（0-15),y(1-2)}上显示一个字符串。
;参数说明:X为1602的列值(取值范围是0-15),y为1602的行值(取值范围是1-2)，str为所要显示字符串对应的指针参数。
;-------------------------------------*/
void LCD_disp_str(uchar x,uchar y,uchar *str)
{
  uchar address;
  if(y==1)
         address=0x80+x;
  else
         address=0xc0+x;
  LCD_write_command(address);  //连续写入字符串数据，直到检测到结束符
  while(*str!='\0')
  { 
    LCD_write_data(*str);      //另外一种写法LCD_write_data(*str++)这行语句中对指针 str 的操作大家一定要理解透彻，先把 str指向的数据取出来用，然后 str 再加 1 以指向下一个数据，这是非常常用的一种简写方式
    str++;
  }
}


/*-------------------------------------- 
;模块名称:delay_n10us(); 
;功    能:延时函数，延时约n个10us
;修改日期:2022.02.26
;修改说明:修改为较精确的延时函数,"_nop_()"延时1us@12M晶振,代表运行一个机器周期。如果这个单片机的晶振是12M的，那么这调代码会运行1US；
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



//*********************第二部分SHT11设置   START**************************************** 
sbit SCK  = P2^6;      //定义通讯时钟端口
sbit DATA = P2^7;      //定义通讯数据端口

typedef union  		   //定义了两个共用体:如果没有typedef那么就是普通的定义了匿名联合的一个变量value.加了typedef后, 定义的就是类型别名, 当类型一样用
{ 
unsigned int i;        //i表示测量得到的温湿度数据（int 形式保存的数据）
  float f; 			   //f表示测量得到的温湿度数据（float 形式保存的数据）
} value; 

enum {TEMP,HUMI};      //enum　枚举名　{枚举元素1,枚举元素2,……};TEMP=0,HUMI=1	枚举：https://www.runoob.com/cprogramming/c-enum.html
 

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


//*********************第三部分报警设置   START****************************************
#define N   0x00
sbit SET = 	P1^5;                   //定义调整键
sbit ADD =	P1^6;                   //定义增加键
sbit DEC = 	P1^7;                   //定义减少键
sbit ESC =  P3^7;                   //定义调整键
sbit BEEP=  P1^4;                   //定义蜂鸣器

sbit  t_green   =  P1^0;
sbit  t_red     =  P1^1;
sbit  rh_green  =  P1^2;
sbit  rh_red    =  P1^3;

int t_shangxian = 20;            //上限报警温度，默认值为 20C
int t_xiaxian   = 10;            //下限报警温度，默认值为 10C
int rh_shangxian= 50;            //上限报警湿度，默认值为50%
int rh_xiaxian  = 40;            //下限报警湿度，默认值为40%

uchar flag=0;


/****************定义函数****************/
void led_control( float *WENDU,  float *SHIDU);
void key();
void delay(uint j);
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


//*********主函数*****************
void main(void)
{
	       value humi_val,temp_val;				     //185行，定义两个共同体，一个用于湿度，一个用于温度
	       unsigned char error,checksum; 			 //用于检验是否出现错误
	       unsigned int wendu,shidu;				 //最终,一位小数温湿度的值
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



```
