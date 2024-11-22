# Variables
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -ggdb 
SRC_DIR = src
BIN_DIR = bin
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c,$(BIN_DIR)/%.o,$(SRC))
TARGET = $(BIN_DIR)/shell
LIB = -lreadline

# Target to build the executable
$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LIB)

# Rule to compile .c files to .o files
$(BIN_DIR)/%.o: $(SRC_DIR)/%.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -c $< -o $@ 

# Create bin directory if it doesn't exist
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Clean up object files and the executable
clean:
	rm -f $(BIN_DIR)/*.o $(TARGET)

run:
	./$(TARGET)



