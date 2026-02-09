# =========================
# Compiler & Flags
# =========================
CC      := gcc
CFLAGS  := -Wall -Wextra -g -Iinclude
LDFLAGS :=

# =========================
# Project Structure
# =========================
SRC_DIR   := src
INC_DIR   := include
BUILD_DIR := build

# =========================
# Source / Object Files
# =========================
SRC := \
	$(SRC_DIR)/main.c \
	$(SRC_DIR)/proxy.c \
	$(SRC_DIR)/epoll_mgr.c \
	$(SRC_DIR)/client.c \
	$(SRC_DIR)/server.c \
	$(SRC_DIR)/http.c

OBJ := $(SRC:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
DEP := $(OBJ:.o=.d)

# =========================
# Output Binary
# =========================
BIN := $(BUILD_DIR)/http_proxy

# =========================
# Default Target
# =========================
all: $(BIN)

# =========================
# Link Step
# =========================
$(BIN): $(OBJ) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJ)

# =========================
# Compile Step
# =========================
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

# =========================
# Build Directory
# =========================
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# =========================
# Dependency Includes
# =========================
-include $(DEP)

# =========================
# Run Proxy (example port)
# =========================
run: $(BIN)
	./$(BIN) 8080

# =========================
# Clean Build
# =========================
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean run

