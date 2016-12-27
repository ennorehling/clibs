# I am not very good at Makefiles.

CFLAGS += -g -O3 -Wall -Wextra -Wstrict-aliasing=2 -Wno-unused-function

ifeq "$(CC)" "clang"
CFLAGS += -Weverything -Wno-padded
# stop clang warning us about strncmp:
CFLAGS += -Wno-disabled-macro-expansion
endif

all: test

test: bin/tests
	@bin/tests

bin obj:
	mkdir -p $@

clean:
	@rm -rf bin obj

obj/%.o: %.c %.h | obj
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES)

obj/test_%.o: test_%.c %.h | obj
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES)

bin/tests: tests.c obj/CuTest.o \
	obj/test_fastlist.o obj/fastlist.o \
	obj/test_critbit.o obj/critbit.o \
	obj/test_strutil.o obj/strutil.o | bin
	$(CC) $(CFLAGS) $(INCLUDES) -lm $^ -o $@

