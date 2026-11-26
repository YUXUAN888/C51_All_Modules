/*	#   I2C代码片段说明
	1. 	本文件夹中提供的驱动代码供参赛选手完成程序设计参考。
	2. 	参赛选手可以自行编写相关代码或以该代码为基础，根据所选单片机类型、运行速度和试题
		中对单片机时钟频率的要求，进行代码调试和修改。
*/

#include "STC15F2K60S2.H"
#include <intrins.h>

#define DELAY_TIME	10

sbit sda = P2^1;
sbit scl = P2^0;

//
static void I2C_Delay(unsigned char n)
{
    do
    {
        _nop_();_nop_();_nop_();_nop_();_nop_();
        _nop_();_nop_();_nop_();_nop_();_nop_();
        _nop_();_nop_();_nop_();_nop_();_nop_();		
    }
    while(n--);      	
}

//
void I2CStart(void)
{
    sda = 1;
    scl = 1;
	I2C_Delay(DELAY_TIME);
    sda = 0;
	I2C_Delay(DELAY_TIME);
    scl = 0;    
}

//
void I2CStop(void)
{
    sda = 0;
    scl = 1;
	I2C_Delay(DELAY_TIME);
    sda = 1;
	I2C_Delay(DELAY_TIME);
}

//
void I2CSendByte(unsigned char byt)
{
    unsigned char i;
	
    for(i=0; i<8; i++){
        scl = 0;
		I2C_Delay(DELAY_TIME);
        if(byt & 0x80){
            sda = 1;
        }
        else{
            sda = 0;
        }
		I2C_Delay(DELAY_TIME);
        scl = 1;
        byt <<= 1;
		I2C_Delay(DELAY_TIME);
    }
	
    scl = 0;  
}

//
unsigned char I2CReceiveByte(void)
{
	unsigned char da;
	unsigned char i;
	for(i=0;i<8;i++){   
		scl = 1;
		I2C_Delay(DELAY_TIME);
		da <<= 1;
		if(sda) 
			da |= 0x01;
		scl = 0;
		I2C_Delay(DELAY_TIME);
	}
	return da;    
}

//
unsigned char I2CWaitAck(void)
{
	unsigned char ackbit;
	
    scl = 1;
	I2C_Delay(DELAY_TIME);
    ackbit = sda; 
    scl = 0;
	I2C_Delay(DELAY_TIME);
	
	return ackbit;
}

//
void I2CSendAck(unsigned char ackbit)
{
    scl = 0;
    sda = ackbit; 
	I2C_Delay(DELAY_TIME);
    scl = 1;
	I2C_Delay(DELAY_TIME);
    scl = 0; 
	sda = 1;
	I2C_Delay(DELAY_TIME);
}

//artificial
unsigned char ad_read(unsigned char addr){
	unsigned char temp;
	
	I2CStart(); 
	I2CSendByte(0x90); //告诉PCF8591 要写点东西
	I2CWaitAck(); //等待回复
	I2CSendByte(addr); //发送要读的通道
	I2CWaitAck(); //等待回复
	
	I2CStart();
	I2CSendByte(0x91); //告诉PCF8591 要读点东西
	I2CWaitAck(); //等待回复
	temp = I2CReceiveByte(); //本次读取直接读取PCF8591发送的电压
	I2CSendAck(1); //读完了 不读了
	I2CStop(); 
	
	return temp;
}

void da_write(unsigned char dat){
	I2CStart();
	I2CSendByte(0x90); //告诉PCF8591 要写点东西
	I2CWaitAck(); //等待回复
	I2CSendByte(0x41); //把电压输出打开
	I2CWaitAck(); //等待回复
	I2CSendByte(dat); //按这个数输出电压
	I2CWaitAck(); //等待回复
	I2CStop();
}

void eeprom_write(unsigned char* str , unsigned char addr,unsigned char len){
	I2CStart();
	I2CSendByte(0xA0); //告诉AT24C02 要写点东西
	I2CWaitAck(); //等待回复
	I2CSendByte(addr); //从addr开始写
	I2CWaitAck(); //等待回复
	while(len--){
		I2CSendByte(*str++);
		I2CWaitAck(); //等待回复
	}
	I2CStop();
	I2C_Delay(255);
	I2C_Delay(255);
	I2C_Delay(255);
	I2C_Delay(255);
	I2C_Delay(255);
	I2C_Delay(255);
	I2C_Delay(255);
	I2C_Delay(255);
	I2C_Delay(255);
	I2C_Delay(255);
	I2C_Delay(255);
}

void eeprom_read(unsigned char* str , unsigned char addr,unsigned char len){
	I2CStart();
	I2CSendByte(0xA0); //告诉AT24C02 要读点东西
	I2CWaitAck(); //等待回复
	I2CSendByte(addr); //告诉它从这个地址读
	I2CWaitAck(); //等待回复
	
	I2CStart();
	I2CSendByte(0xA1); //我来读
	I2CWaitAck();
	while(len--){
		*str++ = I2CReceiveByte();
		if(len){
			I2CSendAck(0); //没读完
		}else{
			I2CSendAck(1); //读完了 不读了
		}
	}
	I2CStop();
}