# I am not very good at Makefiles.

ifndef CUTEST
CUTEST = ../cutest
endif

CFLAGS += -Wall -O3
INCLUDES = -I$(CUTEST) -I.

all: tests

bin:
	mkdir -p $@

test: bin/test_quicklist
	@bin/test_quicklist

bin/test_quicklist: test_quicklist.c \
quicklist.c $(CUTEST)/CuTest.c | bin
	$(CC) $(CFLAGS) $(INCLUDES) -lm -o $@ $^

clean:
	@rm -rf *~ bin
