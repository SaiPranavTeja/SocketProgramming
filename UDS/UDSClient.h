#ifndef UDS_CLIENT_H
#define UDS_CLIENT_H

#include <string>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdexcept>
#include <cstring>
#include <mutex>

template<typename Request, typename Response>
class UDSClient {
public:
    UDSClient(const std::string& socket_path);
    ~UDSClient();

    void connectToServer();
    Response sendRequest(const Request& request);

private:
    std::string socket_path_;
    int client_fd_;
    std::mutex connection_mutex_;
};

#include "UDSClientImpl.h"

#endif // UDS_CLIENT_H
