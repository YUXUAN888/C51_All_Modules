#include "STC15F2K60S2.H"

void led_display(unsigned char* ucled){ //1亮0灭
	unsigned char temp;
	temp = //把这 8 个数组元素，合成为一个二进制数
	(ucled[0] << 0)|
	(ucled[1] << 1)|
	(ucled[2] << 2)|
	(ucled[3] << 3)|
	(ucled[4] << 4)|
	(ucled[5] << 5)|
	(ucled[6] << 6)|
	(ucled[7] << 7)
	;
	P0 = ~temp; //LED需要取反
	P2 = P2 & 0x1F | 0x80;
	P2 &= 0x1F;
}

void relay_beep_motor(unsigned char* rbm_buf){ //rbm_buf 第0，1，2位依次为继电器，蜂鸣器及电机
	unsigned char temp;
	temp = 
	(rbm_buf[0]<<4)|
	(rbm_buf[1]<<6)|
	(rbm_buf[2]<<5)
	;
	P0 = temp;
	P2 = P2 & 0x1F | 0xA0;
	P2 &= 0x1F;
}