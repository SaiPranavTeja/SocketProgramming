#include "UDSClient.h"
#include <iostream>
#include <string>

struct MyRequest {
    char data[1024];
};

struct MyResponse {
    char data[1024];
};

int main() {
    try {
        UDSClient<MyRequest, MyResponse> client("/tmp/uds_socket");
        client.connectToServer();

        MyRequest request;
        snprintf(request.data, sizeof(request.data), "Hello, Server!");
        int i=0;
        while(i<30)
        {
            MyResponse response = client.sendRequest(request);
            std::cout << "Received response: " << response.data << std::endl;
            i++;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
