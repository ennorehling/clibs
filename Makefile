# I am not very good at Makefiles.

CFLAGS += -g -O3 -Wall -Wextra -Wstrict-aliasing=2 -Wno-unused-function

ifeq "$(CC)" "clang"
CFLAGS += -Weverything -Wno-padded
# stop clang warning us about strncmp:
CFLAGS += -Wno-disabled-macro-expansion
endif

all: test

test: bin/test_critbit bin/test_quicklist 
	@bin/test_critbit
	@bin/test_quicklist

bin obj:
	mkdir -p $@

obj/%.c.o: %.c | obj
	$(CC) -o $@ -c $^ $(CFLAGS) $(INCLUDES)

clean:
	@rm -rf bin obj

obj/%.o: %.c %.h | obj
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES)

obj/test_%.o: test_%.c %.h | obj
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES)

bin/test_%: obj/test_%.o obj/%.o \
 obj/CuTest.o | bin
	$(CC) $(CFLAGS) $(INCLUDES) -lm $^ -o $@

