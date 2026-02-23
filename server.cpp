#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main() {
    // intialise socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        std::cerr << "Failed to create socket\n";
        return 1;
    }
     // set socket options for reuse    
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        std::cerr << "setsockopt failed\n";
        return 1;
    }
     // set env variables for the server
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);
     // bind the socket to the address and port
    if (bind(server_fd, (sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Failed to bind socket\n";
        return 1;
    }
    // listen for incoming connections
    if (listen(server_fd, 10) < 0) {
        std::cerr << "Failed to listen on socket\n";
        return 1;
    }

    std::cout << "Server listening on port 8080...\n";
   
    // accept and handle incoming connections
    while (true) {
        socklen_t addrlen = sizeof(address);
        int client_fd = accept(server_fd, (sockaddr*)&address, &addrlen);

        if (client_fd < 0) {
            std::cerr << "Failed to accept connection\n";
            continue;
        }

        std::cout << "Client connected\n";
        // read data from the client
        char buffer[1024] = {0};
        ssize_t bytes = read(client_fd, buffer, sizeof(buffer));

        if (bytes <= 0) {
            std::cerr << "Read failed\n";
            close(client_fd);
            continue;
        }

        std::cout << "Received:\n" << buffer << std::endl;

        const char* response = "Hello from Phase 1 Server\n";
        send(client_fd, response, strlen(response), 0);

        close(client_fd);
    }

    close(server_fd);
    return 0;
}