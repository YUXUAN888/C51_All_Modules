#include "STC15F2K60S2.H"
#include <intrins.h>

sbit us_tx = P1^0;
sbit us_rx = P1^1;

void Delay10us(void)	//@12.000MHz
{
	unsigned char data i;

	_nop_();
	_nop_();
	i = 27;
	while (--i);
}

unsigned char ultrasound_read(){
	unsigned int time = 0x00;
	CMOD = 0x00;
	
	CH = CL = 0;
	
	us_tx = 1;
	Delay10us();
	us_tx = 0;
	
	EA = 0;
	
	CR = 1; //PCA开始计时
	while(CF == 0 && us_rx == 1); //没有溢出且没收到回波
	CR = 0;
	
	EA = 1;
	
	if(CF == 0){ //没有溢出 收到回波
		time = CH << 8 | CL;
		return time * 0.017;
	}else{ //溢出
		CF = 0;
		return 0;
	}
}