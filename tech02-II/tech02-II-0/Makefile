CC=/opt/aarch64-gcc/bin/aarch64-linux-gnu-gcc

.PHONY: all build run clean

all: build

clean:
	${RM} solve.o
	${RM} main.o
	${RM} main

build: main.o solve.o
	$(CC) -g -o main main.o solve.o

main.o: main.c
	$(CC) -c -o main.o -g main.c

solve.o: solve.S 
	$(CC) -c -o solve.o -g solve.S

run:
	qemu-aarch64 -L /opt/aarch64-sysroot/ ./main $(ARGS)

set_debug:
	sudo qemu-aarch64 -L /opt/aarch64-sysroot/ -g 12 ./main 1 0 0 -27

start_debug:
	/opt/aarch64-gcc/bin/aarch64-linux-gnu-gdb ./main
