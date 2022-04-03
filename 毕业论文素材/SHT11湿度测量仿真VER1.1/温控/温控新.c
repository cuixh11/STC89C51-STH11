#include <reg51.h>
#include <intrins.h>
#include <math.h>

#define	uchar unsigned char
#define uint unsigned int

sbit dat=P1^0;   //串行数据
sbit clk0=P1^1;   //串行时钟
sbit lat=P1^2;

sbit key1=P0^0;
sbit key2=P0^1;
sbit key3=P0^2;

sbit led1=P0^6;
sbit led2=P0^7;

sbit  gw=P1^3;   //个位
sbit  sw=P1^4;    //十位
sbit  bw=P3^3;    //百位
sbit  qw=P3^4;

sbit pl=P3^6;         //165锁存
sbit clk1=P3^7;        //165时钟
sbit Do=P3^5;

sbit DQ=P3^1;
code uchar  tab[16]={  
                0x3F,  
				0x06,
				0x5B,
				0x4F,    
  				0x66,
				0x6D,
				0x7D,
				0x07,    
  				0x7F,
				0x6F,  
                0x39,  //"C"
                0x79,  //"E"
                0x76,  //"H"
                0x38,  //"L"
                0x40  //"-"
};
uchar k1,k2,k3;
uint t;
uchar disbuf[4];
uint wendu_h=456,wendu_l=288;

///eeprom address
sfr wdtcontr=0xe1;
sfr ispdata=0xe2;
sfr ispaddrh=0xe3;
sfr ispaddrl=0xe4;
sfr ispcmd=0xe5;
sfr isptrig=0xe6;
sfr ispcontr=0xe7;
////////////////////////
iapdisable()
{
	ispcontr=0;ispcmd=0;isptrig=0;
}
erase(uint addr)
{
	ispcontr=0x83;ispcmd=0x03;
	ispaddrh=addr/256;ispaddrl=addr%256;
	EA=0;isptrig=0x46;isptrig=0xb9;_nop_();EA=1;
	iapdisable();
}
uchar readbyte(uint addr)
{
	uchar ch;
	ispcontr=0x83;ispcmd=0x01;ispaddrh=addr/256;ispaddrl=addr%256;
	isptrig=0x46;isptrig=0xb9;_nop_();ch=ispdata;
	iapdisable();
	return ch;
}
void writebyte(uint addr,uchar ch)
{
	ispcontr=0x83;ispcmd=0x02;
	ispaddrh=addr/256;ispaddrl=addr%256;
	ispdata=ch;
	isptrig=0x46;isptrig=0xb9;_nop_();
	iapdisable();
}
savedata()
{
	uint addr=0x2000;
	erase(addr);
	writebyte(addr,123);
	addr++;writebyte(addr,wendu_h);
	addr++;writebyte(addr,wendu_l);
}
void getdata(void)
{
	uchar label=0;
	uint addr=0x2000;
	label=readbyte(addr);
	if(label==123)
	{
	 addr++;wendu_h=readbyte(addr);
	 addr++;wendu_l=readbyte(addr);
	 }
	else
		savedata();
}

ReadDat()
{
 unsigned char i,t;
 clk1=0;
 pl=0;
 pl=1;      
 for(i=0;i<8;i++)
  {
   t<<=1;       //165在PL有一个脉冲后就输出0位数据
   t|=(bit)Do;
   clk1=1;   // 锁存时钟=1时74HC165才允许读数据
   clk1=0;
  }
 P0=t;
}
dsout(unsigned char j)
{
 unsigned char i; 
 for(i=0;i<8;i++)
    {
     dat=j&0x80;       //高位在前
     clk0=1;
     clk0=0;
     j<<=1;
    }
  lat=1;
  lat=0;               //锁存
}
void delay(uint z)
{
	uint x;
	for(x=z;x>0;x--);
}
ds1ms()
{
 unsigned int d;
 for(d=0;d<250;d++); //大约1ms@12M
} 
//------送出循环-------------------------

