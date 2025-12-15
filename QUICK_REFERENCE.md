# Proxy Webserver - Quick Reference

## 🚀 Getting Started (30 seconds)

```bash
cd /home/acer/proxy-webserver-c
make
./proxy_server -p 8080 -c
```

In another terminal:
```bash
curl -x http://localhost:8080 http://example.com
```

## 📋 Command Reference

| Command | Purpose |
|---------|---------|
| `make` | Build the proxy server |
| `make run` | Build and run with caching on port 8080 |
| `make run-no-cache` | Build and run without caching |
| `make debug` | Build with debug symbols (-g, -O0) |
| `make clean` | Remove build artifacts |
| `./proxy_server -p PORT` | Run on specific port |
| `./proxy_server -p PORT -c` | Run with caching enabled |
| `./proxy_server -h` | Show help message |

## 🌐 Client Usage

### curl
```bash
# Basic request
curl -x http://localhost:8080 http://example.com

# Verbose output
curl -v -x http://localhost:8080 http://example.com

# Follow redirects
curl -L -x http://localhost:8080 http://example.com

# With headers
curl -H "User-Agent: MyApp" -x http://localhost:8080 http://example.com
```

### wget
```bash
# Basic request
wget -e use_proxy=yes -e http_proxy=localhost:8080 http://example.com

# Save to file
wget -O output.html -e use_proxy=yes -e http_proxy=localhost:8080 http://example.com
```

### Browser
```bash
# Chrome with proxy
google-chrome --proxy-server="http://localhost:8080"

# Firefox Settings > Network Settings > Manual proxy
# HTTP Proxy: localhost, Port: 8080
```

## 🔧 Configuration

### Edit proxy.h for advanced settings:

```c
#define MAX_CLIENTS 1024              // Concurrent connections
#define BUFFER_SIZE 8192              // Network buffer
#define TIMEOUT_SEC 30                // Socket timeout
#define CACHE_SIZE 100                // Cache entries
#define MAX_CACHE_ENTRY_SIZE 65536    // Max response size
```

Then rebuild:
```bash
make clean
make
```

## 📊 Monitoring

```bash
# Check if running
ps aux | grep proxy_server

# Check port
netstat -tlnp | grep proxy_server

# Kill gracefully
killall proxy_server

# Kill and restart
pkill -f proxy_server && ./proxy_server -p 8080 -c
```

## 🐛 Troubleshooting

| Issue | Solution |
|-------|----------|
| Port in use | `lsof -i :8080` then `kill -9 <PID>` |
| Connection refused | Check if proxy is running: `ps aux \| grep proxy` |
| DNS failures | Test: `nslookup example.com` |
| Slow responses | Enable caching with `-c` flag |
| Too many clients | Increase `MAX_CLIENTS` in proxy.h |

## 📈 Performance Tips

1. **Enable caching**: `./proxy_server -p 8080 -c`
2. **Increase buffer**: Edit BUFFER_SIZE in proxy.h
3. **Tune timeout**: TIMEOUT_SEC for network conditions
4. **Increase cache**: CACHE_SIZE for popular content
5. **Monitor cache**: Check cache hit rates

## 🧪 Testing

```bash
# Automated tests
./test.sh

# Manual cache test (2nd request should be instant)
curl -x http://localhost:8080 http://example.com
curl -x http://localhost:8080 http://example.com

# Test error handling
curl -x http://localhost:8080 http://invalid-host.test
```

## 📁 Project Structure

```
proxy-webserver-c/
├── main.c              # Entry point
├── proxy.c             # Core logic
├── proxy.h             # Headers
├── server.c            # Threading
├── Makefile            # Build config
├── README.md           # Full docs
├── QUICKSTART.md       # Quick start
├── PROJECT_SUMMARY.md  # Summary
├── EXAMPLES.sh         # Examples
└── test.sh             # Tests
```

## 🎯 Common Scenarios

### Scenario 1: Personal Web Browsing
```bash
# Start proxy
./proxy_server -p 3128 -c

# Configure browser to use localhost:3128
# Browse normally through proxy
```

### Scenario 2: Application Testing
```bash
# Start proxy in terminal
./proxy_server -p 8080 -c

# Configure your app to use http://localhost:8080 as proxy
# Run your app and observe requests in proxy logs
```

### Scenario 3: Development Server
```bash
# Start development version with larger cache
# Edit proxy.h: CACHE_SIZE 500, BUFFER_SIZE 16384
make debug
./proxy_server -p 9000 -c
```

### Scenario 4: Performance Testing
```bash
# Run with caching
./proxy_server -p 8080 -c &

# Measure performance
time curl -x http://localhost:8080 http://example.com
time curl -x http://localhost:8080 http://example.com  # Should be faster
```

## 💡 Key Concepts

**Proxy**: Acts as intermediary between client and server
**Caching**: Stores responses to serve faster on repeat requests
**Multithreading**: Handles multiple clients simultaneously
**Port**: Network endpoint (8080, 3000, 8888, etc.)
**HTTP**: Protocol for web requests and responses

## 🔗 Quick Links

- [README.md](README.md) - Full documentation
- [QUICKSTART.md](QUICKSTART.md) - Quick start guide
- [PROJECT_SUMMARY.md](PROJECT_SUMMARY.md) - Project overview
- [EXAMPLES.sh](EXAMPLES.sh) - Usage examples
- [test.sh](test.sh) - Test script

## ⚡ One-Liners

```bash
# Start and test
./proxy_server -p 8080 -c & sleep 1 && curl -x http://localhost:8080 http://example.com && killall proxy_server

# Build, run, and test
make clean && make && ./proxy_server -p 8080 -c & sleep 1 && ./test.sh ; killall proxy_server

# Monitor with stats
while true; do echo "Connections: $(netstat -tlnp 2>/dev/null | grep proxy_server | wc -l)"; sleep 1; done
```

## 📚 Documentation Priority

1. **First time**: Read [QUICKSTART.md](QUICKSTART.md)
2. **Usage**: Check [EXAMPLES.sh](EXAMPLES.sh)
3. **Details**: Read [README.md](README.md)
4. **Overview**: See [PROJECT_SUMMARY.md](PROJECT_SUMMARY.md)

---

**Quick Build & Run**: `cd /home/acer/proxy-webserver-c && make && ./proxy_server -p 8080 -c`

**Default Port**: 8080
**With Caching**: ~5x faster response times for repeated requests
**Max Connections**: 1024 (configurable)
