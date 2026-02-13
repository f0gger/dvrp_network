INC_DIR := include
LIB_DIR := lib
SRC_DIR := src
OBJ_DIR := build
BIN_DIR := bin

CC      := gcc
CFLAGS  := -I$(INC_DIR) -Wall -pthread -Llib
LDFLAGS := 

SERVER  := $(BIN_DIR)/server
CLIENT  := $(BIN_DIR)/client

LIB_SRC    := $(wildcard $(LIB_DIR)/*c)
SERVER_SRC := $(SRC_DIR)/server.c 
CLIENT_SRC := $(SRC_DIR)/client.c

LIB_OBJ    := $(patsubst $(LIB_DIR)/%.c,$(OBJ_DIR)/lib_%.o,$(LIB_SRC))
SERVER_OBJ := $(OBJ_DIR)/server.o
CLIENT_OBJ := $(OBJ_DIR)/client.o

all: $(SERVER) $(CLIENT)

$(SERVER_OBJ): $(SERVER_SRC)
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(CLIENT_OBJ): $(CLIENT_SRC)
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/lib_%.o: $(LIB_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(SERVER): $(SERVER_OBJ) $(LIB_OBJ)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(CLIENT): $(CLIENT_OBJ) $(LIB_OBJ)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f $(SERVER) $(CLIENT)

.PHONY: all clean
