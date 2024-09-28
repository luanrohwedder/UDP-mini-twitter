CC = g++
CFLAGS = -Wall -Iinclude -g
GTKMM_FLAGS = `pkg-config --cflags gtkmm-3.0`
LDFLAGS = `pkg-config --libs gtkmm-3.0` -pthread

SRC_DIR = src
CLIENT_DIR = $(SRC_DIR)/client
SERVER_DIR = $(SRC_DIR)/server
INCLUDE_DIR = include
BUILD_DIR = build
BIN_DIR = bin

CLIENT_EXEC = $(BIN_DIR)/client
SERVER_EXEC = $(BIN_DIR)/server

CLIENT_SRCS = $(CLIENT_DIR)/core/client.cpp $(CLIENT_DIR)/gui/login_window.cpp $(CLIENT_DIR)/gui/main_window.cpp $(CLIENT_DIR)/main.cpp
SERVER_SRCS = $(SERVER_DIR)/server.cpp $(SERVER_DIR)/main.cpp

CLIENT_OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(CLIENT_SRCS))
SERVER_OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SERVER_SRCS))

all: $(CLIENT_EXEC) $(SERVER_EXEC) 

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(GTKMM_FLAGS) -c $< -o $@

$(CLIENT_EXEC): $(CLIENT_OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) -o $@ $^ $(LDFLAGS)

$(SERVER_EXEC): $(SERVER_OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) -o $@ $^ -pthread

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR) log.txt

.PHONY: all clean
