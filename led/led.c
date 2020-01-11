#include <wiringPi.h>
#include <stdio.h>

int main (void)
{
     wiringPiSetup () ; // 初始化 这个必须写在最前面
     pinMode (0, OUTPUT) ; // 设置GPIO.0 为输出
    for (;;) // 无限循环
    {
       digitalWrite (0, HIGH);  // GPIO.0 输出高
       delayMicroseconds (1);  
       digitalWrite (0, LOW); // GPIO.0 输出低
       delayMicroseconds (1); 
    }
    return 0 ;
}
