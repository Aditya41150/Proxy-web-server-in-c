cmake_minimum_required(VERSION 3.15)
project(ProxyServer VERSION 1.0.0)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
SET(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SOURCES
    src/main.c
    src/proxy.c
    src/server.c
)

add_executable(proxy_server ${SOURCES})

target_include_directories(
    proxy_server PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)