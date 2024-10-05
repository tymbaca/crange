# Define the compiler
CC = gcc

# Define the target executable
TARGET = program

# Define source files
SRCS = main.c

# Define object files
OBJS = $(SRCS:.c=.o)

# Define compiler flags
CFLAGS = -Wall -g -I/opt/homebrew/Cellar/raylib/5.0/include

# Define linker flags
LDFLAGS = -L/opt/homebrew/Cellar/raylib/5.0/lib -lraylib -framework OpenGL -framework Cocoa -framework IOKit

# Rule to build the target executable
$(TARGET): $(OBJS)
	@$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

# Rule to build object files
%.o: %.c
	@$(CC) $(CFLAGS) -c $< -o $@

# Clean up object files and executable
clean:
	rm -f $(OBJS) $(TARGET)

# Rule to run the program
run: $(TARGET)
	@./$(TARGET)

# Phony targets
.PHONY: clean run
