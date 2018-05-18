BIN=./client
CC = gcc
CFLAGS = -Wall -g -lpthread
OBJ = client.o view.o

$(BIN):$(OBJ)
	$(CC) $(OBJ) -o $(BIN) $(CFLAGS)

%.o:%.c
	$(CC) -c $< -o $@

.PHONY:clean $(BIN)
clean:
	rm -rf $(OBJ) $(BIN)
