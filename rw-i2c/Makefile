src  :=$(shell ls *.c)
objs :=$(patsubst %.c,%.o,$(src))

output.bin: $(objs)
	arm-linux-gnueabihf-gcc  -o $@ -g $^ -lwiringPi -lm
%.o:%.c
	arm-linux-gnueabihf-gcc  -c -o $@ -g $< -lstdc++ -lm
