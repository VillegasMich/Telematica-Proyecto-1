# Define the compiler
CC = gcc

# Define the compiler flags
CFLAGS = -Wall -Wextra -g

# Define the target executable
TARGET1 = client
TARGET2 = server

# Define the source files
SRCS1 = src/client.c src/mjep.c
SRCS2 = src/server.c src/mjep.c

# Define the object files
OBJS1 = $(patsubst src/%.c, build/%.o, $(SRCS1))
OBJS2 = $(patsubst src/%.c, build/%.o, $(SRCS2))

# Default rule to build the target
all: $(TARGET1) $(TARGET2)

# Rule to link the object files into the target executable
$(TARGET1): $(OBJS1)
	mkdir -p build
	$(CC) $(CFLAGS) -o build/$@ $^

$(TARGET2): $(OBJS2)
	mkdir -p build
	$(CC) $(CFLAGS) -o build/$@ $^

# Rule to compile the source files into object files
build/%.o: src/%.c
	mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to clean the build files
clean:
	rm -f $(OBJS1) build/$(TARGET1) $(OBJS2) build/$(TARGET2)
	rm -rf build

# Phony targets
.PHONY: all clean

