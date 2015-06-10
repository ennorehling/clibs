# I am not very good at Makefiles.

INCLUDES += -I.

ifeq (,$(wildcard ../cutest))
CUTEST=.
else
CUTEST = ../cutest
INCLUDES += -I../cutest
endif

CFLAGS += -Wall -Wstrict-aliasing=2 -O3 -Wconversion -Wextra
CPPFLAGS += -Wall -O3

ifdef DLMALLOC
LIBS += ${DLMALLOC}/lib/libmalloc.a
endif

all: bin/benchmark bin/naive bin/james

bin obj:
	mkdir -p $@

benchmarks: bin/benchmark bin/naive bin/james
	@bin/benchmark 100 1
	@bin/naive 100 1
	@bin/james 100 1

test: bin/tests
	@bin/tests

obj/%.c.o: %.c | obj
	$(CC) -o $@ -c $^ $(CFLAGS) $(INCLUDES)

obj/%.cpp.o: %.cpp | obj
	$(CXX) -o $@ -c $^ $(CPPFLAGS) $(INCLUDES)

bin/benchmark: obj/benchmark.c.o obj/critbit.c.o obj/strtolh.c.o | bin
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

bin/naive: obj/naive.c.o obj/strtolh.c.o | bin
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)

bin/james: obj/james.cpp.o obj/strtolh.c.o | bin
	$(CXX) -o $@ $^ $(LIBS)

bin/tests: critbit_tests.c \
 test_critbit.c critbit.c \
 $(CUTEST)/CuTest.c | bin
	$(CC) $(CFLAGS) $(INCLUDES) -lm -o $@ $^ $(LIBS)

clean:
	@rm -rf *~ bin obj
