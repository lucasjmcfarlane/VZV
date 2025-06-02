# Makefile

# Compiler and flags
CC = gcc
CFLAGS = -Wall

# Libraries to link
LIBS = -lraylib -lm

# Output binary name
TARGET = VZV

# Source directory and files
SRC_DIR = src
SRCS = $(SRC_DIR)/main.c

# Default target
all: $(TARGET)

# Link and build
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS) $(LIBS)

# Clean build artifacts
clean:
	rm -f $(TARGET)

