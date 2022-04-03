#define uchar unsigned char 
#define uint unsigned int
uint wendu_h=30,wendu_l=14;
uint shidu_h=78,shidu_l=12;
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
	addr++;writebyte(addr,wendu_h/256);
	addr++;writebyte(addr,wendu_h%256);
	addr++;writebyte(addr,wendu_l/256);
	addr++;writebyte(addr,wendu_l%256);
	addr++;writebyte(addr,shidu_h/256);
	addr++;writebyte(addr,shidu_h%256);
	addr++;writebyte(addr,shidu_l/256);
	addr++;writebyte(addr,shidu_l%256);
}
void getdata(void)
{
	uchar label=0,t0,t1;
	uint addr=0x2000;
	label=readbyte(addr);
	if(label==123)
	{
	 addr++;t1=readbyte(addr);
	 addr++;t0=readbyte(addr);
	 wendu_h=t1*256+t0;
	 addr++;t1=readbyte(addr);
	 addr++;t0=readbyte(addr);
	 wendu_l=t1*256+t0;
	 addr++;t1=readbyte(addr);
	 addr++;t0=readbyte(addr);
	 shidu_h=t1*256+t0;
	 addr++;t1=readbyte(addr);
	  addr++;t0=readbyte(addr);
	 shidu_l=t1*256+t0;
	 }
	else
		savedata();
}