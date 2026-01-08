#include "../include/proxy.h"

/* Parse HTTP request from raw data */
HttpRequest *parse_request(const char *raw_request) {
  HttpRequest *req = malloc(sizeof(HttpRequest));
  if (!req)
    return NULL;

  memset(req, 0, sizeof(HttpRequest));

  char *copy = strdup(raw_request);
  if (!copy) {
    free(req);
    return NULL;
  }

  /* Parse request line */
  char *saveptr;
  char *line = strtok_r(copy, "\r\n", &saveptr);
  if (!line) {
    free(copy);
    free(req);
    return NULL;
  }

  /* Use field widths to prevent buffer overflow */
  sscanf(line, "%15s %511s", req->method, req->path);

  /* Parse headers */
  size_t headers_capacity = 1024;
  req->headers = malloc(headers_capacity);
  if (!req->headers) {
    free(copy);
    free(req);
    return NULL;
  }
  req->headers_len = 0;

  req->port = 80; /* Default HTTP port */

  while ((line = strtok_r(NULL, "\r\n", &saveptr)) != NULL &&
         strlen(line) > 0) {
    if (strncasecmp(line, "Host:", 5) == 0) {
      char *host_start = line + 5;
      while (*host_start == ' ')
        host_start++;

      char *colon = strchr(host_start, ':');
      if (colon) {
        int len = colon - host_start;
        strncpy(req->host, host_start,
                len < MAX_HOST_LEN ? len : MAX_HOST_LEN - 1);
        req->host[len < MAX_HOST_LEN ? len : MAX_HOST_LEN - 1] = '\0';
        sscanf(colon + 1, "%d", &req->port);
      } else {
        strncpy(req->host, host_start, MAX_HOST_LEN - 1);
        req->host[MAX_HOST_LEN - 1] = '\0';
      }
    }

    /* Store headers */
    size_t line_len = strlen(line) + 2;
    if (req->headers_len + line_len >= headers_capacity) {
      headers_capacity *= 2;
      char *new_headers = realloc(req->headers, headers_capacity);
      if (!new_headers) {
        free(copy);
        free(req->headers);
        free(req);
        return NULL;
      }
      req->headers = new_headers;
    }

    strcpy(req->headers + req->headers_len, line);
    req->headers_len += strlen(line);
    strcpy(req->headers + req->headers_len, "\r\n");
    req->headers_len += 2;
  }

  free(copy);
  return req;
}

/* Free HTTP request */
void free_request(HttpRequest *req) {
  if (req) {
    if (req->headers)
      free(req->headers);
    if (req->body)
      free(req->body);
    free(req);
  }
}

/* Connect to remote server */
int connect_to_server(const char *host, int port) {
  struct sockaddr_in server_addr;
  struct hostent *he;
  int sock;

  /* Resolve hostname */
  he = gethostbyname(host);
  if (!he) {
    perror("gethostbyname");
    return -1;
  }

  /* Create socket */
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    perror("socket");
    return -1;
  }

  /* Set socket timeout */
  struct timeval tv;
  tv.tv_sec = TIMEOUT_SEC;
  tv.tv_usec = 0;
  setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
  setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));

  /* Connect */
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  memcpy(&server_addr.sin_addr, he->h_addr_list[0], he->h_length);

  if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    perror("connect");
    close(sock);
    return -1;
  }

  return sock;
}

/* Send request to server */
int send_request_to_server(int server_sock, HttpRequest *req) {
  char buffer[4096];
  int n;

  snprintf(buffer, sizeof(buffer), "%s %s HTTP/1.1\r\n", req->method,
           req->path);
  n = send(server_sock, buffer, strlen(buffer), 0);
  if (n < 0)
    return -1;

  snprintf(buffer, sizeof(buffer), "Host: %s:%d\r\n", req->host, req->port);
  n = send(server_sock, buffer, strlen(buffer), 0);
  if (n < 0)
    return -1;

  const char *conn = "Connection: close\r\n";
  send(server_sock, conn, strlen(conn), 0);

  if (req->headers_len > 0) {
    send(server_sock, req->headers, req->headers_len, 0);
  }

  const char *blank = "\r\n";
  send(server_sock, blank, strlen(blank), 0);

  if (req->body_len > 0) {
    send(server_sock, req->body, req->body_len, 0);
  }

  return 0;
}

/* Receive response from server */
int receive_response(int server_sock, char **response, size_t *size) {
  char buffer[BUFFER_SIZE];
  int n;
  size_t total_size = 0;
  size_t capacity = BUFFER_SIZE * 4;

  *response = malloc(capacity);
  if (!*response)
    return -1;

  while ((n = recv(server_sock, buffer, BUFFER_SIZE, 0)) > 0) {
    if (total_size + n >= capacity) {
      capacity *= 2;
      char *new_res = realloc(*response, capacity);
      if (!new_res) {
        free(*response);
        return -1;
      }
      *response = new_res;
    }
    memcpy(*response + total_size, buffer, n);
    total_size += n;
  }

  *size = total_size;
  return 0;
}

/* Cache initialization */
void cache_init(Cache *cache) {
  memset(cache, 0, sizeof(Cache));
  pthread_mutex_init(&cache->lock, NULL);
}

/* Add entry to cache */
void cache_add(Cache *cache, const char *url, const char *response,
               size_t size) {
  if (size > MAX_CACHE_ENTRY_SIZE)
    return;

  pthread_mutex_lock(&cache->lock);

  if (cache->count >= CACHE_SIZE) {
    time_t oldest = cache->entries[0].timestamp;
    int oldest_idx = 0;
    for (int i = 1; i < CACHE_SIZE; i++) {
      if (cache->entries[i].timestamp < oldest) {
        oldest = cache->entries[i].timestamp;
        oldest_idx = i;
      }
    }
    if (cache->entries[oldest_idx].response)
      free(cache->entries[oldest_idx].response);
    cache->count--;
  }

  CacheEntry *entry = &cache->entries[cache->count];
  strncpy(entry->url, url, sizeof(entry->url) - 1);
  entry->url[sizeof(entry->url) - 1] = '\0';

  entry->response = malloc(size);
  if (entry->response) {
    memcpy(entry->response, response, size);
    entry->response_size = size;
    entry->timestamp = time(NULL);
    entry->hits = 0;
    cache->count++;
  }

  pthread_mutex_unlock(&cache->lock);
}

/* Get entry from cache (returns a copy of the response that must be freed by
 * caller) */
int cache_get(Cache *cache, const char *url, char **response, size_t *size) {
  pthread_mutex_lock(&cache->lock);
  for (int i = 0; i < cache->count; i++) {
    if (strcmp(cache->entries[i].url, url) == 0) {
      *size = cache->entries[i].response_size;
      *response = malloc(*size);
      if (*response) {
        memcpy(*response, cache->entries[i].response, *size);
        cache->entries[i].hits++;
        pthread_mutex_unlock(&cache->lock);
        return 1;
      }
      break;
    }
  }
  pthread_mutex_unlock(&cache->lock);
  return 0;
}

/* Cleanup cache */
void cache_cleanup(Cache *cache) {
  pthread_mutex_lock(&cache->lock);
  for (int i = 0; i < cache->count; i++) {
    if (cache->entries[i].response)
      free(cache->entries[i].response);
  }
  pthread_mutex_unlock(&cache->lock);
  pthread_mutex_destroy(&cache->lock);
}

/* Print usage information */
void print_usage(const char *program_name) {
  printf("Usage: %s -p <port> [-c]\n", program_name);
}
