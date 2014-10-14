# I am not very good at Makefiles.

CFLAGS += -Wall -O3
INCLUDES += -I.

ifeq (,$(wildcard ../cutest))
CUTEST=.
else
CUTEST = ../cutest
INCLUDES += -I../cutest
endif

all: bin/test_quicklist

bin:
	mkdir -p $@

test: bin/test_quicklist
	@bin/test_quicklist

bin/test_quicklist: test_quicklist.c quicklist.h \
quicklist.c $(CUTEST)/CuTest.c | bin
	$(CC) $(CFLAGS) $(INCLUDES) -lm -o $@ $^

clean:
	@rm -rf *~ bin
