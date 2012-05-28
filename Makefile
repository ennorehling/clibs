# I am not very good at Makefiles.

#CFLAGS = -Wall -g
CFLAGS += -Wall -O3
INCLUDES = -Istorage -I.

all: tests

bin:
	mkdir -p $@

tests: bin/tests
	@bin/tests

bin/tests: tests.c \
test_quicklist.c quicklist.c \
cutest/CuTest.c | bin
	$(CC) $(CFLAGS) $(INCLUDES) -lm -o $@ $^

clean:
	@rm -rf *~ bin
