src  :=$(shell ls *.c)
objs :=$(patsubst %.c,%.o,$(src))

output.bin: $(objs)
	arm-linux-gnueabihf-gcc -o $@ $^ -lwiringPi
%.o:%.c
	gcc -c -o $@ $<
