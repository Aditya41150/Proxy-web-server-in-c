#!/usr/bin/env bash

# Proxy Server Usage Examples
# This file demonstrates various ways to use the proxy server

echo "=== Proxy Webserver Usage Examples ==="
echo ""

# Color codes
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m'

# Example 1: Build and run
echo -e "${BLUE}Example 1: Build and Run${NC}"
echo "$ cd /home/acer/proxy-webserver-c"
echo "$ make"
echo "$ ./proxy_server -p 8080 -c"
echo ""

# Example 2: Using curl with proxy
echo -e "${BLUE}Example 2: Using curl with the proxy${NC}"
echo "$ curl -x http://localhost:8080 http://example.com"
echo "$ curl -x http://localhost:8080 http://httpbin.org/get"
echo "$ curl -x http://localhost:8080 http://httpbin.org/ip"
echo ""

# Example 3: Using wget with proxy
echo -e "${BLUE}Example 3: Using wget with the proxy${NC}"
echo "$ wget -e use_proxy=yes -e http_proxy=localhost:8080 http://example.com"
echo ""

# Example 4: Testing multiple requests (caching)
echo -e "${BLUE}Example 4: Testing cache with multiple requests${NC}"
echo "# First request - fetched from origin server"
echo "$ curl -x http://localhost:8080 http://example.com/page"
echo ""
echo "# Second request - same URL, should be cached"
echo "$ curl -x http://localhost:8080 http://example.com/page"
echo ""

# Example 5: Browser configuration
echo -e "${BLUE}Example 5: Configure browser to use proxy${NC}"
echo ""
echo "Chrome/Chromium:"
echo "  Settings > Advanced > System > Open Proxy Settings"
echo "  Set HTTP Proxy: localhost:8080"
echo ""
echo "Firefox:"
echo "  Settings > General > Network Settings"
echo "  Manual proxy configuration"
echo "  HTTP Proxy: localhost, Port: 8080"
echo ""
echo "Chrome with command line:"
echo '  google-chrome --proxy-server="http://localhost:8080"'
echo ""

# Example 6: Performance testing
echo -e "${BLUE}Example 6: Performance testing${NC}"
echo "# Test response time with caching"
echo "$ time curl -x http://localhost:8080 http://example.com"
echo "$ time curl -x http://localhost:8080 http://example.com  # Should be faster"
echo ""

# Example 7: Advanced configuration
echo -e "${BLUE}Example 7: Advanced configuration${NC}"
echo ""
echo "To change proxy parameters, edit proxy.h:"
echo ""
echo "Configuration options:"
echo "  MAX_CLIENTS              - Maximum concurrent connections (default: 1024)"
echo "  BUFFER_SIZE              - Network buffer size (default: 8192 bytes)"
echo "  TIMEOUT_SEC              - Socket timeout (default: 30 seconds)"
echo "  CACHE_SIZE               - Number of cache entries (default: 100)"
echo "  MAX_CACHE_ENTRY_SIZE     - Max response size to cache (default: 64KB)"
echo ""
echo "Example: To increase cache size and buffer:"
echo "  1. Edit proxy.h"
echo "  2. Change CACHE_SIZE to 500"
echo "  3. Change BUFFER_SIZE to 16384"
echo "  4. Run: make clean && make"
echo ""

# Example 8: Different ports
echo -e "${BLUE}Example 8: Running on different ports${NC}"
echo "# High-numbered port (no root needed)"
echo "$ ./proxy_server -p 8080 -c"
echo ""
echo "# Another port"
echo "$ ./proxy_server -p 3000 -c"
echo ""
echo "# Standard HTTP port (requires sudo)"
echo "$ sudo ./proxy_server -p 80"
echo ""

# Example 9: Error conditions
echo -e "${BLUE}Example 9: Handling error conditions${NC}"
echo ""
echo "Invalid host (returns 502 Bad Gateway):"
echo "$ curl -x http://localhost:8080 http://invalid-host-that-does-not-exist.test"
echo ""
echo "Malformed request (returns 400 Bad Request):"
echo "$ nc localhost 8080"
echo "  invalid request"
echo ""

# Example 10: Monitoring proxy
echo -e "${BLUE}Example 10: Monitoring the proxy server${NC}"
echo ""
echo "Check if running:"
echo "$ ps aux | grep proxy_server"
echo ""
echo "Check connections:"
echo "$ netstat -tlnp | grep proxy_server"
echo ""
echo "Kill proxy gracefully:"
echo "$ killall proxy_server"
echo ""
echo "Kill running proxy and restart:"
echo "$ pkill -f proxy_server && ./proxy_server -p 8080 -c"
echo ""

echo -e "${GREEN}=== For more details, see README.md and QUICKSTART.md ===${NC}"
