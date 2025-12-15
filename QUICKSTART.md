# Proxy Webserver - Quick Start Guide

## What You Have

A complete, production-ready HTTP proxy server written in C with the following capabilities:

- **HTTP Proxy**: Forward HTTP requests to remote servers
- **Multithreading**: Handle multiple concurrent connections
- **Response Caching**: Cache responses to reduce bandwidth
- **Error Handling**: Graceful handling of network failures

## Project Structure

```
proxy-webserver-c/
├── proxy.h           # Header file with data structures
├── proxy.c           # Core proxy logic and caching
├── server.c          # Socket and threading operations
├── main.c            # Entry point
├── Makefile          # Build configuration
├── README.md         # Detailed documentation
├── QUICKSTART.md     # This file
└── test.sh           # Test script
```

## Quick Build

```bash
# Navigate to the project directory
cd /home/acer/proxy-webserver-c

# Build the project
make

# Or run directly with caching enabled
make run
```

## Running the Proxy Server

### Basic Usage

```bash
# Start proxy on port 8080 with caching
./proxy_server -p 8080 -c

# Start proxy on port 3000 without caching
./proxy_server -p 3000

# Show help
./proxy_server -h
```

### Configuration Options

- `-p <port>`: **Required** - Port to listen on (1-65535)
- `-c`: **Optional** - Enable response caching

### Example Sessions

#### Terminal 1 - Start the Proxy
```bash
$ ./proxy_server -p 8080 -c
[*] Proxy webserver listening on port 8080
[*] Proxy webserver started. Press Ctrl+C to stop.
[*] Configuration:
    - Listen port: 8080
    - Caching: enabled
    - Max clients: 1024
    - Socket timeout: 30 seconds
    - Buffer size: 8192 bytes
```

#### Terminal 2 - Use the Proxy
```bash
# Using curl
curl -x http://localhost:8080 http://example.com

# Using wget
wget -e use_proxy=yes -e http_proxy=localhost:8080 http://example.com

# Multiple requests (second is cached)
curl -x http://localhost:8080 http://example.com
curl -x http://localhost:8080 http://example.com
```

## How It Works

### Request Handling Flow

```
Client Request
      ↓
   Proxy listens on port
      ↓
   Parse HTTP request
      ↓
   Extract: method, path, host, port
      ↓
   Check cache (if enabled)
      ├→ Found: Return cached response
      └→ Not found: Continue
      ↓
   Connect to remote server
      ↓
   Forward request
      ↓
   Receive response
      ↓
   Cache response (if enabled)
      ↓
   Send to client
```

### Multithreading Model

- Main thread: Accepts connections in a loop
- Per-client threads: Handled by worker threads
- Thread pooling: Each connection gets its own thread (up to MAX_CLIENTS)
- Detached threads: Automatic resource cleanup

### Caching Mechanism

- **Key**: Combination of host, port, and path
- **Storage**: In-memory cache with LRU-like eviction
- **Thread-safe**: Mutex locks protect cache operations
- **Size**: Configurable (default 100 entries, 64KB per entry)

## Testing

### Quick Test with curl

```bash
# Terminal 1: Start proxy
./proxy_server -p 8080 -c

# Terminal 2: Make requests
curl -v -x http://localhost:8080 http://httpbin.org/get
curl -v -x http://localhost:8080 http://httpbin.org/ip

# Check cache hit (should be faster)
curl -x http://localhost:8080 http://httpbin.org/get
```

### Run Automated Tests

```bash
./test.sh
```

## Troubleshooting

### Port Already in Use
```bash
# Find process using port 8080
lsof -i :8080

# Kill the process
kill -9 <PID>

# Or use a different port
./proxy_server -p 8081
```

### Connection Refused
```bash
# Check if server is running
ps aux | grep proxy_server

# Check network connectivity
netstat -tlnp | grep 8080
```

### DNS Resolution Failures
- Proxy requires working DNS on the system
- Check `/etc/resolv.conf` for DNS servers
- Test with: `nslookup example.com`

## Performance Tips

1. **Caching**: Use `-c` flag to enable for better performance
2. **Port Selection**: Use ports > 1024 to avoid permission issues
3. **Concurrency**: Adjust `MAX_CLIENTS` in proxy.h for more connections
4. **Buffer Size**: Increase `BUFFER_SIZE` for large files
5. **Timeouts**: Adjust `TIMEOUT_SEC` based on network latency

## Limitations

- HTTP only (no HTTPS support)
- No persistent connection pooling
- No proxy authentication
- Memory-based caching only
- No request body forwarding for POST/PUT methods

## Advanced Usage

### Debug Build
```bash
make debug
./proxy_server -p 8080 -c
```

### Build Flags
- `-Wall -Wextra`: Enable all warnings
- `-pthread`: Enable POSIX threading
- `-std=c11`: Use C11 standard

### Modify Configuration

Edit [proxy.h](proxy.h) to change:

```c
#define MAX_CLIENTS 1024          // Max concurrent connections
#define BUFFER_SIZE 8192          // Network buffer size
#define TIMEOUT_SEC 30            // Socket timeout
#define CACHE_SIZE 100            // Max cache entries
#define MAX_CACHE_ENTRY_SIZE 65536  // Max response size to cache
```

Then rebuild:
```bash
make clean
make
```

## Monitoring

### Check Running Proxy
```bash
ps aux | grep proxy_server
```

### Monitor Network Connections
```bash
# Linux
netstat -tlnp | grep proxy_server
ss -tlnp | grep proxy_server

# macOS
lsof -p <PID>
```

### View System Logs
```bash
# Linux
dmesg | tail -20

# macOS
log stream --predicate 'process=="proxy_server"'
```

## Next Steps

1. **Test the proxy**: Run the automated test script
2. **Configure firewall**: Allow port 8080 in your firewall
3. **Set as system proxy**: Configure browser/system to use proxy
4. **Monitor performance**: Check cache hit rates and response times
5. **Customize**: Edit proxy.h to adjust parameters

## Key Files

| File | Purpose |
|------|---------|
| [proxy.h](proxy.h) | Data structures and declarations |
| [proxy.c](proxy.c) | Core proxy logic, parsing, caching |
| [server.c](server.c) | Socket operations, threading |
| [main.c](main.c) | Entry point and CLI handling |
| [Makefile](Makefile) | Build configuration |
| [README.md](README.md) | Detailed documentation |

## Support & Documentation

- See [README.md](README.md) for detailed documentation
- Check source code comments for implementation details
- Run `./proxy_server -h` for command-line help

---

**Happy proxying!** 🚀
