CC=clang
CFLAGS=-emit-llvm -g -O0

TESTS = simple/correct/correct simple/df/df simple/uaf/uaf \
		loops/correct/correct loops/df/df loops/uaf/uaf \
		conditionals/correct/correct conditionals/df/df conditionals/uaf/uaf

all: $(TESTS)

%: %.c
	$(CC) $(CFLAGS) -o $@.bc -c $?
	$(CC) $(CFLAGS) -S -o $@.ll -c $?