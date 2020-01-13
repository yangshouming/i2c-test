/**
 * @copyright (C) 2017 Melexis N.V.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
/**
 * As the timings depend heavily on the MCU in use, it is recommended
 * to make sure that the proper timings are achieved. For that purpose
 * an oscilloscope might be needed to strobe the SCL and SDA signals.
 * The Wait(int) function could be modified in order to better 
 * trim the frequency. For coarse setting of the frequency or 
 * dynamic frequency change using the default function implementation, 
 * ‘freqCnt’ argument should be changed – lower value results in 
 * higher frequency.
 */

#include <wiringPi.h>
#include "MLX90640_I2C_Driver.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>

#include <wiringPi.h>

#include <errno.h>

#define DEVICE_NODE "/dev/i2c-1"

int fd;

int hal_i2c_init(void)
{
    fd = 0;
    fd = open(DEVICE_NODE, O_RDWR); //打开设备节点
    if (fd < 0)
    {
        printf("%s:fd %d error\n", DEVICE_NODE, fd);
        return fd;
    }
    else
    {
        printf("%s:fd %d ok\n", DEVICE_NODE, fd);
    }

    ioctl(fd, I2C_TIMEOUT, 1); //设置超时时间
    ioctl(fd, I2C_RETRIES, 2); //设置重试次数

    return fd;
}

/*****************************************************************************
  i2c读函数，参数1：从设备地址，参数2：寄存器地址，参数3：读取数据缓冲区，参数4：读取数据大小(byte)
******************************************************************************/
int hal_i2c_read(uint8_t device_addr, uint16_t reg_addr, uint8_t *buff, int ByteNo)
{
    int ret;
    uint8_t buftmp[32];
    struct i2c_rdwr_ioctl_data i2c_data;

    //init
    i2c_data.nmsgs = 2; //命令的条数（组数）
    i2c_data.msgs = (struct i2c_msg *)malloc(i2c_data.nmsgs * sizeof(struct i2c_msg));
    if (i2c_data.msgs == NULL)
    {
        printf("malloc error");
        return -1;
    }

    //write reg addr
    memset(buftmp, 0, 32);
    buftmp[0] = reg_addr >> 8;
    buftmp[1] = reg_addr;
    i2c_data.msgs[0].len = 2; //启动多少个时钟周期   发送的字节个数 2 地址宽度为2byte
    i2c_data.msgs[0].addr = device_addr;
    i2c_data.msgs[0].flags = 0; // 0: write 1:read
    i2c_data.msgs[0].buf = buftmp;
    //read data
    i2c_data.msgs[1].len = ByteNo; //启动多少个时钟周期   读N个数据
    i2c_data.msgs[1].addr = device_addr;
    i2c_data.msgs[1].flags = 1; // 0: write 1:read
    i2c_data.msgs[1].buf = buff;

    ret = ioctl(fd, I2C_RDWR, (unsigned long)&i2c_data);

    // if (ret < 0)
    // {
    //     printf("read data %x %x error\r\n", device_addr, reg_addr);
    //     printf("ioctl: %s\n", strerror(errno));
    //     free(i2c_data.msgs);
    //     return ret;
    // }

    free(i2c_data.msgs);

#if 0
    int i;
    printf("hal_i2c_read 0x%02x:", buftmp[0]);
    for (i = 0; i < ByteNo; i++)
    {
        printf(" 0x%02x", buff[i]);
    }
    printf("\n");
#endif

    return 0;
}

/*****************************************************************************
  i2c写函数，参数1：从设备地址，参数2：寄存器地址，参数3：要写入的数据缓冲区，参数4：写入数据大小(byte)
******************************************************************************/
int hal_i2c_write(uint8_t device_addr, uint16_t reg_addr, uint8_t *buff, int ByteNo)
{

    int ret;
    uint8_t buftmp[32];
    struct i2c_rdwr_ioctl_data i2c_data;

    //init
    i2c_data.nmsgs = 1;
    i2c_data.msgs = (struct i2c_msg *)malloc(i2c_data.nmsgs * sizeof(struct i2c_msg));
    if (i2c_data.msgs == NULL)
    {
        printf("malloc error");
        return -1;
    }

    //write reg addr
    memset(buftmp, 0, 32);
    buftmp[0] = reg_addr >> 8;
    buftmp[1] = reg_addr;
    memcpy(buftmp + 2, buff, ByteNo);
    i2c_data.msgs[0].len = ByteNo + 2;

    i2c_data.msgs[0].addr = device_addr;
    i2c_data.msgs[0].flags = 0; // 0: write 1:read
    i2c_data.msgs[0].buf = buftmp;
    ret = ioctl(fd, I2C_RDWR, (unsigned long)&i2c_data);
    if (ret < 0)
    {
        printf("write reg %x %x error\r\n", device_addr, reg_addr);
        free(i2c_data.msgs);
        return 1;
    }
    free(i2c_data.msgs);

#if 1
    int i;
    printf("hal_i2c_write 0x%02x:", buftmp[0]);
    for (i = 0; i < ByteNo; i++)
    {
        printf(" 0x%02x", buftmp[1 + i]);
    }
    printf("\n");
#endif

    return 0;
}

