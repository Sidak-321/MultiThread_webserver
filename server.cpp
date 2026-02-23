#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sstream>

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
         char buffer[4096]={0};
         ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer));
         if (bytes_read < 0) {
             std::cerr << "Failed to read from client\n";
             close(client_fd);
             continue;  
         }
         std:: string request(buffer);

         size_t pos = request.find("\r\n");
        std::string request_line = request.substr(0, pos);
         
        std::cout<<"Request Line: " << request_line << std::endl;
        
        std::istringstream iss(request_line);
        std::string method, path, version;
        iss >> method >> path >> version;
        std::cout << "Method: " << method << ", Path: " << path << ", Version: " << version << std::endl;

        //routing

        std :: string body;
        if(path=="/")
        {
            body="<h1>welcome to my server</h1>";
        }
        else if(path=="/about")
        {
            body="<h1>this is a simple http server implemented in c++</h1>";
        }
        else
        {
            body="<h1>404 not found</h1>";
        }
        // response 
        std :: string response =
           "HTTP/1.1 200 OK\r\n"
              "Content-Type: text/html\r\n" 
                "Content-Length: " + std::to_string(body.size()) + "\r\n"
                "Connection: close\r\n"
                "\r\n" + body;

        // send response to the client
        send(client_fd, response.c_str(), response.size(), 0);
      
    }


    return 0;
}