/*
# Copyright 2025 University of Kentucky
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# SPDX-License-Identifier: Apache-2.0
*/

/* 
Please specify the group members here

# Student #1: Fernando C. Tanase Mosneagu
# Student #2:
# Student #3: 

*/
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <pthread.h>

#define MAX_EVENTS 64
#define MESSAGE_SIZE 16
#define DEFAULT_CLIENT_THREADS 4

char *server_ip = "127.0.0.1";
int server_port = 12345;
int num_client_threads = DEFAULT_CLIENT_THREADS;
int num_requests = 1000000;

/*
 * This structure is used to store per-thread data in the client
 */
typedef struct {
    int epoll_fd;        /* File descriptor for the epoll instance, used for monitoring events on the socket. */
    int socket_fd;       /* File descriptor for the client socket connected to the server. */
    long long total_rtt; /* Accumulated Round-Trip Time (RTT) for all messages sent and received (in microseconds). */
    long total_messages; /* Total number of messages sent and received. */
    float request_rate;  /* Computed request rate (requests per second) based on RTT and total messages. */
} client_thread_data_t;

/*
 * This function runs in a separate client thread to handle communication with the server
 */
void *client_thread_func(void *arg) {
    client_thread_data_t *data = (client_thread_data_t *)arg;
    struct epoll_event event, events[MAX_EVENTS];
    char send_buf[MESSAGE_SIZE] = "ABCDEFGHIJKMLNOP"; /* Send 16-Bytes message every time */
    char recv_buf[MESSAGE_SIZE];
    struct timeval start, end;

    // Hint 1: register the "connected" client_thread's socket in the its epoll instance
    // Hint 2: use gettimeofday() and "struct timeval start, end" to record timestamp, which can be used to calculated RTT.

    /* TODO:
     * It sends messages to the server, waits for a response using epoll,
     * and measures the round-trip time (RTT) of this request-response.
     */
 
    /* TODO:
     * The function exits after sending and receiving a predefined number of messages (num_requests). 
     * It calculates the request rate based on total messages and RTT
     */

    return NULL;
}

/*
 * This function orchestrates multiple client threads to send requests to a server,
 * collect performance data of each threads, and compute aggregated metrics of all threads.
 */
void run_client() {
    pthread_t threads[num_client_threads];
    client_thread_data_t thread_data[num_client_threads];
    struct sockaddr_in server_addr;

    /* TODO:
     * Create sockets and epoll instances for client threads
     * and connect these sockets of client threads to the server
     */
    
    // Hint: use thread_data to save the created socket and epoll instance for each thread
    // You will pass the thread_data to pthread_create() as below
    for (int i = 0; i < num_client_threads; i++) {
        pthread_create(&threads[i], NULL, client_thread_func, &thread_data[i]);
    }

    /* TODO:
     * Wait for client threads to complete and aggregate metrics of all client threads
     */

    //printf("Average RTT: %lld us\n", total_rtt / total_messages);
    //printf("Total Request Rate: %f messages/s\n", total_request_rate);
}