int MLX90640_I2CInit()
{
    int ret = 0;
    if (hal_i2c_init() < 0)
    {
        ret = -1;
    }

    return ret;
}

/*
* 函数名称 : MLX90640_I2CRead
* 功能描述 : This function reads a desired number of words from a selected MLX90640 device memory starting from a
given address and stores the data in the MCU memory location defined by the user. If the returned value is
0, the communication is successful, if the value is -1
* 参    数 : slaveAddr-器件地址（没有移位之前） 默认0x33
             startAddress-寄存器地址 
             nMemAddressRead-读取的数据个数（16bit）
             data-读取的数据指针
* 返回值   :
* 示    例 ：
*/
/******************************************************************************/
int MLX90640_I2CRead(uint8_t slaveAddr, uint16_t startAddress, uint16_t nMemAddressRead, uint16_t *data)
{

    uint8_t iic_read_buff[1664] = {0x00};
    uint32_t err_code = 0;

    uint16_t cnt = 0;
    uint16_t i = 0;
    uint16_t *p = data;

    //读出数据
    err_code = hal_i2c_read(slaveAddr, startAddress, iic_read_buff, nMemAddressRead * 2);
    //err_code = HAL_I2C_Mem_Read(&hi2c1, slaveAddr << 1, startAddress, I2C_MEMADD_SIZE_16BIT, iic_read_buff, nMemAddressRead * 2, 1000);

    if (err_code)
    {
        return -1;
    }

    //数据复制
    for (cnt = 0; cnt < nMemAddressRead; cnt++)
    {
        i = cnt << 1;
        *p++ = (uint16_t)iic_read_buff[i] * 256 + (uint16_t)iic_read_buff[i + 1];
    }

    return 0;
}

/*
* 函数名称 : MLX90640_I2CFreqSet
* 功能描述 : 设定频率
* 参    数 : freq-频率 单位 KHZ ,100=100KHZ
* 返回值   :
* 示    例 ：
*/
/******************************************************************************/
void MLX90640_I2CFreqSet(int freq)
{
    ;
}

/*
* 函数名称 : MLX90640_I2CWrite
* 功能描述 : This function writes a 16-bit value to a desired memory address of a selected MLX90640 device. The function
reads back the data after the write operation is done and returns 0 if the write was successful, -1 if NACK
occurred during the communication and -2 if the data in the memory is not the same as the intended one
* 参    数 : slaveAddr-器件地址（没有移位之前）
             writeAddress-寄存器地址
             data-读取的数据指针（16bit）
* 返回值   :
* 示    例 ：
*/
/******************************************************************************/
int MLX90640_I2CWrite(uint8_t slaveAddr, uint16_t writeAddress, uint16_t data)
{

    uint8_t iic_write_buff[4] = {0x00};
    //uint8_t iic_read_buff[1664]= {0x00};
    uint32_t err_code = 0;

    uint16_t dataCheck;

    iic_write_buff[0] = data >> 8;     //数据
    iic_write_buff[1] = data & 0x00FF; //数据

    err_code = hal_i2c_write(slaveAddr, writeAddress, iic_write_buff, 2);
    //err_code = HAL_I2C_Mem_Write(&hi2c1, slaveAddr << 1, writeAddress, I2C_MEMADD_SIZE_16BIT, iic_write_buff, 2, 1000);
    if (err_code)
    {
        return -1;
    }

    MLX90640_I2CRead(slaveAddr, writeAddress, 1, &dataCheck);

    if (dataCheck != data)
    {
        return -2;
    }

    return 0;
}
