CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c11 -Iinc -D_POSIX_C_SOURCE=200809L
SRC = src
BIN = bin
OBJ = $(BIN)/main.o $(BIN)/request.o $(BIN)/response.o

all: $(BIN) server

$(BIN):
	mkdir -p $(BIN)

server: $(OBJ)
	$(CC) $(CFLAGS) -o $(BIN)/server $(OBJ)

$(BIN)/request.o: $(SRC)/request.c inc/request.h inc/header.h | $(BIN)
	$(CC) $(CFLAGS) -c $(SRC)/request.c -o $(BIN)/request.o

$(BIN)/response.o: $(SRC)/response.c inc/response.h | $(BIN)
	$(CC) $(CFLAGS) -c $(SRC)/response.c -o $(BIN)/response.o

$(BIN)/main.o: main.c inc/request.h inc/response.h inc/header.h | $(BIN)
	$(CC) $(CFLAGS) -c main.c -o $(BIN)/main.o

# routes.o: $(SRC)/routes.c inc/routes.h  # Future use ;)
# 	$(CC) $(CFLAGS) -c $(SRC)/routes.c -o $(BIN)/routes.o

clean:
	rm -rf $(BIN)
