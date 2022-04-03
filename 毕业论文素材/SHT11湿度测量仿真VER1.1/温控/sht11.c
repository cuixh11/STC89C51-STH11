#include <reg52.h>	  
#include <intrins.h> //Keil library (is used for _nop()_ operation)  
#include <math.h>    //Keil library  
#include <stdio.h>   //Keil library
#include <stceeprom.h>

#define	uchar unsigned char	  //  声明文件标志
#define uint unsigned int
#define noACK 0
#define ACK   1
                           //adr  command  r/w
#define STATUS_REG_W 0x06   //000   0011    0
#define STATUS_REG_R 0x07   //000   0011    1
#define MEASURE_TEMP 0x03   //000   0001    1
#define MEASURE_HUMI 0x05   //000   0010    1
#define RESET        0x1e   //000   1111    0
//---------全局变量--------    																			 
uint t;
uchar k1,k2,k3,sl,msec,disbuf[4];		 
code uchar  tab[16]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,  
                0xC6,  //"C"
                0x86,  //"E"
                0x89,  //"H"
                0xC7,  //"L"
                0xBF  //"-"
};
typedef union 
{ 
	uint i;
  	float f;
}value;
enum {TEMP,HUMI};
//---------7459-------- 
sbit D_dat=P2^1;    //定义驱动74595的数据脚为p1.0
sbit D_clk=P2^0;    //74595的时钟
sbit D_lat=P2^2;    //栓锁
//---------动态数码位-------- 
sbit  gw=P0^3;   //个位
sbit  sw=P0^2;    //十位
sbit  bw=P0^1;    //百位
sbit  qw=P0^0;

sbit CTR1 = P0^4;	 // 温度上限越限灯
sbit CTR2 = P0^5;	 // 温度下限
sbit CTR3 = P0^6;	 // 湿度上限
sbit CTR4 = P0^7;	 // 湿度下限

sbit dat = P1^0;	 //	sht11传感器数据线
sbit SCK = P1^6;	 // sht11传感器时钟

sbit key1=P1^2;			 // 按键  功能选择
sbit key2=P1^3;			 // 减小键
sbit key3=P1^4;			 // 增加键

sbit beep=P1^5;

//---------延时函数-----------
void delay(uchar a)	
{
    uchar i;
    while (--a!=0)
    {
	   for (i=0;i<100;i++);	
	}
}
dsout(uchar j)
{
 unsigned char i; 
 for(i=0;i<8;i++)
    {
     	D_dat=j&0x80;       //高位在前
    	 D_clk=1;
    	 D_clk=0;
    	 j<<=1;
    }
  D_lat=1;
  D_lat=0;               //锁存
}
//----74HC595显示程序--------------------
display(uchar sjqw,sjbw,sjsw,sjgw)
{ uint i;
  dsout(tab[sjgw]);  gw=0;for(i=0;i<=1000;i++);gw=1;
  dsout(tab[sjsw]);  sw=0;for(i=0;i<=1000;i++);sw=1; 
  dsout(tab[sjbw]);  bw=0;for(i=0;i<=1000;i++);bw=1;
  dsout(tab[sjqw]);  qw=0;for(i=0;i<=1000;i++);qw=1; 
}
display1(uchar sjqw,sjbw,sjsw,sjgw)
{ 
	uint i;
	for(i=0;i<=80;i++)
	{
    	dsout(tab[sjgw]);  gw=0;delay(3);gw=1;
  		dsout(tab[sjsw]);  sw=0;delay(3);sw=1; 
  		dsout(tab[sjbw]+0x80);  bw=0;delay(3);bw=1;
  		dsout(tab[sjqw]);  qw=0;delay(3);qw=1;
   } 
}  
//------蜂鸣器---------
void ming(uchar c)		
{		uint i;
		
        for(i=0;i<1;i++)
		{		
	//	beep=1;
		beep=0;	
		display1(t/100,(t%100)/10,t%10,c);
		}
	//	
}			
//------sht11功能函数---------
char s_write_byte(uchar value)	   // writes a byte on the Sensibus and checks the acknowledge 
{ 
  uchar i,n,error=0;  
  for (i=0x80;i>0;i/=2)             //shift bit for masking
  { 
 	 if (i & value) dat=1;          //masking value with i , write to SENSI-BUS
 	   else dat=0;                        
    SCK=1; 
	  for(n=0;n<10;n++) _nop_();    //pulswith approx. 5 us                         //clk for SENSI-BUS
    _nop_();_nop_();        //pulswith approx. 5 us   
    SCK=0;
	_nop_();
	  for(n=0;n<7;n++) _nop_();    //pulswith approx. 5 us
  }
  dat=1;
     for(n=0;n<5;n++) _nop_();                           //release DATA-line
  SCK=1;                            //clk #9 for ack 
  error=dat;
     for(n=0;n<5;n++) _nop_();                       //check ack (DATA will be pulled down by SHT11)
  SCK=0;
    dat=1;                           //release DATA-line          
  return error;                     //error=1 in case of no acknowledge	 此刻为0 ranhou从机释放数据线
}
char s_read_byte(uchar ack)	  // reads a byte form the Sensibus and gives an acknowledge in case of "ack=1" 
{ 
	uchar i,val=0;
	dat=1;                           //release DATA-line
	for (i=0x80;i>0;i/=2)             //shift bit for masking
	{ 
 		SCK=1;                          //clk for SENSI-BUS
    	if (dat) val=(val|i);        //read bit
		_nop_();_nop_();_nop_();        //pulswith approx. 5 us   
    	SCK=0;        
	}
    if(ack==1)dat=0;                 //in case of "ack==1" pull down DATA-Line  
  	else dat=1;                      //如果是校验(ack==0)，读取完后结束通讯 

	//dat=!ack;                        //in case of "ack==1" pull down DATA-Line
	SCK=1;                            //clk #9 for ack
	_nop_();_nop_();_nop_();          //pulswith approx. 5 us 
	SCK=0;
    _nop_();_nop_();_nop_();          //pulswith approx. 5 us            
	dat=1;                           //release DATA-line
	return val;
}

