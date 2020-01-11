src  :=$(shell ls *.c)
objs :=$(patsubst %.c,%.o,$(src))

output.bin: $(objs)
	#gcc -o $@ $^
	arm-linux-gnueabihf-gcc -o $@ $^ -lwiringPi
%.o:%.c
	gcc -c -o $@ $<
