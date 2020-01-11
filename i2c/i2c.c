#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>


#include "MLX90640_I2C_Driver.h"
#include "MLX90640_API.h"
#include <math.h>


//****************************************
// 定义内部地址
//****************************************
#define	SLAVEADDRESS		0x50 // 器件地址  mlx90640 == 0x33   测试EEP地址 0x50
#define TA_SHIFT 8 //the default shift for a MLX90640 device in open air


uint16_t eeMLX90640[832];
uint16_t mlx90640Frame[834];
paramsMLX90640 mlx90640;
float mlx90640To[768];





static float emissivity = 0.95;
static float tr;
static int status;

static unsigned short read_buf[127]={0x00};
static unsigned short cnt;
static int fd;


int i2c_init(void);
int i2c_test(void);



int main (int argv)
{
	int i=10;

	
    // if(i2c_init()<0)
	// {
		// return -1;
	// }
	
	//i2c_test();

	MLX90640_I2CInit();
	
	// status = MLX90640_DumpEE (SLAVEADDRESS, eeMLX90640);
	// printf("MLX90640_DumpEE status %d \n",status);
	// status = MLX90640_ExtractParameters(eeMLX90640, &mlx90640);
	// printf("MLX90640_ExtractParameters status %d \n",status);
	
    for(;;)
    {

		// status = MLX90640_GetFrameData (SLAVEADDRESS, mlx90640Frame);
		// printf("MLX90640_GetFrameData status %d \n",status);
		// tr = MLX90640_GetTa(mlx90640Frame, &mlx90640)-TA_SHIFT; //reflected temperature based on the sensor ambient temperature
		// printf("MLX90640_GetFrameData tr %d \n",tr);
		//MLX90640_CalculateTo(mlx90640Frame, &mlx90640, emissivity, tr, mlx90640To);//The object temperatures

		MLX90640_I2CWrite(SLAVEADDRESS, 0x00,cnt++);

		delay(10);
		
		printf("\n");
		MLX90640_I2CRead(SLAVEADDRESS,0x00, sizeof(read_buf)/2, read_buf);
		for(i=0;i<sizeof(read_buf)/2;)
		{
			printf("0x%04x ",read_buf[i]);
			i++;
			if((i%16)==0)
			{
				printf("\n");
			}
		}
		printf("\n");
		printf("run...\n");
		
		delay(1000);
    }
    return 0;
}



// 初始化 
int i2c_init(void)
{
    wiringPiSetup();
    fd = wiringPiI2CSetup (SLAVEADDRESS);
	
	if (fd >= 0)   // fd 为负数，说明IIC连接失败
    {
        printf("wiringPiI2CSetup ok fd = %d\n",fd);
    }
    else
    {
        printf("wiringPiI2CSetup error\n");
    }
	
	return fd;
}


int i2c_test(void)
{
	int i;
	#if 1
	for(i=0;i<16;i++)
	{
		wiringPiI2CWriteReg8(fd,i,i);
		delay(3);
	}
	

	for(i=0;i<16;i++)
	{
		printf("0x%02X ",wiringPiI2CReadReg8(fd,i));
		if(i%16==0)
		{
			printf("\n");
		}
	}
	printf("\n");
	
	delay(1000);
		
	for(i=0;i<8;i++)
	{
		wiringPiI2CWriteReg16(fd,i,i);
		delay(3);
	}
	
	for(i=0;i<8;i++)
	{
		printf("0x%04X ",wiringPiI2CReadReg16(fd,0x1200+i*2));
		if(i%16==0)
		{
			printf("\n");
		}
	}
	printf("\n");
	#endif
	
}





