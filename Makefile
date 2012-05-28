# I am not very good at Makefiles.

#CFLAGS = -Wall -g
CFLAGS += -Wall -O3
INCLUDES = -Istorage -I.

all: benchmarks tests

bin:
	mkdir -p $@

benchmarks: bin/benchmark bin/naive
	@bin/benchmark 100 1
	@bin/naive 100 1

tests: bin/tests
	@bin/tests

bin/benchmark: benchmark.c strtolh.c critbit.c | bin
	$(CC) $(CFLAGS) $(INCLUDES) -lm -o $@ $^

bin/naive: naive.c strtolh.c | bin
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^

bin/tests: tests.c \
test_critbit.c critbit.c \
cutest/CuTest.c | bin
	$(CC) $(CFLAGS) $(INCLUDES) -lm -o $@ $^

clean:
	@rm -rf *~ bin
