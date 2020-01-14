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
#include <wiringPi.h>
#include <wiringPiI2C.h>

#include "MLX90640_I2C_Driver.h"
#include "MLX90640_API.h"
#include <math.h>
#include <stdlib.h>

#include "i2c.h"

/******************************************************************************\
                             Variables definitions
\******************************************************************************/
#define SLAVEADDRESS 0x33 // 器件地址  mlx90640=0x33   测试EEP=0x50

static unsigned long run_cnt;			  //运行次数统计
static unsigned long get_source_data_cnt; //运行次数统计
static unsigned long get_calc_data_cnt;   //运行次数统计
static float get_source_data_percent;	 //
static float get_calc_data_percent;		  //

static uint16_t eeMLX90640[832];	//EEPROM数据
static paramsMLX90640 mlx90640;		//参数
static uint16_t mlx90640Frame[834]; //原始图像数据
static float mlx90640To[768];		//计算后的图像数据

static float emissivity; //发射系数
static float tr;		 //环境温度
static int status;		 //API执行状态
static int sensor_type;  //设备类型 0-90640 1-90641
static int ta_shift;	 //温度偏移值 8-MLX90640  5-MLX90641

static uint16_t mlx90640Frame_app[834]; //原始图像数据   APP测试数据
static float mlx90640To_app[768];		//计算后的图像数据		APP测试数据

/******************************************************************************\
                             Functions definitions
\******************************************************************************/

static void printf_dump_uint8_t(uint8_t *pdata, int length);
static void printf_dump_uint16_t(uint16_t *pdata, int length);
//计算百分比
float get_data_correct_percent(int data_target, int data_source);

int main(int argc, char **argv)
{
	int i;

	mlx_init();

#if 0
	if (argv[1] == NULL)
	{
		printf("enter number\n");
		return -1;
	}
	i = atoi(argv[1]);
	printf("i %d\n", i);

	MLX90640_I2CInit();
	MLX90640_I2CRead(SLAVEADDRESS, 0x2400, i, eeMLX90640);
	printf_dump_uint16_t(eeMLX90640, i);
	// MLX90640_I2CWrite(SLAVEADDRESS, 0x8000, 0x1234);
#endif

	printf("MLX90640_SetRefreshRate 0x%x\n", MLX90640_GetRefreshRate(SLAVEADDRESS));

	for (;;)
	{

#if 1
		if (mlx_get_source_data(mlx90640Frame_app) == 0)
		{
			get_source_data_cnt++;
		}

		if (mlx_get_calculate_data(mlx90640To_app, mlx90640Frame_app) == 0)
		{
			get_calc_data_cnt++;
		}

		// printf("mlx_get_calculate_data\n");
		// for (i = 0; i < 768;)
		// {
		// 	printf("%04.1f ", mlx90640To_app[i]);
		// 	i++;
		// 	if ((i % 32) == 0)
		// 	{
		// 		printf("\n");
		// 	}
		// }

		//成功率数据统计
		run_cnt++;

#endif
		if (run_cnt >= 1000)
		{

			get_source_data_percent = get_data_correct_percent(get_source_data_cnt, run_cnt);
			get_calc_data_percent = get_data_correct_percent(get_calc_data_cnt, run_cnt);
			printf("get_source %d %f;get_calc_data %d %f;run_cnt %d;      \n", get_source_data_cnt, get_source_data_percent, get_calc_data_cnt, get_calc_data_percent, run_cnt);
			break;
		}

		delay(8);
	}
	printf("break exit\n");
	return 0;
}

//打印数组，调试用
static void printf_dump_uint16_t(uint16_t *pdata, int length)
{
	int i;

	for (i = 0; i < length;)
	{
		printf("0x%04x ", pdata[i]);
		i++;
		if ((i % 16) == 0)
		{
			printf("\n");
		}
	}
	printf("\n");
}

//打印数组，调试用
static void printf_dump_uint8_t(uint8_t *pdata, int length)
{
	int i;

	for (i = 0; i < length;)
	{
		printf("0x%02x ", pdata[i]);
		i++;
		if ((i % 16) == 0)
		{
			printf("\n");
		}
	}
	printf("\n");
}

