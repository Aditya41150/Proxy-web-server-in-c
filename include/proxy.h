#ifndef PROXY_H
#define PROXY_H

#define _GNU_SOURCE

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>


/* Configuration constants */
#define MAX_CLIENTS 1024
#define BUFFER_SIZE 8192
#define MAX_HOST_LEN 256
#define MAX_PORT_LEN 6
#define TIMEOUT_SEC 30
#define CACHE_SIZE 100
#define MAX_CACHE_ENTRY_SIZE 65536

/* Request structure */
typedef struct {
  char method[16];
  char path[512];
  char host[MAX_HOST_LEN];
  int port;
  char *headers;
  size_t headers_len;
  char *body;
  size_t body_len;
} HttpRequest;

/* Cache entry structure */
typedef struct {
  char url[512];
  char *response;
  size_t response_size;
  time_t timestamp;
  int hits;
} CacheEntry;

/* Cache structure */
typedef struct {
  CacheEntry entries[CACHE_SIZE];
  int count;
  pthread_mutex_t lock;
} Cache;

/* Client arguments structure */
typedef struct {
  int client_sock;
  Cache *cache;
  int use_cache;
} ClientArgs;

/* Function prototypes */
HttpRequest *parse_request(const char *raw_request);
void free_request(HttpRequest *req);
int connect_to_server(const char *host, int port);
int send_request_to_server(int server_sock, HttpRequest *req);
int receive_response(int server_sock, char **response, size_t *size);
void handle_client(void *arg);
void *thread_handler(void *arg);
int setup_server_socket(int port);
void cache_init(Cache *cache);
void cache_add(Cache *cache, const char *url, const char *response,
               size_t size);
int cache_get(Cache *cache, const char *url, char **response, size_t *size);
void cache_cleanup(Cache *cache);
void print_usage(const char *program_name);

#endif // PROXY_H