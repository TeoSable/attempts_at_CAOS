CC=/opt/aarch64-gcc/bin/aarch64-linux-gnu-gcc

.PHONY: all build run clean

all: build

clean:
	${RM} func.o
	${RM} main.o
	${RM} main

build: main.o func.o
	$(CC) -g -o main main.o func.o

main.o: main.c
	$(CC) -c -o main.o -g main.c

func.o: func.S 
	$(CC) -c -o func.o -g func.S

run:
	qemu-aarch64 -L /opt/aarch64-sysroot/ ./main $(ARGS)

set_debug:
	sudo qemu-aarch64 -L /opt/aarch64-sysroot/ -g 12 ./main 1 0 0 -27

start_debug:
	/opt/aarch64-gcc/bin/aarch64-linux-gnu-gdb ./main
