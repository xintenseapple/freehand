CC=clang
CFLAGS=-emit-llvm -g -O0

all: correct.bc correct.ll

correct.bc: correct.c
	$(CC) $(CFLAGS) -o $@ -c $?

correct.ll: correct.c
	$(CC) $(CFLAGS) -S -o $@ -c $?