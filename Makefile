CC = gcc
all: bin/manager_main bin/employee_main bin/clock
CFLAGS = -g -Wall -Wextra -Wpedantic -Wconversion -Wstrict-prototypes -Wsign-conversion -D_GNU_SOURCE -Iinclude/

INCLUDES = include/*.h

COMMON_DEPS = $(INCLUDES)

PROJ_DEPS = build/utils.o build/config.o build/ipc_config.o build/sem.o build/msg.o build/shm.o build/ftok_key.o

build/%.o: src/%.c $(COMMON_DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

bin/manager_main: build/manager_main.o build/sem_utils.o build/seats.o $(PROJ_DEPS)
	$(CC) $(CFLAGS) -o bin/manager_main build/manager_main.o build/sem_utils.o build/seats.o $(PROJ_DEPS)

bin/employee_main: build/employee_main.o build/sem_utils.o build/seats.o $(PROJ_DEPS)
	$(CC) $(CFLAGS) -o bin/employee_main build/employee_main.o build/sem_utils.o build/seats.o $(PROJ_DEPS)

bin/clock: build/clock.o build/sem_utils.o $(PROJ_DEPS)
	$(CC) $(CFLAGS) -o bin/clock build/clock.o build/sem_utils.o $(PROJ_DEPS)


#bin/user_main: build/user_main.o build/sem_utils.o $(PROJ_DEPS)
#	$(CC) $(CFLAGS) -o bin/user_main build/user_main.o build/sem_utils.o $(PROJ_DEPS)

clean:
	rm -f build/* bin/*



#build/tests/%.o: tests/%.c $(COMMON_DEPS)
#	$(CC) $(CFLAGS) -c $< -o $@

