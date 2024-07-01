#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <vector>
#include <string>
enum class SERVER_INIT_STATUS:int{
    INIT_SUCCESS=0,
    SOCKET_ERROR=-1,
    BIND_ERROR=-2
};
class UDSServe
{
private:
    /* data */
    int server_fd;
    std::vector<int> client_fds;
    socklen_t client_len;
    struct sockaddr_un server_addr, client_addr;
public:
    UDSServe(std::string socket_path);
    ~UDSServe();
    SERVER_INIT_STATUS init();

};

