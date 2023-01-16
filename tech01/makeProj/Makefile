CC := gcc
HASH := \#
TEXT := "$$(sed '/^$$/d;/^$(HASH)/d;s/.*\=//g;/^[0-9]*$$/d' compile.conf | )"

.PHONY : all build clean

all : build

build : program.exe

# Is this a comment?
# Yes, it is.

clean :
	rm -f program.o
	rm -f program.exe

program.exe : program.o
	${CC} -g -o program.exe program.o

program.o : program.c
	${CC} -DTEXT_VARS=${TEXT} -g -fPIC -c -o program.o program.c
