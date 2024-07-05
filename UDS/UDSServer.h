#ifndef UDS_SERVER_H
#define UDS_SERVER_H

#include <functional>
#include <string>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <vector>
#include <thread>
#include <mutex>
#include <unordered_set>

template<typename Request, typename Response>
class UDSServer {
public:
    using RequestHandler = std::function<Response(const Request&)>;

    UDSServer(const std::string& socket_path);
    ~UDSServer();

    void setRequestHandler(RequestHandler handler);
    void start();

private:
    std::string socket_path_;
    int server_fd_;
    RequestHandler request_handler_;
    std::vector<std::thread> client_threads_;
    std::unordered_set<int> clients_;
    std::mutex clients_mutex_;
    bool stop_server_;

    void handleClient(int client_fd);
    void processRequest(Request request_);
    void acceptClients();
    void cleanup();
};

#include "UDSServerImpl.h"

#endif // UDS_SERVER_H