void s_transstart(void)
//----------------------------------------------------------------------------------
// generates a transmission start 
//       _____         ________
// DATA:      |_______|
//           ___     ___
// SCK : ___|   |___|   |______
{  
   dat=1; SCK=0;                   //Initial state
   _nop_();
   SCK=1;
   _nop_();
   dat=0;
   _nop_();
   SCK=0;  
   _nop_();_nop_();_nop_();
   SCK=1;
   _nop_();
   dat=1;     
   _nop_();
   SCK=0;     
}//----------------------------------------------------------------------------------
void s_connectionreset(void)
//----------------------------------------------------------------------------------
// communication reset: DATA-line=1 and at least 9 SCK cycles followed by transstart
//       _____________________________________________________         ________
// DATA:                                                      |_______|
//          _    _    _    _    _    _    _    _    _        ___     ___
// SCK : __| |__| |__| |__| |__| |__| |__| |__| |__| |______|   |___|   |______
{  
	uchar i; 
	dat=1; SCK=0;                    //Initial state
	for(i=0;i<9;i++)                  //9 SCK cycles
	{ 
		SCK=1;
    	_nop_();
    	SCK=0;
		_nop_();
 	}
  	s_transstart();                   //transmission start
}
char s_measure(uchar *p_value, uchar *p_checksum, uchar mode)
{ 
	uchar error=0;
	uint i;
	s_transstart();                   //transmission start
 	switch(mode)
	{                    
   		case TEMP : error+=s_write_byte(MEASURE_TEMP); break;
   		case HUMI : error+=s_write_byte(MEASURE_HUMI); break;
  		default   : break;  
	}
 	 for (i=0;i<65535;i++)
  	if(dat==0) break; //wait until sensor has finished the measurement

 	 if(dat) error+=1; 
                 // or timeout (~2 sec.) is reached
  	*(p_value)=s_read_byte(ACK);    //read the first byte (MSB)
  	*(p_value+1)=s_read_byte(ACK);    //read the second byte (LSB)
  	*p_checksum =s_read_byte(noACK);  //read checksum
  	return error;
}
void calc_sth11(float *p_humidity ,float *p_temperature)
{ 
  const float C1=-4.0;              // for 12 Bit
  const float C2=+0.0405;           // for 12 Bit
  const float C3=-0.0000028;        // for 12 Bit
  const float T1=+0.01;             // for 14 Bit @ 5V
  const float T2=+0.00008;           // for 14 Bit @ 5V 

  float rh=*p_humidity;             // rh:      Humidity [Ticks] 12 Bit 
  float t=*p_temperature;           // t:       Temperature [Ticks] 14 Bit
  float rh_lin;                     // rh_lin:  Humidity linear
  float rh_true;                    // rh_true: Temperature compensated humidity
  float t_C;                        // t_C   :  Temperature [癈]

  t_C=t*0.01 - 40;                  //calc. temperature from ticks to [癈]
  rh_lin=C3*rh*rh + C2*rh + C1;     //calc. humidity from ticks to [%RH]
  rh_true=(t_C-25)*(T1+T2*rh)+rh_lin;   //calc. temperature compensated humidity [%RH]
  if(rh_true>100)rh_true=100.0;       //cut if the value is outside of
  if(rh_true<0.1)rh_true=0.1;       //the physical possible range

  *p_temperature=t_C;               //return temperature [癈]
  *p_humidity=rh_true;              //return humidity[%RH]	  
} 
//--------按键调节函数------------
scankey1()
{	
	if(key1==0)
	{	
		delay(2);
		if(key1==0)
	    { 	   			
		  k1++;		   				 
          while(!key1);
		  if(k1>=3)
		  k1=0;
		}
	}
}
adjustt()
{
	scankey1(); 
	while(k1!=0)
	 { 
	 	if(k1==1)
        { 
	   		if(key2==0)
		  	{	
				delay(2);
				if(key2==0)
				{ 	 
			    	wendu_h=wendu_h+5;
				    if(wendu_h>=999) wendu_h=999;
					savedata();
			   		while(!key2);	
				}
		  	}
		  	if(key3==0)
		  	{	
				delay(5);
				if(key3==0)
				{ 	 
			    	wendu_h=wendu_h-5;
				   	if(wendu_h<=0) wendu_h=0;
				   	savedata();
			   		while(!key3);
				}
		  	}
			getdata();
			t=wendu_h;
		display(12,t/100,(t%100)/10,t%10);
	    } 
	   if(k1==2)
	  	{  	
			if(key2==0)
			{	
				delay(5);
				if(key2==0)
				{ 	 
		    		wendu_l=wendu_l+5;
					if(wendu_l>=999)  wendu_l=999;
					savedata();
			   		while(!key2);
				}
			}
		if(key3==0)
		{	
			delay(5);
			if(key3==0)
			{ 	 
		    	wendu_l=wendu_l-5;
		      if(wendu_l<=0) 
			   	wendu_l=0;
		    	 savedata();
		   	    while(!key3);
			}
		}
		getdata();
		t=wendu_l;
		display(13,t/100,(t%100)/10,t%10);
	
		}
		scankey1();
	}
}
adjusth()
{
	scankey1(); 
	while(k1!=0)
	 { 
	 	if(k1==1)
       { 
		 if(key2==0)
		  {	
			delay(5);
			if(key2==0)
			{ 	 
		    	shidu_h=shidu_h+10;
			    if(shidu_h>=999)
				 shidu_h=999;
				savedata();
		   		while(!key2);

			}
		  }
		  if(key3==0)
		  {	
			delay(5);
			if(key3==0)
			{ 	 
		    	shidu_h=shidu_h-10;
			   	if(shidu_h<=0) shidu_h=0;
			   	savedata();
		   	while(!key3);
			}
		  }
		getdata();
		t=shidu_h;
		display(12,t/100,(t%100)/10,t%10);
	
		}
	scankey1();
	 if(k1==2)
	{  	
		if(key2==0)
		{	
			delay(5);
			if(key2==0)
			{ 	 
		    	shidu_l=shidu_l+10;
				if(shidu_l>=999)  shidu_l=999;
				savedata();
			   	while(!key2);
			}
		}
		if(key3==0)
		{	
			delay(5);
			if(key3==0)
			{ 	 
		    	shidu_l=shidu_l-10;
			if(shidu_l<=0)  shidu_l=0;
		     savedata();
		   	while(!key3);
			}
		}
		getdata();
		t=shidu_l;
		display(13,t/100,(t%100)/10,t%10);
	
		}
	}
}
//-----中断初始化----------------
void init()					
{	 
	 EA=1;
	 ET0=1;
	 TMOD=0x01;            //T1=16位外部计数器,T0=定时方式1
	 TH0=0x3C;
	 TL0=0xB0;            //12.0000MHz@50mS
     TR0=1;	 
	 k1=0;
	 k2=0;
	 k3=0;	 
}
//--------主函数-------------	 	
void main()
{ 
	value humi_val,temp_val;
	uchar error1,checksum; 
	init();
	getdata();
	s_connectionreset();
	while(1)
	{ 	
		TR0=0;	
  		error1=0;
    	error1+=s_measure((unsigned char*) &humi_val.i,&checksum,HUMI);  //measure humidity  
    	error1+=s_measure((unsigned char*) &temp_val.i,&checksum,TEMP); 
		 TR0=1;  
    	if(error1!=0) s_connectionreset();
    	else
    	{  		  
	 		humi_val.f=(float)humi_val.i;                   //converts integer to float
      		temp_val.f=(float)temp_val.i;                   //converts integer to float
      		calc_sth11(&humi_val.f,&temp_val.f);  	//calculate humidity, temperature  
		  	if(sl>=5)
	  		{
				t=temp_val.f*10; 			
				display1(t/100,(t%100)/10,t%10,10);
				adjustt();
				if(t>=wendu_h)				
					{CTR1=0;ming(10);}
				else
				{
					beep=1;
					CTR1=1;	
				}		
				if(t<=wendu_l)	
					{CTR2=0;ming(10);}
				else
				{
					beep=1;
					CTR2=1;	
				}
			}
			   
	   		else		  //湿度显示与调节，越限报警
		  	{  	  		  
		 	 	t=humi_val.f*10;	  
	 			display1(t/100,(t%100)/10,t%10,12);
				adjusth();
				if(t>=shidu_h)				
					{CTR3=0;ming(12);}
				else
				{
					beep=1;
					CTR3=1;	
				}			
				if(t<=shidu_l)	
					{CTR4=0;ming(12);}
				else
				{
					beep=1;
					CTR4=1;	
				}
			}
    	} 
   }
}
//-----中断函数----------------
void time() interrupt 1	using 1
{						  
	TH0=0x3C;
	TL0=0xB0;    //12.0000MHz@50mS
    msec++;        //50mS +1
	
    if (msec>=20)  //1秒闸门  
		{
			msec=0; 
			sl++;
			if (sl>=8) 
			sl=0;	   
		 }
}	        