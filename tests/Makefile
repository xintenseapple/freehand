CC=clang
CFLAGS=-emit-llvm -g -O0

TESTS = simple/correct/correct simple/df/df simple/uaf/uaf

all: $(TESTS)

%: %.c
	$(CC) $(CFLAGS) -o $@.bc -c $?
	$(CC) $(CFLAGS) -S -o $@.ll -c $?