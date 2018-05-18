BIN=./server
CC = gcc
CFLAGS = -Wall -g -lpthread -lmysqlclient
OBJ = server.o sql.o view.o threadpool.o

$(BIN):$(OBJ)
	$(CC) $(OBJ) -o $(BIN) $(CFLAGS)

%.o:%.c
	$(CC) -c $< -o $@

.PHONY:clean $(BIN)
clean:
	rm -rf $(OBJ) $(BIN)