void run_server() {

    /* TODO:
     * Server creates listening socket and epoll instance.
     * Server registers the listening socket to epoll
     */

    // Create listening socket.
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Listening Socket creation failed");
        exit(EXIT_FAILURE);
    }
    // Make socket non-blocking
    fcntl(server_socket, F_SETFL, O_NONBLOCK);
    //Bind the socket to the server address and port
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    server_addr.sin_addr.s_addr = inet_addr(server_ip);
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
    // Listen for incoming connections
    if (listen(server_socket, DEFAULT_CLIENT_THREADS*2) == -1) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    // Create epoll instance
    int server_epoll_fd = epoll_create1(0);
    if (server_epoll_fd == -1) {
        perror("Epoll creation failed");
        exit(EXIT_FAILURE);
    }
    // Register the server socket to epoll in order to monitor incoming connections
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = server_socket;
    if (epoll_ctl(server_epoll_fd, EPOLL_CTL_ADD, server_socket, &event) == -1) {
        perror("Epoll_ctl failed");
        exit(EXIT_FAILURE);
    }

    /* Server's run-to-completion event loop */
    while (1) {
        /* TODO:
         * Server uses epoll to handle connection establishment with clients
         * or receive the message from clients and echo the message back
         */
        // 1. wait for new events
        struct epoll_event events[MAX_EVENTS];
        int num_events = epoll_wait(server_epoll_fd, events, MAX_EVENTS, -1);
        if (num_events == -1) {
            perror("Epoll wait failed");
            exit(EXIT_FAILURE);
        }
        // Process each event
        for (int i = 0; i < num_events; i++) {
            // 2. If it's the listening socket, accept new connection
            if (events[i].data.fd == server_socket) {
                struct sockaddr_in client_addr;
                socklen_t client_len = sizeof(client_addr);
                int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
                if (client_socket == -1) {
                    perror("Accept failed");
                    continue;
                }
            
                // Make the new client socket non-blocking
                fcntl(client_socket, F_SETFL, O_NONBLOCK);

                // 3. Add new client socket to epoll instance
                struct epoll_event ev;
                ev.events = EPOLLIN;
                ev.data.fd = client_socket;
                if (epoll_ctl(server_epoll_fd, EPOLL_CTL_ADD, client_socket, &ev) == -1) {
                    perror("Failed to add client socket to epoll");
                    close(client_socket);
                    continue;
                }
            }
            // else handle data from an already existing client
            else{
                // Read message from client socket into buffer.
                int client_socket = events[i].data.fd;
                char data_read[MESSAGE_SIZE];
                ssize_t bytes_read = read(client_socket, data_read, MESSAGE_SIZE);
                if (bytes_read == -1) {
                    perror("Failed to read data from client's socket");
                    close(client_socket);
                    epoll_ctl(server_epoll_fd, EPOLL_CTL_DEL, client_socket, NULL);
                    continue;
                } else if (bytes_read == 0) {
                    // Connection closed by client
                    close(client_socket);
                    epoll_ctl(server_epoll_fd, EPOLL_CTL_DEL, client_socket, NULL);
                    continue;
                }
                // Set epollout (so that we can echo the message)
                struct epoll_event ev;
                ev.events = EPOLLOUT;
                ev.data.fd = client_socket;
                if(epoll_ctl(server_epoll_fd, EPOLL_CTL_MOD, client_socket, &ev) == -1){
                    perror("Failed to set epollout");
                    close(client_socket);
                    epoll_ctl(server_epoll_fd, EPOLL_CTL_DEL, client_socket, NULL);
                    continue;
                }
                // Echo the message back.
                if(write(client_socket, data_read, MESSAGE_SIZE) == -1){
                    perror("Failed to echo message back to client");
                    close(client_socket);
                    epoll_ctl(server_epoll_fd, EPOLL_CTL_DEL, client_socket, NULL);
                    continue;
                }
                // Set epoll back to epollin so that server is ready to read again.
                ev.events = EPOLLIN;
                if(epoll_ctl(server_epoll_fd, EPOLL_CTL_MOD, client_socket, &ev) == -1){
                    perror("Failed to set epollin ");
                    close(client_socket);
                    epoll_ctl(server_epoll_fd, EPOLL_CTL_DEL, client_socket, NULL);
                    continue;
                }
            } 
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc > 1 && strcmp(argv[1], "server") == 0) {
        if (argc > 2) server_ip = argv[2];
        if (argc > 3) server_port = atoi(argv[3]);

        run_server();
    } else if (argc > 1 && strcmp(argv[1], "client") == 0) {
        if (argc > 2) server_ip = argv[2];
        if (argc > 3) server_port = atoi(argv[3]);
        if (argc > 4) num_client_threads = atoi(argv[4]);
        if (argc > 5) num_requests = atoi(argv[5]);

        run_client();
    } else {
        printf("Usage: %s <server|client> [server_ip server_port num_client_threads num_requests]\n", argv[0]);
    }

    return 0;
}