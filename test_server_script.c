#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

#define MESSAGE_SIZE 16

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(12345),
        .sin_addr.s_addr = inet_addr("127.0.0.1")
    };

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("Connect failed");
        close(sock);
        exit(EXIT_FAILURE);
    }
    
    char msg[MESSAGE_SIZE] = "ABCDEFGHIJKLMNOP";
    char buf[MESSAGE_SIZE];
    
    for(int i = 0; i < 5; i++) {
        printf("Sending message %d...\n", i+1);
        
        ssize_t bytes_written = write(sock, msg, MESSAGE_SIZE);
        if (bytes_written != MESSAGE_SIZE) {
            printf("Write error: %zd bytes written, expected %d\n", 
                   bytes_written, MESSAGE_SIZE);
            break;
        }

        ssize_t bytes_read = read(sock, buf, MESSAGE_SIZE);
        if (bytes_read != MESSAGE_SIZE) {
            printf("Read error: %zd bytes read, expected %d\n", 
                   bytes_read, MESSAGE_SIZE);
            break;
        }

        printf("Response %d: '%.*s'\n", i+1, MESSAGE_SIZE, buf);
        printf("Hex dump: ");
        for(int j = 0; j < MESSAGE_SIZE; j++) {
            printf("%02x ", (unsigned char)buf[j]);
        }
        printf("\n\n");
        
        sleep(1); // Add delay between messages
    }
    
    close(sock);
    return 0;
}