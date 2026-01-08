#include "../include/proxy.h"

/* Handle individual client connection */
void handle_client(void *arg) {
  ClientArgs *args = (ClientArgs *)arg;
  int client_sock = args->client_sock;
  Cache *cache = args->cache;
  int use_cache = args->use_cache;

  char buffer[BUFFER_SIZE];
  int n;

  /* Receive request from client */
  n = recv(client_sock, buffer, BUFFER_SIZE - 1, 0);
  if (n <= 0) {
    close(client_sock);
    free(args);
    return;
  }

  buffer[n] = '\0';
  printf("[*] Received request from client:\n%.100s...\n", buffer);

  /* Parse request */
  HttpRequest *req = parse_request(buffer);
  if (!req || strlen(req->host) == 0) {
    const char *error = "HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\n\r\n";
    send(client_sock, error, strlen(error), 0);
    free_request(req);
    close(client_sock);
    free(args);
    return;
  }

  /* Check cache first */
  char cache_key[1024];
  snprintf(cache_key, sizeof(cache_key), "%s:%d%s", req->host, req->port,
           req->path);

  char *cached_response = NULL;
  size_t cached_size = 0;

  if (use_cache &&
      cache_get(cache, cache_key, &cached_response, &cached_size)) {
    printf("[*] Cache hit for %s\n", cache_key);
    send(client_sock, cached_response, cached_size, 0);
    free(cached_response); /* Free the copy returned by cache_get */
    free_request(req);
    close(client_sock);
    free(args);
    return;
  }

  /* Connect to remote server */
  int server_sock = connect_to_server(req->host, req->port);
  if (server_sock < 0) {
    const char *error = "HTTP/1.1 502 Bad Gateway\r\nContent-Length: 0\r\n\r\n";
    send(client_sock, error, strlen(error), 0);
    free_request(req);
    close(client_sock);
    free(args);
    return;
  }

  /* Send request to server */
  if (send_request_to_server(server_sock, req) < 0) {
    const char *error = "HTTP/1.1 502 Bad Gateway\r\nContent-Length: 0\r\n\r\n";
    send(client_sock, error, strlen(error), 0);
    free_request(req);
    close(server_sock);
    close(client_sock);
    free(args);
    return;
  }

  /* Receive response from server */
  char *response = NULL;
  size_t response_size = 0;

  if (receive_response(server_sock, &response, &response_size) < 0) {
    const char *error = "HTTP/1.1 502 Bad Gateway\r\nContent-Length: 0\r\n\r\n";
    send(client_sock, error, strlen(error), 0);
    free_request(req);
    close(server_sock);
    close(client_sock);
    free(args);
    return;
  }

  /* Cache the response if enabled */
  if (use_cache && response_size > 0) {
    cache_add(cache, cache_key, response, response_size);
  }

  /* Send response to client */
  send(client_sock, response, response_size, 0);

  /* Cleanup */
  free_request(req);
  free(response);
  close(server_sock);
  close(client_sock);
  free(args);
}

/* Thread handler wrapper */
void *thread_handler(void *arg) {
  handle_client(arg);
  pthread_exit(NULL);
}

/* Setup server socket */
int setup_server_socket(int port) {
  int server_sock;
  struct sockaddr_in server_addr;
  int opt = 1;

  server_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (server_sock < 0)
    return -1;

  setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(port);

  if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) <
      0) {
    close(server_sock);
    return -1;
  }

  if (listen(server_sock, MAX_CLIENTS) < 0) {
    close(server_sock);
    return -1;
  }

  printf("[*] Proxy server listening on port %d\n", port);
  return server_sock;
}
