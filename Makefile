CFLAGS = -Wall -g
CFLAGS = -Wall -O3
INCLUDES = -Istorage -I.

all: bin benchmarks tests

bin:
	mkdir -p $@

tests: bin/test_ctools
	@bin/test_ctools

benchmarks: bin/benchmark
	@bin/benchmark

binarystore.o: storage/binarystore.c storage/binarystore.h storage/storage.h
	$(CC) -c storage/binarystore.c $(INCLUDES) $(CFLAGS)

textstore.o: storage/textstore.c storage/textstore.h storage/storage.h
	$(CC) -c storage/textstore.c $(INCLUDES) $(CFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

bin/benchmark: benchmark.o critbit.o
	$(CC) -o bin/benchmark benchmark.o critbit.o $(INCLUDES) $(CFLAGS) -lm

bin/test_ctools: quicklist.o critbit.o binarystore.o textstore.o \
test_ctools.c storage/test_storage.c test_quicklist.c test_critbit.c
	$(CC) -o $@ textstore.o binarystore.o quicklist.o critbit.o test_ctools.c test_quicklist.c test_critbit.c cutest/CuTest.c storage/test_storage.c $(INCLUDES) $(CFLAGS) -lm

clean:
	@rm -rf *.o *~ bin

