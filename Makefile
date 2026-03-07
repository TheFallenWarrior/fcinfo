# fcinfo
# Copyright 2026 TheFallenWarrior

# Compiler and flags
CC = gcc
CFLAGS  = -Wall -Wextra -Ofast
LDFLAGS = -lm

# Project name and directories
TARGET  = fcinfo
SRC_DIR = .
OBJ_DIR = obj
BIN_DIR = bin

# Source and object files
SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SOURCES))

# Phony targets
.PHONY: all clean

# Default target
all: $(BIN_DIR)/$(TARGET)

# Link object files to create the executable
$(BIN_DIR)/$(TARGET): $(OBJECTS) | $(BIN_DIR)
	$(CC) $(LDFLAGS) -o $@ $(OBJECTS)

# Compile source files to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

# Create directories if they don't exist
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

cppcheck:
	cppcheck -q -x c --enable=all --suppress=missingIncludeSystem $(SRC_DIR)

clean:
	rm -rf $(BIN_DIR) $(OBJ_DIR)