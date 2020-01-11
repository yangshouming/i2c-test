#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

#include "add2.h"
//****************************************
// 定义内部地址
//****************************************


int val;
int sum_val;
int main (void)
{
	printf("start \n");	
    for(;;)
    {
		val++;
		printf("val %d \n",val);
		sum_val=add2(val);
		printf("sum_val %d \n",sum_val);
		delay(1000);
		
    }
    return 0 ;
}
