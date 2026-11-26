#include "STC15F2K60S2.H"
#include "seg.h"
#include "key.h"
#include "led.h"
#include "onewire.h"
#include "ds1302.h"
#include "ultrasound.h"
#include "iic.h"
#include "uart.h"
#include <stdio.h>
#include <string.h>

unsigned char ucrtc[3] = {23,59,50};
unsigned char distance;

unsigned int temperature_100x;

/*数码管变量区*/
unsigned char seg_buf[] = {16,16,16,16,16,16,16,16};
unsigned char seg_pos;
unsigned char seg_slow_down;

/*按键变量区*/
unsigned char key_read();
unsigned char key_slow_down; //按键处理函数减速变量
unsigned char key_old; //储存上一次键值
unsigned char key_up; //捕获上升沿
unsigned char key_down; //捕获下降沿
unsigned char key_val; //储存键值

/*LED变量区*/
unsigned char led_buf[] = {0,0,0,0,0,0,0,0};

/*算法变量区*/
unsigned char num;
unsigned int time_200ms;
unsigned int freq;

unsigned int ad1_100x;
unsigned int ad3_100x;
unsigned int da_out_100x;

unsigned char eeprom_write_buf[] = {1,2,3,4,5,6,7,8};
unsigned char eeprom_read_buf[] = {3,3,3,3,3,3,3,3};
/*继电器 蜂鸣器及电机*/
unsigned char rbm_buf[]={1,1,1};

/*串口*/
unsigned char idata uart_rec_buf[20] = {0};
unsigned char uart_rec_index;
unsigned char uart_tick;
unsigned char led_num;

//按键处理函数
void key_proc(){
	if(key_slow_down < 20){
		return;
	}
	key_slow_down = 0;
		//^异或符号，相同为0
	key_val = key_read();
	key_down = key_val &   (key_val^key_old);
	key_up = ~ key_val &   (key_val^key_old);
	key_old = key_val;
	
}

//数码管处理函数
void seg_proc(){
	if(seg_slow_down < 100){
		return;
	}
	seg_slow_down = 0;
	
	/*数据获取区*/
	//temperature_100x = temperature_read()*100;
	//distance = ultrasound_read();
	//rtc_read(ucrtc);
	
	/*读取电压*/
	
	//读取两遍防止错乱（光敏）
	ad_read(0x41) / 51.0f * 100; //0x41 光敏 0x43 电位器
	ad1_100x = ad_read(0x41) / 51.0f * 100; //0x41 光敏 0x43 电位器
	
	//读取两遍防止错乱（电位器）
	ad_read(0x43) / 51.0f * 100; //0x41 光敏 0x43 电位器
	ad3_100x = ad_read(0x43) / 51.0f * 100; //0x41 光敏 0x43	电位器
	
	/*输出电压*/
	da_out_100x = 200;
	da_write(da_out_100x / 100.0f * 51 + 0.5f);
	
	/*数据显示区*/
//	seg_buf[0] = temperature_100x / 1000 % 10;
//	seg_buf[1] = temperature_100x / 100 % 10 + ',';
//	seg_buf[2] = temperature_100x / 10 % 10;
//	seg_buf[3] = temperature_100x % 10;
	
//	seg_buf[0] = ucrtc[0] / 10;
//	seg_buf[1] = ucrtc[0] % 10;
//	
//	seg_buf[3] = ucrtc[1] / 10;
//	seg_buf[4] = ucrtc[1] % 10;
//	
//	seg_buf[6] = ucrtc[2] / 10;
//	seg_buf[7] = ucrtc[2] % 10;
//	seg_buf[0] = distance / 100 % 10;
//	seg_buf[1] = distance / 10 % 10;
//	seg_buf[2] = distance % 10;

//		seg_buf[0] = ad3_100x / 100 % 10 + ',';
//		seg_buf[1] = ad3_100x / 10 % 10;
//		seg_buf[2] = ad3_100x % 10;
//		
//		seg_buf[5] = ad1_100x / 100 % 10 + ',';
//		seg_buf[6] = ad1_100x / 10 % 10;
//		seg_buf[7] = ad1_100x % 10;
		seg_buf[0] = eeprom_read_buf[0];
		seg_buf[1] = eeprom_read_buf[1];
		seg_buf[2] = eeprom_read_buf[2];
		seg_buf[3] = eeprom_read_buf[3];
		seg_buf[4] = eeprom_read_buf[4];
		seg_buf[5] = eeprom_read_buf[5];
		seg_buf[6] = eeprom_read_buf[6];
		seg_buf[7] = eeprom_read_buf[7];
}

