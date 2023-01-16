CC=/opt/aarch64-gcc/bin/aarch64-linux-gnu-gcc
asmfile=solve

.PHONY: all build run clean

all: build

clean:
	${RM} $(asmfile).o
	${RM} main.o
	${RM} main

build: main.o $(asmfile).o
	$(CC) -g -o main main.o $(asmfile).o

main.o: main.c
	$(CC) -c -o main.o -g main.c

$(asmfile).o: $(asmfile).S 
	$(CC) -c -o $(asmfile).o -g $(asmfile).S

run:
	qemu-aarch64 -L /opt/aarch64-sysroot/ ./main $(ARGS)

set_debug:
	sudo qemu-aarch64 -L /opt/aarch64-sysroot/ -g 12 ./main 1 0 0 -27

start_debug:
	/opt/aarch64-gcc/bin/aarch64-linux-gnu-gdb ./main
