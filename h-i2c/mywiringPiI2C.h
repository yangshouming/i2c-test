/*
 * mywiringPiI2C.h:
 *	Simplified I2C access routines
 *	Copyright (c) 2013 Gordon Henderson
 ***********************************************************************
 * This file is part of wiringPi:
 *	https://projects.drogon.net/raspberry-pi/wiringpi/
 *
 *    wiringPi is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Lesser General Public License as
 *    published by the Free Software Foundation, either version 3 of the
 *    License, or (at your option) any later version.
 *
 *    wiringPi is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with wiringPi.
 *    If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************
 */

#ifdef __cplusplus
extern "C" {
#endif

extern int mywiringPiI2CRead           (int fd) ;
extern int mywiringPiI2CReadReg8       (int fd, int reg) ;
extern int mywiringPiI2CReadReg16      (int fd, int reg) ;

extern int mywiringPiI2CWrite          (int fd, int data) ;
extern int mywiringPiI2CWriteReg8      (int fd, int reg, int data) ;
extern int mywiringPiI2CWriteReg16     (int fd, int reg, int data) ;

extern int mywiringPiI2CSetupInterface (const char *device, int devId) ;
extern int mywiringPiI2CSetup          (const int devId) ;

extern int mywiringPiI2CWriteReg16_mlx (int fd, int reg, int value);
    

/*
int fd, --文件句柄
uint16_t reg, 寄存器地址
uint16_t *value,数据指针
uint16_t length 长度(byte)
*/
extern int mywiringPiI2CReadReg16_mlx (int fd, uint16_t reg, uint16_t *value,uint16_t length);



#ifdef __cplusplus
}
#endif
