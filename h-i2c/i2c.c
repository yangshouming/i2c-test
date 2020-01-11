/******************************************************************************\
**  版    权 :  深圳市和而泰智能控制股份有限公司所有（2020）
**  文件名称 :  i2c.h
**  功能描述 :  迈来芯红外热传感器I2C驱动
**  作    者 :  杨守明
**  日    期 :  2020-1-10
**  版    本 :  V0.0.1
**  变更记录 :  V0.0.1/2020-1-10
                1.首次创建
\******************************************************************************/

/******************************************************************************\
                                 Includes
\******************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

#include "mywiringPiI2C.h"


/******************************************************************************\
                             Variables definitions
\******************************************************************************/
#define	SLAVEADDRESS		0x50 // 器件地址  mlx90640=0x33   测试EEP=0x50

int fd;
int cnt;

uint16_t read_buf[64]={0x00};

/******************************************************************************\
                             Functions definitions
\******************************************************************************/

static void printf_dump_uint8_t(uint8_t *pdata,int length);
static void printf_dump_uint16_t(uint16_t *pdata,int length);

int main (int argv)
{
	int i;
	
	#if 0
	wiringPiSetup();
	
	fd = wiringPiI2CSetup(SLAVEADDRESS);  
	
	if (fd >= 0)
	{ // fd 为负数，说明IIC连接失败
        printf("fd = %d\n",fd);
    }
    else 
	{
        printf("IIC error \n");
    }
	
    #else 
    
	wiringPiSetup();
	
	fd = mywiringPiI2CSetup(SLAVEADDRESS);  
	
	if (fd >= 0)
	{ // fd 为负数，说明IIC连接失败
        printf("fd = %d\n",fd);
    }
    else 
	{
        printf("IIC error \n");
    }
	
	#endif
    
	/*
    //int MLX90640_I2CRead(uint8_t slaveAddr, uint16_t startAddress,uint16_t nMemAddressRead, uint16_t *data)
		
    //int MLX90640_I2CWrite(uint8_t slaveAddr, uint16_t writeAddress, uint16_t data)//写入2byte地址，2byte数据

	*/

    for(;;)
    {
		
		cnt++;
		
        #if 0
		for(i=0;i<8;i++)
		{
			wiringPiI2CWriteReg8(fd,i,cnt); 
			delay(5);
		}
		for(i=0;i<8;i++)
		{
			read_buf[i] = wiringPiI2CReadReg8(fd,i);
			delay(5);
		}
		printf_dump_uint8_t( (uint8_t *) read_buf,8);
        #endif



        #if 1


        //mywiringPiI2CWriteReg16(fd,0x1234,0x5678); 
        delay(50);
        //mywiringPiI2CWriteReg16_mlx(fd,0x1234,0x5678);

        mywiringPiI2CReadReg16_mlx(fd,0x1234,read_buf,2);
        

//        for(i=0;i<8;i++)
//		{
//			//wiringPiI2CWriteReg16(fd,i*2,cnt); 
//            mywiringPiI2CWriteReg16_mlx(fd,i*2,cnt);
//			delay(5);
//		}
	  
//		for(i=0;i<8;i++)
//		{
//			read_buf[i] = wiringPiI2CReadReg16(fd,i*2);
//			delay(5);
//		}
//		printf_dump_uint16_t( read_buf,8);

        #endif
		
		
		printf("i2c test run...\n");

		delay(1000);

    }
    return 0;
}


//打印数组，调试用
static void printf_dump_uint16_t(uint16_t *pdata,int length)
{
	int i;
	
	for(i=0;i<length;)
	{
		printf("0x%04x ",pdata[i]);
		i++;
		if((i%16)==0)
		{
			printf("\n");
		}
	}
	printf("\n");
}

//打印数组，调试用
static void printf_dump_uint8_t(uint8_t *pdata,int length)
{
	int i;
	
	for(i=0;i<length;)
	{
		printf("0x%02x ",pdata[i]);
		i++;
		if((i%16)==0)
		{
			printf("\n");
		}
	}
	printf("\n");
}

