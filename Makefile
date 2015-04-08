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

bin obj:
	mkdir -p $@

test: bin/test_quicklist
	@bin/test_quicklist

obj/%.o: %.c %.h | obj
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES)

obj/test_%.o: test_%.c %.h | obj
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES)

obj/CuTest.o: $(CUTEST)/CuTest.c $(CUTEST)/CuTest.h | obj
	$(CC) -o $@ -c $(CUTEST)/CuTest.c $(CFLAGS) -I$(CUTEST)

bin/test_quicklist: obj/test_quicklist.o obj/quicklist.o \
 obj/CuTest.o | bin
	$(CC) $(CFLAGS) $(INCLUDES) -lm $^ -o $@

clean:
	@rm -rf *~ bin obj
