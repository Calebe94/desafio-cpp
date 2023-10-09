###############
# Desafio C++ #
###############

CC=g++
CCFLAGS=-std=c++11 -Wall
SRC_DIR=src
BUILD_DIR=bin
SERVER_SRC=$(SRC_DIR)/server.cpp
CLIENT_SRC=$(SRC_DIR)/client.cpp

SERVER_BINARY=server
CLIENT_BINARY=client

SERVER_COMPILE=$(CC) $(CCFLAGS) -o $(BUILD_DIR)/$(SERVER_BINARY) $(SERVER_SRC) -lboost_system -lboost_thread -DBOOST_BIND_GLOBAL_PLACEHOLDERS
CLIENT_COMPILE=$(CC) $(CCFLAGS) -o $(BUILD_DIR)/$(CLIENT_BINARY) $(CLIENT_SRC) -lboost_system -DBOOST_BIND_GLOBAL_PLACEHOLDERS

all: server client

server: $(BUILD_DIR)
	@$(SERVER_COMPILE)

client: $(BUILD_DIR)
	@$(CLIENT_COMPILE)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all server client clean
