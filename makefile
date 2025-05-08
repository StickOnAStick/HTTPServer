# Detect platform
UNAME_S := $(shell uname -s)

# Default Compiler
CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c11 -Iinc

# OS-Specific Flags
ifeq ($(UNAME_S), Darwin)  # macOS
    CFLAGS += -D_DARWIN_C_SOURCE
else # Linux
    CFLAGS += -D_POSIX_C_SOURCE=200809L
endif

SRC = src
BIN = bin
OBJ = $(BIN)/main.o $(BIN)/request.o $(BIN)/response.o $(BIN)/queue.o $(BIN)/worker.o

all: $(BIN) server

$(BIN):
	mkdir -p $(BIN)

server: $(OBJ)
	$(CC) $(CFLAGS) -o $(BIN)/server $(OBJ)

# Responses / Requests
$(BIN)/request.o: $(SRC)/request.c inc/request.h inc/header.h | $(BIN)
	$(CC) $(CFLAGS) -c $(SRC)/request.c -o $(BIN)/request.o

$(BIN)/response.o: $(SRC)/response.c inc/response.h | $(BIN)
	$(CC) $(CFLAGS) -c $(SRC)/response.c -o $(BIN)/response.o

# Internal server functioning
$(BIN)/queue.o: inc/queue.h inc/task.h | $(BIN)
	$(CC) $(CFLAGS) -c $(SRC)/queue.c -o $(BIN)/queue.o

$(BIN)/worker.o: inc/queue.h inc/worker.h | $(BIN)
	$(CC) $(CFLAGS) -c $(SRC)/worker.c -o $(BIN)/worker.o

$(BIN)/main.o: main.c inc/request.h inc/response.h inc/header.h inc/queue.h inc/worker.h | $(BIN)
	$(CC) $(CFLAGS) -c main.c -o $(BIN)/main.o


clean:
	rm -rf $(BIN)
