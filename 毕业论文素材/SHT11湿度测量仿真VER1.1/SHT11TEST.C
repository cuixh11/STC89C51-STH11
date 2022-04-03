/*-------------------------------
作者：小新
qq：405228062
email:xuxing1261207@126.com
淘宝：http://xx-mcu.taobao.com
--------------------------------*/
#include<reg52.h>
#include <intrins.h>
#include <math.h>    //Keil library 
#include <stdio.h>	 //Keil library

#define uchar unsigned char

typedef union		   //定义共同类型
{ unsigned int i;	   //i表示测量得到的温湿度数据（int 形式保存的数据） 
 float f;			   //f表示测量得到的温湿度数据（float 形式保存的数据）
} value;


enum {TEMP,HUMI};

sbit DATA = P1^1;
sbit SCK = P1^0;

sbit RS  = P2^0;
sbit RW  = P2^1;
sbit E  = P2^2;
sfr DBPort  = 0x80;     //P0=0x80,P1=0x90,P2=0xA0,P3=0xB0.数据端口

/********     1602函数声明     ********/
void LCD_Initial();
void GotoXY(unsigned char x, unsigned char y);
void Print(unsigned char *str);
void LCD_Write(bit style, unsigned char input);


/********     SHT11函数声明      ********/
void s_connectionreset(void);
char s_measure(unsigned char *p_value, unsigned char *p_checksum, unsigned char mode);
void calc_sth10(float *p_humidity ,float *p_temperature);
//float calc_dewpoint(float h,float t);



/***********************************************************************************************************************************************************/

//SHT11程序


#define noACK 0				//继续传输数据，用于判断是否结束通讯
#define ACK   1             //结束数据传输；
							//地址  命令  读/写
#define STATUS_REG_W 0x06   //000   0011    0
#define STATUS_REG_R 0x07   //000   0011    1
#define MEASURE_TEMP 0x03   //000   0001    1
#define MEASURE_HUMI 0x05   //000   0010    1
#define RESET        0x1e   //000   1111    0



//写字节程序
char s_write_byte(unsigned char value)   
{ 
	unsigned char i,error=0; 
	for (i=0x80;i>0;i/=2)             //shift bit for masking 高位为1，循环右移
	{ 
		if (i&value) DATA=1;          //和要发送的数相与，结果为发送的位
    	else DATA=0;                        
    	SCK=1;                          
    	_nop_();_nop_();_nop_();        //延时3us 
    	SCK=0;
	}
	DATA=1;                           //释放数据线
	SCK=1;                            
	error=DATA;                       //检查应答信号，确认通讯正常
	_nop_();_nop_();_nop_();
	SCK=0;        
	DATA=1;
	return error;                     //error=1 通讯错误
}

//读字节程序
char s_read_byte(unsigned char ack)
//----------------------------------------------------------------------------------
{ 
	unsigned char i,val=0;
	DATA=1;                           //释放数据线
	for(i=0x80;i>0;i>>=1)             //高位为1，循环右移
	{ 
		SCK=1;                         
    	if(DATA) val=(val|i);        //读一位数据线的值 
    	SCK=0;       
	}
	DATA=!ack;                        //如果是校验，读取完后结束通讯；
	SCK=1;                            
	_nop_();_nop_();_nop_();          //延时3us 
	SCK=0;   
	_nop_();_nop_();_nop_();       
	DATA=1;                           //释放数据线
	return val;
}

