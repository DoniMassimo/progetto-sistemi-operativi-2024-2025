CC = gcc
all: ex1/main_ex1 ex1/test_ex1
CFLAGS = -g -Wall -Wextra -Wpedantic -Wconversion -Iinclude/

INCLUDES = include/*.h

COMMON_DEPS = $(INCLUDES)

build/%.o: src/%.c $(COMMON_DEPS)
	$(CC) $(CFLAGS) -c $< -o $@ $(END_CFLAGS)

build/tests/%.o: tests/%.c $(COMMON_DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

ex1/main_ex1: build/sorting_main.o build/sorting.o
	$(CC) $(CFLAGS) -o ex1/main_ex1 build/sorting_main.o build/sorting.o $(END_CFLAGS)

ex1/test_ex1: build/tests/sorting_test.o build/sorting.o build/unity.o
	$(CC) $(CFLAGS) -o ex1/test_ex1 build/tests/sorting_test.o build/sorting.o build/unity.o $(END_CFLAGS)

clean:
	rm -f build/* bin/*