//----74HC595显示程序--------------------
display(unsigned char sjqw,sjbw,sjsw,sjgw)
{ 

  dsout(tab[sjgw]);gw=0;ds1ms();gw=1;        //查表得到数据段码送显示
  dsout(tab[sjsw]);sw=0;ds1ms();sw=1; 
  dsout(tab[sjbw]);bw=0;ds1ms();bw=1;
  dsout(tab[sjqw]);qw=0;ds1ms();qw=1; 
  
}
Init_DS18B20(void)
{
	 unsigned char x=0;
	 DQ = 1;    //DQ复位
	 delay(12);  //稍做延时
	 DQ = 0;    //单片机将DQ拉低
	 delay(150); //精确延时 大于 480us
	 DQ = 1;    //拉高总线
	 delay(60);
	 x=DQ;      //稍做延时后 如果x=0则初始化成功 x=1则初始化失败
	 delay(30);
}
//读一个字节
uchar ReadOneChar(void)
{
	unsigned char i=0;
	unsigned char dat = 0;
	for (i=8;i>0;i--)
	 {
		  DQ = 0; // 给脉冲信号
		  dat>>=1;
		  _nop_();_nop_();_nop_();_nop_();_nop_();_nop_();
		  DQ = 1; // 给脉冲信号
		  _nop_();_nop_();_nop_();_nop_();_nop_();_nop_();
		  if(DQ)
		  dat|=0x80;
		  delay(12);
	 }
	 return(dat);
}
//写一个字节
WriteOneChar(unsigned char dat)
{
	 unsigned char i=0;
	 for (i=8; i>0; i--)
	 {
		  DQ = 0;
		  _nop_();_nop_();_nop_();_nop_();_nop_();_nop_();
		  DQ = dat&0x01;
		  delay(8);
		  DQ = 1;
		  dat>>=1;
		  _nop_();_nop_();_nop_();_nop_();_nop_();_nop_();
	 }
}
adt(uchar x)
{
	Init_DS18B20();
	WriteOneChar(0xCC); // 跳过读序号列号的操作
	WriteOneChar(x); // 启动温度转换
	_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();
	_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();
}
//读取温度
unsigned char a=0;
unsigned char b=0;
zhuand()
{	
	disbuf[0]=0;	    	
	disbuf[1]=t/100;
	disbuf[2]=(t%100)/10;
	disbuf[3]=t%10;

}
void zhuan()	 //温度转换将单片机里二进制温度转换成十进制
{
	uint tt=0;
 	t=b;//将温度高八位传入t
	t<<=8;
	t=t|a;//将温度低八位传入t
	if (b>0x07)
	{
		t=~t;tt=((t+1)*5)/8;t= tt; //放大10倍输出并四舍五入
		if(t>=99) t=99;
		zhuand();
		disbuf[0]=14;
		
	}
	else{
		    tt=t;tt=(t*5)/8;t= tt; //放大10倍输出并四舍五入
			if(t>=999) t=999;
			if(t>=wendu_h) led1=0;
			if(t<=wendu_l) led2=0;
			zhuand();
		}
}
scankey1()
{	
	ReadDat();
	if(key1==0)
	{	
		delay(5);
		if(key1==0)
	   { 
		    k1++;
          while(!key1)
     	{	
 	    	ReadDat();
		}
		}
	}
}
adjust()
{	scankey1(); 
	while(k1!=0)
	 { if(k1==1)
       { 
	  	  ReadDat();
		 if(key2==0)
		  {	
			delay(5);
			if(key2==0)
			{ 	 
		    	wendu_h++;
			    if(wendu_h>=999) wendu_h=999;
				savedata();
		   	while(!key2)
 			{	
 				ReadDat();
			}
			}
		  }
		  if(key3==0)
		  {	
			delay(5);
			if(key3==0)
			{ 	 
		    	wendu_h--;
			   if(wendu_h<=0) wendu_h=0;
			   savedata();
		   	while(!key3)
 			{	
 				ReadDat();
			}
			}
		  }
		getdata();
		t=wendu_h;
        zhuand();
		disbuf[0]=12;
		display(disbuf[0],disbuf[1],disbuf[2],disbuf[3]);
	
		}
	scankey1();
	 if(k1==2)
	{  	
		ReadDat();
		if(key2==0)
		{	
			delay(5);
			if(key2==0)
			{ 	 
		    	wendu_l++;
			if(wendu_l>=999)  wendu_l=999;
			savedata();
		   	while(!key2)
 			{	
 				ReadDat();
			}
			}
		}
		if(key3==0)
		{	
			delay(5);
			if(key3==0)
			{ 	 
		    	wendu_l--;
			if(wendu_l<=0)  wendu_l=0;
		     savedata();
		   	while(!key3)
 			{	
 				ReadDat();
			}
			}
		}
		getdata();
		t=wendu_l;
        zhuand();
		disbuf[0]=13; 
		display(disbuf[0],disbuf[1],disbuf[2],disbuf[3]);
	
		}
   scankey1();
	if(k1>=3)
	    k1=0;
	}
} 
void main()
{	
	getdata();
	EA=0;adt(0x44);adt(0xbe);a=ReadOneChar();b=ReadOneChar();EA=1;
	EA=0;adt(0x44);adt(0xbe);a=ReadOneChar();b=ReadOneChar();EA=1;
	EA=0;adt(0x44);adt(0xbe);a=ReadOneChar();b=ReadOneChar();EA=1;
	EA=0;adt(0x44);adt(0xbe);a=ReadOneChar();b=ReadOneChar();EA=1;
	while(1)
     {
	   EA=0;adt(0x44);adt(0xbe);a=ReadOneChar();b=ReadOneChar();EA=1;
	   zhuan();
	   display(disbuf[0],disbuf[1],disbuf[2],disbuf[3]);
	   adjust();  	  	 

	 }
}



