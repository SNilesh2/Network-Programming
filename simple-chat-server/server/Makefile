# Compiler and Flags
CC = gcc
# -Iinclude tells the compiler to look in the include directory for .h files
CFLAGS = -Wall -Wextra -g -Iinclude
LIBS = -lssl -lcrypto

# Target executable name
TARGET = chat_server

# Directories
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build

# Source files (located in src/)
SRCS = $(SRC_DIR)/main.c \
       $(SRC_DIR)/server.c \
       $(SRC_DIR)/epoll_mgr.c \
       $(SRC_DIR)/websocket.c \
       $(SRC_DIR)/chat_mgr.c

# Object files (placed in build/)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Default rule
all: prepare $(TARGET)

# Create the build directory if it doesn't exist
prepare:
	mkdir -p $(BUILD_DIR)

# Link the object files into the final binary
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

# Compile each .c file into a .o file in the build directory
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: all clean prepare
