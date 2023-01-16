.PHONY: all build run clean
filename=main

all: build_c

clean:
	$(RM) main

build_c: $(filename).c
	gcc -g -nostdlib -fno-stack-protector -o main $(filename).c syscall.s

build_S: $(filename).S
	gcc -g -nostdlib -c -o main.o $(filename).S
	ld -o main main.o

run:
	./main
