CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -pthread -Iinclude
LDFLAGS = -pthread

# Target executable
TARGET = proxy_server

# Source files
SOURCES = src/main.c src/proxy.c src/server.c
OBJECTS = $(SOURCES:.c=.o)

# Default target
all: $(TARGET)

# Build the executable
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "[✓] Build successful: $(TARGET)"

# Compile source files
%.o: %.c include/proxy.h
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(OBJECTS) $(TARGET)
	@echo "[✓] Clean complete"

# Run the proxy server
run: $(TARGET)
	./$(TARGET) -p 8080 -c

# Help
help:
	@echo "Available targets:"
	@echo "  all           - Build the proxy server"
	@echo "  run           - Build and run with caching enabled on port 8080"
	@echo "  clean         - Remove build artifacts"
	@echo "  help          - Show this help message"

.PHONY: all run clean help
