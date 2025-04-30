#include <string>
#include <sys/socket.h>
#include <netinet/in.h>

#include "include/RomerinMessage.h"
typedef unsigned char uint8_t;

class ModuleController{
    public:
    std::string name;
    bool simulated=false;
    uint8_t virtualid=0;

    int sock_fd;
    sockaddr_in ip_port;    

    void sendMessage(const RomerinMsg &m);

};
