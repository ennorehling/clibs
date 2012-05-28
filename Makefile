# I am not very good at Makefiles.

INCLUDES = -I.
CFLAGS = -Wall -O3 $(INCLUDES)
CPPFLAGS = -Wall -O3 $(INCLUDES)

all: benchmarks tests

bin obj:
	mkdir -p $@

benchmarks: bin/benchmark bin/naive bin/james
	@bin/benchmark 100 1
	@bin/naive 100 1
	@bin/james 100 1

tests: bin/tests
	@bin/tests

obj/%.c.o: %.c | obj
	$(CC) -o $@ -c $^ $(CFLAGS)

obj/%.cpp.o: %.cpp | obj
	$(CXX) -o $@ -c $^ $(CPPFLAGS)

bin/benchmark: obj/benchmark.c.o obj/critbit.c.o obj/strtolh.c.o | bin
	$(CC) $(CFLAGS) $(INCLUDES) -lm -o $@ $^

bin/naive: obj/naive.c.o obj/strtolh.c.o | bin
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^

bin/james: obj/james.cpp.o obj/strtolh.c.o | bin
	$(CXX) -o $@ $^

bin/tests: tests.c \
 test_critbit.c critbit.c \
 cutest/CuTest.c | bin
	$(CC) $(CFLAGS) $(INCLUDES) -lm -o $@ $^

clean:
	@rm -rf *~ bin obj
