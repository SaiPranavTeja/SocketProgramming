#include "UDSClient.h"

template<typename Request, typename Response>
UDSClient<Request, Response>::UDSClient(const std::string& socket_path)
    : socket_path_(socket_path), client_fd_(-1) {}

template<typename Request, typename Response>
UDSClient<Request, Response>::~UDSClient() {
    if (client_fd_ != -1) {
        close(client_fd_);
    }
}

template<typename Request, typename Response>
void UDSClient<Request, Response>::connectToServer() {
    std::lock_guard<std::mutex> lock(connection_mutex_);

    if (client_fd_ != -1) {
        throw std::runtime_error("Client is already connected");
    }

    client_fd_ = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_fd_ == -1) {
        throw std::runtime_error("Failed to create socket");
    }

    struct sockaddr_un addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    std::strncpy(addr.sun_path, socket_path_.c_str(), sizeof(addr.sun_path) - 1);

    if (connect(client_fd_, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        close(client_fd_);
        client_fd_ = -1;
        throw std::runtime_error("Failed to connect to server");
    }
}

template<typename Request, typename Response>
Response UDSClient<Request, Response>::sendRequest(const Request& request) {
    std::lock_guard<std::mutex> lock(connection_mutex_);

    if (client_fd_ == -1) {
        throw std::runtime_error("Client is not connected");
    }

    if (write(client_fd_, &request, sizeof(request)) != sizeof(request)) {
        throw std::runtime_error("Failed to send request");
    }

    Response response;
    int bytes_read = read(client_fd_, &response, sizeof(response));
    if (bytes_read != sizeof(response)) {
        throw std::runtime_error("Failed to read response");
    }

    return response;
}
