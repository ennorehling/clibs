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

tests: bin/test_ctools
	@bin/test_ctools

bin/benchmark: benchmark.c critbit.c | bin
	$(CC) $(CFLAGS) $(INCLUDES) -lm -o $@ $^

bin/naive: naive.c | bin
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^

bin/test_ctools: test_ctools.c \
test_critbit.c critbit.c \
test_quicklist.c quicklist.c \
storage/test_storage.c storage/textstore.c storage/binarystore.c \
cutest/CuTest.c | bin
	$(CC) $(CFLAGS) $(INCLUDES) -lm -o $@ $^

clean:
	@rm -rf *~ bin