//启动传输
void s_transstart(void)
// generates a transmission start 
//       _____         ________
// DATA:      |_______|
//           ___     ___
// SCK : ___|   |___|   |______
{ 
   	DATA=1; SCK=0;                   //准备
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


//连接复位
void s_connectionreset(void)
// communication reset: DATA-line=1 and at least 9 SCK cycles followed by transstart
//       _____________________________________________________         ________
// DATA:                                                      |_______|
//          _    _    _    _    _    _    _    _    _        ___     ___
// SCK : __| |__| |__| |__| |__| |__| |__| |__| |__| |______|   |___|   |______
{ 
	unsigned char i; 
	DATA=1; SCK=0;                    //准备
	for(i=0;i<9;i++)                  //DATA保持高，SCK时钟触发9次，发送启动传输，通迅即复位
	{ 
		SCK=1;
    	SCK=0;
	}
	s_transstart();                   //启动传输
}


//软复位程序
char s_softreset(void)
// resets the sensor by a softreset 
{ 
	unsigned char error=0; 
	s_connectionreset();              //启动连接复位
	error+=s_write_byte(RESET);       //发送复位命令
	return error;                     //error=1 通讯错误
}


/*
//读状态寄存器
char s_read_statusreg(unsigned char *p_value, unsigned char *p_checksum)
//----------------------------------------------------------------------------------
// reads the status register with checksum (8-bit)
{ 
	unsigned char error=0;
	s_transstart();                   //transmission start
	error=s_write_byte(STATUS_REG_R); //send command to sensor
	*p_value=s_read_byte(ACK);        //read status register (8-bit)
	*p_checksum=s_read_byte(noACK);   //read checksum (8-bit) 
	return error;                     //error=1 in case of no response form the sensor
}

//写状态寄存器
char s_write_statusreg(unsigned char *p_value)
// writes the status register with checksum (8-bit)
{ 
	unsigned char error=0;
	s_transstart();                   //transmission start
	error+=s_write_byte(STATUS_REG_W);//send command to sensor
	error+=s_write_byte(*p_value);    //send value of status register
	return error;                     //error>=1 in case of no response form the sensor
}															*/			   

           
//温湿度测量
char s_measure(unsigned char *p_value, unsigned char *p_checksum, unsigned char mode)
// 进行温度或者湿度转换，由参数mode决定转换内容；
{ 
//	enum {TEMP,HUMI};		 //已经在头文件中定义
	unsigned error=0;
	unsigned int i;

	s_transstart();                   //启动传输
	switch(mode)                     //选择发送命令
    {	
		case TEMP : error+=s_write_byte(MEASURE_TEMP); break;		   //测量温度
    	case HUMI : error+=s_write_byte(MEASURE_HUMI); break;		   //测量湿度
    	default     : break; 
	}
	for (i=0;i<65535;i++) if(DATA==0) break; //等待测量结束
	if(DATA) error+=1;                // 如果长时间数据线没有拉低，说明测量错误 
	*(p_value) =s_read_byte(ACK);    //读第一个字节，高字节 (MSB)
	*(p_value+1)=s_read_byte(ACK);    //读第二个字节，低字节 (LSB)
	*p_checksum =s_read_byte(noACK); //read CRC校验码
	return error;					 // error=1 通讯错误
}

//温湿度值标度变换及温度补偿
void calc_sth10(float *p_humidity ,float *p_temperature)
{ 
	const float C1=-4.0;              // 12位湿度精度 修正公式
	const float C2=+0.0405;           // 12位湿度精度 修正公式
	const float C3=-0.0000028;        // 12位湿度精度 修正公式
	const float T1=+0.01;             // 14位温度精度 5V条件  修正公式
	const float T2=+0.00008;          // 14位温度精度 5V条件  修正公式

	float rh=*p_humidity;             // rh:      12位 湿度 
	float t=*p_temperature;           // t:       14位 温度
	float rh_lin;                     // rh_lin: 湿度 linear值
	float rh_true;                    // rh_true: 湿度 ture值
	float t_C;                        // t_C   : 温度 ℃

	t_C=t*0.01 - 40;                  //补偿温度
	rh_lin=C3*rh*rh + C2*rh + C1;     //相对湿度非线性补偿
	rh_true=(t_C-25)*(T1+T2*rh)+rh_lin;   //相对湿度对于温度依赖性补偿
	if(rh_true>100)rh_true=100;       //湿度最大修正
	if(rh_true<0.1)rh_true=0.1;       //湿度最小修正

	*p_temperature=t_C;               //返回温度结果
	*p_humidity=rh_true;              //返回湿度结果
}

//从相对温度和湿度计算露点
/*float calc_dewpoint(float h,float t)
{
	float logEx,dew_point;
	logEx=0.66077+7.5*t/(237.3+t)+(log10(h)-2);
	dew_point = (logEx - 0.66077)*237.3/(0.66077+7.5-logEx);
	return dew_point;
}						   					*/

/***********************************************************************************************************************************************************/
//1602程序


//内部等待函数**************************************************************
unsigned char LCD_Wait(void)
{
    RS=0;
    RW=1;    _nop_();
    E=1;    _nop_();           
    E=0;
    return DBPort;  
}
//向LCD写入命令或数据********************************************************
#define LCD_COMMAND         0      // Command
#define LCD_DATA            1      // Data
#define LCD_CLEAR_SCREEN    0x01      // 清屏
#define LCD_HOMING          0x02      // 光标返回原点
void LCD_Write(bit style, unsigned char input)
{
    E=0;
    RS=style;
    RW=0;        _nop_();
    DBPort=input;   _nop_();//注意顺序
    E=1;        _nop_();//注意顺序
    E=0;        _nop_();
    LCD_Wait(); 
}

//设置显示模式************************************************************
#define LCD_SHOW            0x04    //显示开
#define LCD_HIDE            0x00    //显示关 

#define LCD_CURSOR          0x02    //显示光标
#define LCD_NO_CURSOR       0x00    //无光标        

#define LCD_FLASH           0x01    //光标闪动
#define LCD_NO_FLASH        0x00    //光标不闪动

void LCD_SetDisplay(unsigned char DisplayMode)
{
    LCD_Write(LCD_COMMAND, 0x08|DisplayMode); 
}

//设置输入模式************************************************************
#define LCD_AC_UP       0x02
#define LCD_AC_DOWN         0x00      // default

#define LCD_MOVE            0x01      // 画面可平移
#define LCD_NO_MOVE         0x00      //default

void LCD_SetInput(unsigned char InputMode)
{
    LCD_Write(LCD_COMMAND, 0x04|InputMode);
}

//初始化LCD************************************************************
void LCD_Initial()
{
    E=0;
    LCD_Write(LCD_COMMAND,0x38);           //8位数据端口,2行显示,5*7点阵
    LCD_Write(LCD_COMMAND,0x38);
    LCD_SetDisplay(LCD_SHOW|LCD_NO_CURSOR);    //开启显示, 无光标
    LCD_Write(LCD_COMMAND,LCD_CLEAR_SCREEN);   //清屏
    LCD_SetInput(LCD_AC_UP|LCD_NO_MOVE);       //AC递增, 画面不动
}

//液晶字符输入的位置************************
void GotoXY(unsigned char x, unsigned char y)
{
    if(y==0)
        LCD_Write(LCD_COMMAND,0x80|x);
    if(y==1)
        LCD_Write(LCD_COMMAND,0x80|(x-0x40));
}

//将字符输出到液晶显示
void Print(unsigned char *str)
{
    while(*str!='\0')
    {
        LCD_Write(LCD_DATA,*str);
        str++;
    }
}



//延时函数
void delay(int z)		//z为毫秒数
{
	int x,y;
	for(x=z;x>0;x--)
		for(y=125;y>0;y--);
}

/***********************************************************************************************************************************************************/
//主函数

void main()
{ 
	unsigned int temp,humi;
	value humi_val,temp_val;		//定义两个共同体，一个用于湿度，一个用于温度
//	float dew_point;			    //用于记录露点值
	unsigned char error;		    //用于检验是否出现错误
	unsigned char checksum;			//CRC			 
	uchar TEMP1[7];				 	//用于记录温度
	uchar HUMI1[6];				 	//用于记录湿度
	
					 
	LCD_Initial();					   //初始化液晶		  
   	GotoXY(0,0);					   //选择温度显示位置
   	Print("Temp:      C");			   //5格空格
 	GotoXY(0,1);					   //选择湿度显示位置
   	Print("Humi:     %RH");			   //5格空格
	s_connectionreset();
				   //启动连接复位
	while(1)
	{
		error=0;					   //初始化error=0，即没有错误
		error+=s_measure((unsigned char*)&temp_val.i,&checksum,TEMP); //温度测量
		error+=s_measure((unsigned char*)&humi_val.i,&checksum,HUMI); //湿度测量
    	if(error!=0) s_connectionreset();                 ////如果发生错误，系统复位
    	else
    	{ 
			humi_val.f=(float)humi_val.i;                   //转换为浮点数
     		temp_val.f=(float)temp_val.i;                   //转换为浮点数
     		calc_sth10(&humi_val.f,&temp_val.f);            //修正相对湿度及温度
//   		dew_point=calc_dewpoint(humi_val.f,temp_val.f); //计算e dew_point
   			temp=temp_val.f*10;
     		humi=humi_val.f*10;
     		GotoXY(6,0);						//设置温度显示位置
      		TEMP1[0]=temp/1000+'0';     		//温度百位 
			if (TEMP1[0]==0x30) TEMP1[0]=0x20;
			TEMP1[1]=temp%1000/100+'0';     	//温度十位	
			if (TEMP1[1]==0x30 && TEMP1[0]!=0x30) TEMP1[1]=0x20;
   			TEMP1[2]=temp%100/10+'0';			//温度个位
      		TEMP1[3]=0x2e;						//小数点
   			TEMP1[4]=temp%10+'0';				//温度小数点后第一位		
			TEMP1[5]=0xdf;						//显示温度符号℃
			Print(TEMP1);						//输出温度	   	  
	    	GotoXY(6,1);						//设置湿度显示位置					 
			HUMI1[0]=humi/1000+'0';     		//湿度百位
      		if (HUMI1[0]==0x30) HUMI1[0]=0x20;
			HUMI1[1]=humi%1000/100+'0';     	//湿度十位	
   			if (HUMI1[1]==0x30 && HUMI1[0]!=0x30) HUMI1[1]=0X20;  
			HUMI1[2]=humi%100/10+'0';			//湿度个位
      		HUMI1[3]=0x2E;						//小数点
   			HUMI1[4]=humi%10+'0';				//湿度小数点后第一位	
			Print(HUMI1);						//输出湿度	   
    	}  
		delay(800);                             //等待足够长的时间，以进行下一次转换                    
	}
}
