#include "STC15F2K60S2.H"

//按键返回函数（返回按下的按键编号）
unsigned char key_read()
{
	unsigned char temp = 0;
	P44 = 0;P42=1;P35=1;; //第一列
	if(P33 == 0){
		temp = 4;
	}	
	if(P32 == 0){
		temp = 5;
	}	
	if(P31 == 0){
		temp = 6;
	}	
	if(P30 == 0){
		temp = 7;
	}
	P44 = 1;P42=0;P35=1;; //第二列
	if(P33 == 0){
		temp = 8;
	}	
	if(P32 == 0){
		temp = 9;
	}	
	if(P31 == 0){
		temp = 10;
	}	
	if(P30 == 0){
		temp = 11;
	}
	P44 = 1;P42=1;P35=0;; //第三列
	if(P33 == 0){
		temp = 12;
	}	
	if(P32 == 0){
		temp = 13;
	}	
	if(P31 == 0){
		temp = 14;
	}	
	if(P30 == 0){
		temp = 15;
	}
	P44 = 1;P42=1;P35=1;; //第四列
	if(P33 == 0){
		temp = 16;
	}	
	if(P32 == 0){
		temp = 17;
	}	
	if(P31 == 0){
		temp = 18;
	}	
	if(P30 == 0){
		temp = 19;
	}
	return temp;
}