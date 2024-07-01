#include "UDSServer.h"

#define SOCKET_PATH "/tmp/mysocket"

UDSServe::UDSServe(std::string socket_path)
{
      // Initialize socket structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, socket_path.c_str(), sizeof(server_addr.sun_path) - 1);

}
SERVER_INIT_STATUS UDSServe::init()
{
       // Create a Unix domain stream socket
    if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket error");
        throw("socket error");
    }
    // Remove any existing socket with the same filename
    unlink(server_addr.sun_path);

    // Bind socket to the address
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind error");
        throw("bind error");
    }
}
UDSServe::~UDSServe()
{
}

int main() {
    int server_fd, client_fd;
    socklen_t client_len;
    struct sockaddr_un server_addr, client_addr;
    char buf[1024];

    // Create a Unix domain stream socket
    if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket error");
        exit(EXIT_FAILURE);
    }

   
    // Listen for incoming connections
    if (listen(server_fd, 5) == -1) {
        perror("listen error");
        exit(EXIT_FAILURE);
    }

    printf("Server is listening...\n");

    // Accept incoming connection
    client_len = sizeof(client_addr);
    if ((client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len)) == -1) {
        perror("accept error");
        exit(EXIT_FAILURE);
    }

    // Receive data from client
    int len = read(client_fd, buf, sizeof(buf) - 1);
    if (len == -1) {
        perror("read error");
        exit(EXIT_FAILURE);
    }
    buf[len] = '\0'; // Null-terminate the received data
    printf("Received message from client: %s\n", buf);

    // Send response back to client
    const char *response = "Message received";
    if (write(client_fd, response, strlen(response)) == -1) {
        perror("write error");
        exit(EXIT_FAILURE);
    }

    // Close sockets
    close(client_fd);
    close(server_fd);

    // Remove the socket file
    unlink(SOCKET_PATH);

    return 0;
}
