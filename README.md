# Proxy Webserver in C

A high-performance HTTP proxy server written in C with support for multithreading, response caching, and connection pooling.

## Features

- **HTTP/1.1 Proxy**: Forward HTTP requests to remote servers
- **Multithreaded**: Handle multiple client connections concurrently using POSIX threads
- **Response Caching**: Optional caching of responses to reduce server load
- **Connection Management**: Automatic timeout and error handling
- **Configurable**: Command-line options for port and caching
- **Robust Error Handling**: Graceful handling of network errors and timeouts

## Architecture

### Components

- **proxy.h**: Header file with data structures and function declarations
- **proxy.c**: Core proxy logic including:
  - HTTP request parsing
  - Server connection management
  - Response handling
  - Caching mechanism
- **server.c**: Socket and threading operations
- **main.c**: Entry point and connection handler
- **Makefile**: Build configuration

### Data Structures

- **HttpRequest**: Parsed HTTP request with method, path, host, and headers
- **CacheEntry**: Individual cache entry with URL, response, and metadata
- **Cache**: Thread-safe cache container with mutex protection
- **ClientArgs**: Arguments passed to client handler threads

## Building

### Prerequisites

- GCC compiler
- POSIX-compliant system (Linux, macOS, etc.)
- Standard C library with threading support

### Compilation

```bash
# Build the project
make

# Build with debug symbols
make debug

# Clean build artifacts
make clean
```

## Usage

```bash
# Run with caching enabled on port 8080
./proxy_server -p 8080 -c

# Run without caching on default port
./proxy_server -p 3000

# Show help
./proxy_server -h
```

### Command Line Options

- `-p <port>`: Port number to listen on (required)
- `-c`: Enable response caching (optional)
- `-h, --help`: Show help message

## Configuration Constants

Edit `proxy.h` to adjust:

- `MAX_CLIENTS`: Maximum concurrent client connections (default: 1024)
- `BUFFER_SIZE`: Size of network buffers (default: 8192 bytes)
- `TIMEOUT_SEC`: Socket timeout duration (default: 30 seconds)
- `CACHE_SIZE`: Maximum number of cached entries (default: 100)
- `MAX_CACHE_ENTRY_SIZE`: Maximum size of a single cache entry (default: 64KB)

## How It Works

### Request Flow

1. Client connects to proxy on specified port
2. Proxy receives HTTP request from client
3. Proxy parses request to extract destination host and path
4. Proxy checks cache (if enabled) for matching response
5. If cached, response is immediately returned to client
6. Otherwise, proxy connects to remote server
7. Proxy forwards request to remote server
8. Proxy receives response from remote server
9. Proxy caches response (if enabled)
10. Proxy sends response to client

### Multithreading

- Each client connection is handled by a dedicated thread
- Main thread continuously accepts new connections
- Client handler threads are detached for automatic resource cleanup
- Thread-safe cache with mutex locks for concurrent access

### Caching

- URL-based cache key (host:port + path)
- LRU-like replacement: oldest entry removed when cache is full
- Cache entries track hit count and timestamp
- Thread-safe operations with mutex protection

## Testing

### Using curl

```bash
# In one terminal, start the proxy
./proxy_server -p 8080 -c

# In another terminal, test with curl
curl -x http://localhost:8080 http://example.com

# Test multiple requests (second request should be cached)
curl -x http://localhost:8080 http://example.com/page
curl -x http://localhost:8080 http://example.com/page
```

### Using wget

```bash
wget -e use_proxy=yes -e http_proxy=localhost:8080 http://example.com
```

## Performance Considerations

- **Buffering**: Default 8KB per connection; adjust for large files
- **Caching**: Disabled by default; enable with `-c` for better performance
- **Threads**: Each connection creates a thread; limit with `MAX_CLIENTS`
- **Timeouts**: 30-second default; adjust based on network conditions

## Limitations

- HTTP only (no HTTPS/SSL support)
- No persistent connection pooling to origin servers
- No request/response body handling for certain content types
- No proxy authentication support
- Memory-based caching only (no disk persistence)

## Future Enhancements

- HTTPS/SSL/TLS support with OpenSSL
- Connection pooling to origin servers
- Configurable cache expiration policies
- Request rate limiting
- Access logging to file
- Proxy authentication (Basic, Digest)
- Support for CONNECT method (for HTTPS tunneling)
- Load balancing across multiple backends

## Error Handling

The proxy handles various error conditions:

- **400 Bad Request**: Malformed HTTP requests
- **502 Bad Gateway**: Cannot connect to remote server or network error
- **504 Gateway Timeout**: Connection timeout while communicating with server

## System Requirements

- **OS**: Linux or other POSIX-compliant system
- **Architecture**: x86_64 recommended
- **RAM**: 10MB+ for typical usage
- **Network**: Standard TCP/IP stack

## License

This project is provided as-is for educational and commercial use.

## Author

Created as a complete C implementation of a functional HTTP proxy server.