//计算百分比
float get_data_correct_percent(int data_target, int data_source)
{
	float sdata_target = data_target;
	float sdata_source = data_source;
	return (sdata_target / sdata_source) * 100;
}
/******************************************************************************/
/*
* 函数名称 : mlx_init
* 功能描述 : 传感器初始化
* 参    数 : 无
* 返回值   : @-1	错误
			 @0		传感器型号 90640 (24*32)
			 @1		传感器型号 90641 (12*16)  
* 示    例 : status = mlx_init();
*/
/******************************************************************************/
int mlx_init(void)
{
	int ret = 0;
	int i = 0;

	//硬件初始化
	ret = MLX90640_I2CInit();

	if (ret)
	{
		printf("mlx_init error\n");
		return -1;
	}

	delay(500); //操作延时
	//EEP读出解析
	ret = MLX90640_DumpEE(SLAVEADDRESS, eeMLX90640);
	printf("MLX90640_DumpEE ret %d \n", ret);
	printf_dump_uint16_t(eeMLX90640, sizeof(eeMLX90640) / sizeof(eeMLX90640[0]));

	if (ret)
	{
		printf("mlx_init error\n");
		return -1;
	}

	ret = MLX90640_ExtractParameters(eeMLX90640, &mlx90640);
	printf("MLX90640_ExtractParameters ret %d \n", ret);

	if (ret)
	{
		printf("mlx_init error\n");
		return -1;
	}

	//根据型号设置参数
	if (mlx_get_sensor_type() == 1)
	{
		ta_shift = 5;	  //偏移系数
		emissivity = 0.95; //发射系数
		sensor_type = 1;
		printf("sensor_type mlx90641\n");
	}
	else
	{
		ta_shift = 8;	  //偏移系数
		emissivity = 0.95; //发射系数
		sensor_type = 0;
		printf("sensor_type mlx90640\n");

		//0x00 – 0.5Hz
		//0x01 – 1Hz
		//0x02 – 2Hz
		//0x03 – 4Hz
		//0x04 – 8Hz
		//0x05 – 16Hz
		//0x06 – 32Hz
		//0x07 – 64Hz
		ret = MLX90640_SetRefreshRate(SLAVEADDRESS, 0x06); //0x04-OK
		printf("MLX90640_SetRefreshRate ret %d \n", ret);

		delay(500); //操作延时
		//读出解析前3笔图像数据，并舍弃
		for (i = 0; i < 3; i++)
		{
			ret = MLX90640_GetFrameData(SLAVEADDRESS, mlx90640Frame);
			printf("MLX90640_GetFrameData ret %d \n", ret);
			//printf_dump_uint16_t(mlx90640Frame,sizeof(mlx90640Frame)/sizeof(mlx90640Frame[0]));

			tr = MLX90640_GetTa(mlx90640Frame, &mlx90640) - ta_shift; //reflected temperature based on the sensor ambient temperature
			printf("MLX90640_GetFrameData tr %f \n", tr);
			MLX90640_CalculateTo(mlx90640Frame, &mlx90640, emissivity, tr, mlx90640To); //The object temperatures

			delay(500); //需要延时
		}
	}

	return 0;
}

/******************************************************************************/
/*
* 函数名称 : mlx_get_sensor_type
* 功能描述 : 获取传感器型号
* 参    数 : 无
* 返回值   : @0		传感器型号 90640 (24*32)
			 @1		传感器型号 90641 (12*16) 
* 示    例 : sensor_type = mlx_get_sensor_type();
*/
/******************************************************************************/
int mlx_get_sensor_type(void)
{
	int ret = 0;

	if (eeMLX90640[0X0A] & 0X40)
	{
		ret = 1;
	}

	return ret;
}

/******************************************************************************/
/*
* 函数名称 : mlx_get_source_data
* 功能描述 : 获取传感器原始图像数据
* 参    数 : @uint16_t *p_source	用于保存数据的指针，申请的内存应该不小于834个uint16_t
* 返回值   : @-1	错误
			 @0		正常
* 示    例 : status = mlx_get_source_data();
*/
/******************************************************************************/
int mlx_get_source_data(uint16_t *p_source)
{
	int ret = 0;
	int i = 0;
	//根据型号进行操作
	if (mlx_get_sensor_type() == 1)
	{
		//90641
	}
	else
	{
		//90640 先进行本地处理
		status = MLX90640_GetFrameData(SLAVEADDRESS, mlx90640Frame);
		//		printf("MLX90640_GetFrameData status %d \n",status);
		// printf_dump_uint16_t(mlx90640Frame,sizeof(mlx90640Frame)/sizeof(mlx90640Frame[0]));

		if (status >= 0)
		{
			for (i = 0; i < sizeof(mlx90640Frame) / sizeof(mlx90640Frame[0]); i++) //确认数据正常，数据更新到上层
			{
				p_source[i] = mlx90640Frame[i];
			}
			ret = 0;
		}
		else
		{
			ret = -1;
		}
	}
	return ret;
}

/******************************************************************************/
/*
* 函数名称 : mlx_get_calculate_data
* 功能描述 : 获取传感器计算后的温度图像数据
* 参    数 : @float *p_calc		用于保存数据的指针，申请的内存应该不小于768个float
			 uint16_t *p_source	原始数据的指针
* 返回值   : @-1	错误
			 @0		正常
* 示    例 : status = mlx_get_calculate_data();
*/
/******************************************************************************/
int mlx_get_calculate_data(float *p_calc, uint16_t *p_source)
{
	//计算环境温度，根据温度范围判断该数据是否正常
	int ret = 0;
	int i = 0;
	//根据型号进行操作
	if (mlx_get_sensor_type() == 1)
	{
		//90641
	}
	else
	{
		//90640
		tr = MLX90640_GetTa(p_source, &mlx90640) - ta_shift; //reflected temperature based on the sensor ambient temperature
															 //		printf("MLX90640_GetFrameData tr %f \n",tr);

		if ((tr > 0) & (tr < 50))
		{
			//确认数据正常，数据更新到上层
			MLX90640_CalculateTo(p_source, &mlx90640, emissivity, tr, mlx90640To); //The object temperatures
			for (i = 0; i < sizeof(mlx90640To) / sizeof(mlx90640To[0]); i++)
			{
				p_calc[i] = mlx90640To[i];
			}
			ret = 0;
		}
		else
		{
			printf("MLX90640_GetTa error\n");
			for (i = 0; i < 834;)
			{
				printf("%04x ", p_source[i]);
				i++;
				if ((i % 32) == 0)
				{
					printf("\n");
				}
			}

			ret = -1;
		}
	}
	return ret;
}
