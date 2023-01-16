CC=/opt/aarch64-gcc/bin/aarch64-linux-gnu-gcc
asmfile=main

.PHONY: all build run clean

all: build

clean:
	${RM} $(asmfile).o
	${RM} $(asmfile)

build: $(asmfile).o
	$(CC) -g -o $(asmfile) $(asmfile).o

$(asmfile).o: $(asmfile).S 
	$(CC) -c -o $(asmfile).o -g $(asmfile).S

run:
	qemu-aarch64 -L /opt/aarch64-sysroot/ ./$(asmfile) $(ARGS)

set_debug:
	sudo qemu-aarch64 -L /opt/aarch64-sysroot/ -g 12 ./$(asmfile) 1 0 0 -27

start_debug:
	/opt/aarch64-gcc/bin/aarch64-linux-gnu-gdb ./$(asmfile)
