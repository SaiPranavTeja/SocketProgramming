#include "UDSServer.h"
#include <iostream>
#include <stdexcept>
#include <cstring>

template<typename Request, typename Response>
UDSServer<Request, Response>::UDSServer(const std::string& socket_path)
    : socket_path_(socket_path), server_fd_(-1), stop_server_(false) {
    server_fd_ = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd_ == -1) {
        throw std::runtime_error("Failed to create socket");
    }

    struct sockaddr_un addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    std::strncpy(addr.sun_path, socket_path_.c_str(), sizeof(addr.sun_path) - 1);

    unlink(socket_path_.c_str());

    if (bind(server_fd_, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        throw std::runtime_error("Failed to bind socket");
    }

    if (listen(server_fd_, 5) == -1) {
        throw std::runtime_error("Failed to listen on socket");
    }
}

template<typename Request, typename Response>
UDSServer<Request, Response>::~UDSServer() {
    stop_server_ = true;
    close(server_fd_);
    unlink(socket_path_.c_str());
    cleanup();
}

template<typename Request, typename Response>
void UDSServer<Request, Response>::setRequestHandler(RequestHandler handler) {
    request_handler_ = handler;
}

template<typename Request, typename Response>
void UDSServer<Request, Response>::start() {
    std::thread(&UDSServer::acceptClients, this).detach();
    while (!stop_server_) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

template<typename Request, typename Response>
void UDSServer<Request, Response>::acceptClients() {
    while (!stop_server_) {
        int client_fd = accept(server_fd_, nullptr, nullptr);
        if (client_fd == -1) {
            if (stop_server_) break;
            std::cerr << "Failed to accept client connection" << std::endl;
            continue;
        }
        {
            std::lock_guard<std::mutex> lock(clients_mutex_);
            clients_.insert(client_fd);
        }
        client_threads_.emplace_back(&UDSServer::handleClient, this, client_fd);
    }
}

template<typename Request, typename Response>
void UDSServer<Request, Response>::handleClient(int client_fd) {
    while (!stop_server_) {
        Request request;
        int bytes_read = read(client_fd, &request, sizeof(Request));
        if (bytes_read > 0) {
            if (request_handler_) {
                Response response = request_handler_(request);
                write(client_fd, &response, sizeof(Response));
            } else {
                std::cerr << "No request handler set" << std::endl;
            }
        } else if (bytes_read == 0) {
            break; // Client disconnected
        } else {
            std::cerr << "Failed to read from client" << std::endl;
            break;
        }
    }
    {
        std::lock_guard<std::mutex> lock(clients_mutex_);
        clients_.erase(client_fd);
    }
    close(client_fd);
}

template<typename Request, typename Response>
void UDSServer<Request, Response>::cleanup() {
    {
        std::lock_guard<std::mutex> lock(clients_mutex_);
        for (int client_fd : clients_) {
            close(client_fd);
        }
        clients_.clear();
    }
    for (auto& thread : client_threads_) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    client_threads_.clear();
}
