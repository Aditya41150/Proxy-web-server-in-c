CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -pthread
LDFLAGS = -pthread

# Target executable
TARGET = proxy_server

# Source files
SOURCES = main.c proxy.c server.c
OBJECTS = $(SOURCES:.c=.o)

# Default target
all: $(TARGET)

# Build the executable
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "[✓] Build successful: $(TARGET)"

# Compile source files
%.o: %.c proxy.h
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(OBJECTS) $(TARGET)
	@echo "[✓] Clean complete"

# Run the proxy server
run: $(TARGET)
	./$(TARGET) -p 8080 -c

# Run without caching
run-no-cache: $(TARGET)
	./$(TARGET) -p 8080

# Debug build
debug: CFLAGS += -g -O0
debug: clean all

# Help
help:
	@echo "Available targets:"
	@echo "  all           - Build the proxy server"
	@echo "  run           - Build and run with caching enabled on port 8080"
	@echo "  run-no-cache  - Build and run without caching on port 8080"
	@echo "  debug         - Build with debug symbols"
	@echo "  clean         - Remove build artifacts"
	@echo "  help          - Show this help message"

.PHONY: all run run-no-cache debug clean help
