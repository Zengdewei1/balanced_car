#ifndef __MPUIIC_H
#define __MPUIIC_H

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板V3
//MPU6050 IIC驱动 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/1/17
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////

#include <pic16f18854.h>
#include <stdint.h>

//IO方向设置
#define MPU_SDA_IN()    TRISCbits.TRISC4 = 1;
#define MPU_SDA_OUT()   TRISCbits.TRISC4 = 0;

//IO操作函数	 
#define MPU_IIC_SCL    LATCbits.LATC3 		//SCL
#define MPU_IIC_SDA    LATCbits.LATC4 		//SDA	 
#define MPU_READ_SDA   PORTCbits.RC4 		//输入SDA 

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

//IIC所有操作函数
u8 MPU_IIC_Delay(void); //MPU IIC延时函数
void MPU_IIC_Init(void); //初始化IIC的IO口				 
void MPU_IIC_Start(); //发送IIC开始信号
void MPU_IIC_Stop(); //发送IIC停止信号
void MPU_IIC_Send_Byte(u8 txd); //IIC发送一个字节
u8 MPU_IIC_Read_Byte(unsigned char ack); //IIC读取一个字节
u8 MPU_IIC_Wait_Ack(); //IIC等待ACK信号
void MPU_IIC_Ack(); //IIC发送ACK信号
void MPU_IIC_NAck(); //IIC不发送ACK信号

#endif
















