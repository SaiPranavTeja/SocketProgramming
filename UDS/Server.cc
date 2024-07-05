#include "UDSServer.h"
#include <iostream>
#include <string>

struct MyRequest {
    char data[1024];
};

struct MyResponse {
    char data[1024];
};

MyResponse handleRequest(const MyRequest& request) {
    std::cout << "Received request: " << request.data << std::endl;
    MyResponse response;
    snprintf(response.data, sizeof(response.data), "Response: %s", request.data);
    return response;
}

int main() {
    try {
        UDSServer<MyRequest, MyResponse> server("/tmp/uds_socket");
        server.setRequestHandler(handleRequest);
        server.start();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
