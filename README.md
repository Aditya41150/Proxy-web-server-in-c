# 🌐 Multithreaded Proxy Webserver in C

A high-performance, multithreaded HTTP proxy server written in C. This project demonstrates advanced systems programming concepts including socket programming, POSIX threads, synchronization primitives, and automated response caching.

![Build Status](https://img.shields.io/badge/build-passing-brightgreen)
![C Version](https://img.shields.io/badge/C-C11-blue)
![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20WSL-lightgrey)

---

## 🚀 Features

- **Multithreaded Architecture**: Uses a "thread-per-request" model to handle hundreds of concurrent client connections efficiently.
- **HTTP/1.1 Compliant**: Forwards method, path, and headers to remote servers, maintaining protocol integrity.
- **Intelligent Caching**: Implements a thread-safe LRU (Least Recently Used) caching mechanism to reduce latency for frequently accessed resources.
- **Graceful Shutdown**: Robust signal handling for `SIGINT` (Ctrl+C) ensuring all sockets are closed and memory is freed correctly.
- **Dynamic Configuration**: Easy-to-use command line interface to specify ports and enable/disable features.

---

## 🛠 Project Structure

```text
.
├── src/
│   ├── main.c          # Entry point and server loop
│   ├── proxy.c         # Caching and request parsing logic
│   └── server.c        # Socket handling and thread management
├── include/
│   └── proxy.h         # Global definitions and function prototypes
├── Makefile            # Build configuration
├── CMakeLists.txt      # Modern CMake build configuration
└── test.sh             # Automated testing script
```

---

## 📦 Getting Started

### Prerequisites

Ensure you have a POSIX-compliant environment (Linux, macOS, or WSL) and theFollowing tools installed:
- `gcc` or `clang`
- `make` or `cmake`
- `curl` (for testing)

### Build Instructions

#### Using Make (Recommended)
```bash
# Clean previous builds
make clean

# Compile the project
make
```

#### Using CMake
```bash
mkdir build && cd build
cmake ..
make
```

### Running the Server

Start the proxy server by specifying a port:

```bash
# Run with caching enabled on port 8080
./proxy_server -p 8080 -c

# Run without caching on port 9000
./proxy_server -p 9000
```

---

## 🔍 Testing & Usage

### Command Line Flags
- `-p <port>`: **Required**. Port on which the proxy will listen.
- `-c`: **Optional**. Enable response caching.
- `-h/--help`: Display help message.

### Automated Testing
You can run the included test script to verify the build and basic functionality:
```bash
chmod +x test.sh
./test.sh
```

### Manual Testing with Curl
To route your requests through the proxy:
```bash
curl -x http://localhost:8080 http://example.com
```

---

## 🧠 Technical Deep Dive

### 1. The Threading Model
The server uses a master loop that accepts new connections. For every connection, a new detached POSIX thread is created. This allows the server to remain responsive while waiting for remote server responses.

### 2. Caching Strategy
The cache is implemented as an array of entries protected by a `pthread_mutex`. When a request is made, the proxy generates a key based on `Host + Path`. 
- **Cache Hit**: Returns the response immediately from memory.
- **Cache Miss**: Fetches from the remote server, serves the client, and updates the cache.
- **Eviction**: When the cache is full, the entry with the oldest timestamp is evicted to make room for new data.

### 3. Memory Safety
The project uses `strtok_r` for thread-safe string parsing and ensures all dynamically allocated memory (requests, responses, thread arguments) is properly freed, even in error paths.

---

## 📜 License

This project is licensed under the MIT License - see the LICENSE file for details.

---
*Created with ❤️ by Aditya (Aditya41150)*
