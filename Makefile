CC = gcc
all: manager_main employee_main user_main
CFLAGS = -g -Wall -Wextra -Wpedantic -Wconversion -Iinclude/

INCLUDES = include/*.h

COMMON_DEPS = $(INCLUDES)
PROJ_DEPS = build/utils.o build/config.o

build/%.o: src/%.c $(COMMON_DEPS)
	$(CC) $(CFLAGS) -c $< -o $@ $(END_CFLAGS)

build/tests/%.o: tests/%.c $(COMMON_DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

manager_main: build/manager_main.o build/sem_utils.o $(PROJ_DEPS)
	$(CC) $(CFLAGS) -o bin/manager_main build/manager_main.o build/sem_utils.o $(PROJ_DEPS)

employee_main: build/employee_main.o build/sem_utils.o $(PROJ_DEPS)
	$(CC) $(CFLAGS) -o bin/employee_main build/employee_main.o build/sem_utils.o $(PROJ_DEPS)

user_main: build/user_main.o build/sem_utils.o $(PROJ_DEPS)
	$(CC) $(CFLAGS) -o bin/user_main build/user_main.o build/sem_utils.o $(PROJ_DEPS)

#ex1/test_ex1: build/tests/sorting_test.o build/sorting.o build/unity.o
#	$(CC) $(CFLAGS) -o ex1/test_ex1 build/tests/sorting_test.o build/sorting.o build/unity.o $(END_CFLAGS)

clean:
	rm -f build/* bin/*