//中断服务函数（定时器1）
void Timer1_Isr(void) interrupt 3
{
	key_slow_down++; //10毫秒（按键）
	seg_slow_down++; //100毫秒（数码管）
	uart_tick++;
	
	/* NE555 代码
	if(++time_200ms == 200){
		time_200ms = 0;
		freq = TH0 << 8 | TL0;
		freq *= 5;
		TH0 = TL0 = 0;
	}
	*/
	
	//显示seg_buf
	if(++seg_pos == 8){
		seg_pos = 0;
	}
	if(seg_buf[seg_pos] >= ','){
		seg_display(seg_pos,seg_buf[seg_pos] - ',',1);
	}
	else{
		seg_display(seg_pos,seg_buf[seg_pos],0);
	}
	
	//显示LED灯
	led_display(led_buf);
	//继电器，蜂鸣器及电机
	relay_beep_motor(rbm_buf);
}

void Timer1_Init(void)		//1毫秒@12.000MHz
{
	AUXR &= 0xBF;			//定时器时钟12T模式
	TMOD &= 0x0F;			//设置定时器模式
	TL1 = 0x18;				//设置定时初始值
	TH1 = 0xFC;				//设置定时初始值
	TF1 = 0;				//清除TF1标志
	TR1 = 1;				//定时器1开始计时
	ET1 = 1;				//使能定时器1中断
	EA = 1;  //手动添加 EA=1 开启总中断
}

//定时器0（用于NE555）
void Timer0_Init(void)		//100微秒@12.000MHz
{
	AUXR &= 0x7F;			//定时器时钟12T模式
	TMOD &= 0xF0;			//设置定时器模式
	TMOD |= 0x01;			//设置定时器模式
	TMOD |= 0x04;     //配置定时器0的时钟原是外部P34引脚
	TL0 = 0;				//设置定时初始值
	TH0 = 0;				//设置定时初始值
	TF0 = 0;				//清除TF0标志
	TR0 = 1;				//定时器0开始计时
}

void uart_proc(){
	if(uart_rec_index == 0){
		return;
	}
	if(uart_tick < 10){
		return;
	}
	
	printf("%s",uart_rec_buf);
	sscanf(uart_rec_buf,"led:%bu",&led_num);
	
	led_buf[led_num] = 1;
	memset(uart_rec_buf,0,20);
	uart_rec_index = 0;
}

//串口中断，分为串口接收中断及发射中断
void uart_irs() interrupt 4{ //手动写一下串口的中断函数
	//RI = 1 时是接收的  TI = 1 时是发送的
//	if(RI == 1){ //如果是接收中断
//		unsigned char num = 0x00;
//		num = SBUF;
//		num = num + 1;
//		SBUF = num; 
//		while(TI == 0);
//		TI = 0;
//		RI = 0;
//	}
	if(RI == 1){
		uart_tick = 0;
		uart_rec_buf[uart_rec_index] = SBUF;
		uart_rec_index++;
		
		RI = 0;
	}
}

void main(){
	Timer1_Init(); //定时器1初始化
	Timer0_Init(); //定时器0初始化
	Uart1_Init(); //初始化串口1
	EA = 0;
	eeprom_write(eeprom_write_buf,0x18,8);
		eeprom_write(eeprom_write_buf,0x21,1);
	//rtc_set(ucrtc); //设置DS1302初始时间
	eeprom_read(eeprom_read_buf,0x18,8);
	EA = 1;
	
	printf("hello world:%bu",seg_buf[7]);
	while(1){
		key_proc();
		seg_proc();
		uart_proc();
	}
}