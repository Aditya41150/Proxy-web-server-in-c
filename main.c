#include "proxy.h"

int main(int argc, char *argv[]) {
    int port = -1;
    int use_cache = 0;
    int server_sock;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len;
    int client_sock;
    pthread_t thread;
    Cache cache;
    
    /* Parse command line arguments */
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) {
            port = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-c") == 0) {
            use_cache = 1;
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        }
    }
    
    /* Validate port */
    if (port < 1 || port > 65535) {
        fprintf(stderr, "Error: Invalid port number. Must be between 1 and 65535.\n");
        print_usage(argv[0]);
        return 1;
    }
    
    /* Initialize cache if enabled */
    if (use_cache) {
        cache_init(&cache);
        printf("[*] Caching enabled (capacity: %d entries)\n", CACHE_SIZE);
    }
    
    /* Setup server socket */
    server_sock = setup_server_socket(port);
    if (server_sock < 0) {
        return 1;
    }
    
    printf("[*] Proxy webserver started. Press Ctrl+C to stop.\n");
    printf("[*] Configuration:\n");
    printf("    - Listen port: %d\n", port);
    printf("    - Caching: %s\n", use_cache ? "enabled" : "disabled");
    printf("    - Max clients: %d\n", MAX_CLIENTS);
    printf("    - Socket timeout: %d seconds\n", TIMEOUT_SEC);
    printf("    - Buffer size: %d bytes\n\n", BUFFER_SIZE);
    
    /* Accept and handle client connections */
    while (1) {
        client_addr_len = sizeof(client_addr);
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_addr_len);
        
        if (client_sock < 0) {
            perror("accept");
            continue;
        }
        
        /* Log client connection */
        printf("[+] New client connection from %s:%d\n", 
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        
        /* Create thread to handle client */
        ClientArgs *args = malloc(sizeof(ClientArgs));
        if (!args) {
            perror("malloc");
            close(client_sock);
            continue;
        }
        
        args->client_sock = client_sock;
        args->cache = use_cache ? &cache : NULL;
        args->use_cache = use_cache;
        
        if (pthread_create(&thread, NULL, thread_handler, args) != 0) {
            perror("pthread_create");
            free(args);
            close(client_sock);
            continue;
        }
        
        /* Detach thread so resources are freed automatically */
        pthread_detach(thread);
    }
    
    /* Cleanup (never reached unless killed) */
    if (use_cache) {
        cache_cleanup(&cache);
    }
    close(server_sock);
    
    return 0;
}
