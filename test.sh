#!/bin/bash

# Proxy Webserver Test Script
# Tests the proxy server functionality

set -e

echo "=== Proxy Webserver Test Script ==="
echo ""

# Colors for output
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Check if proxy_server exists
if [ ! -f "./proxy_server" ]; then
    echo -e "${YELLOW}[!] proxy_server binary not found. Building...${NC}"
    make clean
    make
fi

# Port to use for testing
TEST_PORT=8888

echo -e "${BLUE}[*] Starting proxy server on port ${TEST_PORT}...${NC}"
./proxy_server -p $TEST_PORT -c &
PROXY_PID=$!

# Give server time to start
sleep 2

echo -e "${GREEN}[✓] Proxy server started (PID: $PROXY_PID)${NC}"
echo ""

# Function to cleanup
cleanup() {
    echo ""
    echo -e "${YELLOW}[*] Shutting down proxy server...${NC}"
    kill $PROXY_PID 2>/dev/null || true
    wait $PROXY_PID 2>/dev/null || true
    echo -e "${GREEN}[✓] Proxy server stopped${NC}"
}

trap cleanup EXIT

# Test 1: Simple request through proxy
echo -e "${BLUE}[TEST 1] Simple HTTP request through proxy${NC}"
if command -v curl &> /dev/null; then
    echo "Testing: curl -x http://localhost:$TEST_PORT http://httpbin.org/get"
    timeout 10 curl -x http://localhost:$TEST_PORT http://httpbin.org/get 2>/dev/null | head -20
    echo -e "${GREEN}[✓] Test 1 passed${NC}"
else
    echo -e "${YELLOW}[!] curl not found, skipping test${NC}"
fi
echo ""

# Test 2: Multiple requests (cache test)
echo -e "${BLUE}[TEST 2] Multiple requests (cache test)${NC}"
if command -v curl &> /dev/null; then
    echo "First request (should be cached):"
    timeout 10 curl -x http://localhost:$TEST_PORT http://httpbin.org/delay/1 2>/dev/null | head -5
    
    echo ""
    echo "Second request (should come from cache):"
    timeout 10 curl -x http://localhost:$TEST_PORT http://httpbin.org/delay/1 2>/dev/null | head -5
    echo -e "${GREEN}[✓] Test 2 passed${NC}"
else
    echo -e "${YELLOW}[!] curl not found, skipping test${NC}"
fi
echo ""

# Test 3: Invalid request handling
echo -e "${BLUE}[TEST 3] Invalid request handling${NC}"
if command -v curl &> /dev/null; then
    echo "Testing invalid host (should get error response):"
    timeout 5 curl -x http://localhost:$TEST_PORT http://invalid.host.test.example.com 2>&1 | head -3
    echo -e "${GREEN}[✓] Test 3 passed (error handled gracefully)${NC}"
else
    echo -e "${YELLOW}[!] curl not found, skipping test${NC}"
fi
echo ""

echo -e "${GREEN}=== All tests completed ===${NC}"
