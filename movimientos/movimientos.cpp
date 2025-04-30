
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "include/RomerinMessage.h"
//#include "modulecontroller.h"
#include "modulo.h"

unsigned char dispositivos_conectados = 0;
int main(){

//     //Conexion inicial

//         // Creacion del socket escucha

        int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
        if (sock_fd < 0) {
            perror("socket");
            return -1;
        }

        int opt = 1;
        if(setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))<0) return -1;
        if(setsockopt(sock_fd, SOL_SOCKET, SO_BROADCAST, &opt, sizeof(opt))<0) return -1;

        sockaddr_in local{};
        local.sin_family = AF_INET;
        local.sin_port = htons(12001);
        local.sin_addr.s_addr = inet_addr("10.0.2.15");

        if (bind(sock_fd, (struct sockaddr*)&local, sizeof(local)) < 0) {
            perror("bind");
            close(sock_fd);
            sock_fd = -1;
            return -1;
        }

//         //Broadcast de señal
        
        while(dispositivos_conectados <4){
            std::cout << "inicio while" << std::endl;
            static int count = 0;
            if(count++>20)count=0;
            if (count == 0) {
                RomerinMsg ping(ROM_SET_MASTER_IP);
                
                sockaddr_in broadcast{};
                broadcast.sin_family = AF_INET;
                broadcast.sin_port = htons(12000);
                broadcast.sin_addr.s_addr = htonl(INADDR_BROADCAST);
        
                int sent = sendto(sock_fd, ping.data, ping.size + 3, 0,
                                  (struct sockaddr*)&broadcast, sizeof(broadcast));
                if (sent < 0) {
                    perror("sendto");
                }
                std::cout << "datos enviados" << std::endl;
            }

//         //recepcion de señal rom_name
            char buffer[1024];            
            std::cout << "Esperando mensajes UDP en el puerto " << 12001 << "...\n";

            ModuleController *module = nullptr;
            socklen_t len = sizeof(module->ip_port);
            memset(buffer, 0, sizeof(buffer));
            int n = recvfrom(sock_fd, buffer, sizeof(buffer) - 1, 0,
                            (struct sockaddr*)&module->ip_port, &len);
            if (n < 0) {
                perror("Error al recibir");
                break;
            }
            std::cout << "datos recibidos" << std::endl;
            char ipRemitente[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(module->ip_port.sin_addr), ipRemitente, INET_ADDRSTRLEN);

            std::cout << "Mensaje recibido desde " << ipRemitente
                    << ":" << ntohs(module->ip_port.sin_port) << " -> "
                    << buffer << "\n";




//         //almacenamiento de id para posterior envio
    
//     //Bucle principal

//         //lectura de movimiento por teclado

//         //interpretacion de comando

//         //gestion del comando

//             //estado actual

//             //calculo de nueva posicion 

//         //mensaje

//             //creacion del mensaje

//             //envio del mensaje
    }
    return 0;
}
// #include <iostream>
// #include <cstring>
// #include <unistd.h>
// #include <fcntl.h>
// #include <arpa/inet.h>
// #include <netinet/in.h>
// #include <sys/socket.h>
// #include <sys/types.h>

// #include "include/RomerinMessage.h"
// #include "modulecontroller.h"

// int sock_fd = -1;
// int count = 0;

// MsgReader udp_reader;

// void loop_wifi() {
//     if (count++ > 20) count = 0;

//     // Crear socket si no existe
//     if (sock_fd < 0) {
//         sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
//         if (sock_fd < 0) {
//             perror("socket");
//             return;
//         }

//         int opt = 1;
//         setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
//         setsockopt(sock_fd, SOL_SOCKET, SO_BROADCAST, &opt, sizeof(opt));

//         sockaddr_in local{};
//         local.sin_family = AF_INET;
//         local.sin_port = htons(12001);
//         local.sin_addr.s_addr = inet_addr("127.0.0.1");

//         if (bind(sock_fd, (struct sockaddr*)&local, sizeof(local)) < 0) {
//             perror("bind");
//             close(sock_fd);
//             sock_fd = -1;
//             return;
//         }

//         // No usamos event loop, así que no hay connect(). Se gestiona manualmente.
//     }

//     // Enviar broadcast cada 20 ciclos
//     if (count == 0) {
//         RomerinMsg ping(ROM_SET_MASTER_IP);

//         sockaddr_in broadcast{};
//         broadcast.sin_family = AF_INET;
//         broadcast.sin_port = htons(12000);
//         broadcast.sin_addr.s_addr = htonl(INADDR_BROADCAST);

//         int sent = sendto(sock_fd, ping.data, ping.size + 3, 0,
//                           (struct sockaddr*)&broadcast, sizeof(broadcast));
//         if (sent < 0) {
//             perror("sendto");
//         }
//     }

//     // Leer datagramas pendientes (no bloqueante)
//     read_ip_port();
// }

// void read_ip_port() {
//     fd_set read_fds;
//     FD_ZERO(&read_fds);
//     FD_SET(sock_fd, &read_fds);

//     timeval timeout{0, 0};  // sin bloqueo
//     int ready = select(sock_fd + 1, &read_fds, nullptr, nullptr, &timeout);

//     if (ready > 0 && FD_ISSET(sock_fd, &read_fds)) {
//         while (true) {
//             uint8_t buffer[512];
//             sockaddr_in sender{};
//             socklen_t sender_len = sizeof(sender);

//             ssize_t len = recvfrom(sock_fd, buffer, sizeof(buffer), MSG_DONTWAIT,
//                                    (struct sockaddr*)&sender, &sender_len);
//             if (len <= 0) break;

//             std::string sender_ip = inet_ntoa(sender.sin_addr);

//             // Aquí iría la lógica de udp_reader, getMessage(), ModulesHandler, etc.
//             // Por ejemplo:
//             for (ssize_t i = 0; i < len; ++i) {
//                 if (udp_reader.add_uchar(buffer[i])) {
//                     auto msg = udp_reader.getMessage();

//                     bool simulated = (msg.id == ROM_SIMULATION);
//                     uint8_t virtualid = 0;

//                     ModuleController* module = nullptr;

//                     if (simulated) {
//                         virtualid = msg.info[0];
//                         msg = get_simulatedMsg(msg);
//                         module = ModulesHandler::getWithSimulatedId(virtualid);
//                     } else {
//                         module = ModulesHandler::getWithAddress(sender.sin_addr.s_addr);
//                     }

//                     if (module) {
//                         module->executeMessage(msg);
//                         module->reset_wifi_watchdog();
//                     } else if (msg.size && msg.id == ROM_NAME) {
//                         char name[100] = "";
//                         romerin_getString(msg.info, name, 99);

//                         std::string qname = name;
//                         std::cout << "Robot " << qname << " detected at IP " << sender_ip << std::endl;

//                         module = ModulesHandler::getWithName(qname);
//                         if (simulated) {
//                             module->simulated = true;
//                             module->virtualid = virtualid;
//                         }

//                         module->ip = sender.sin_addr.s_addr;
//                         if (!module->tab) {
//                             module->tab = new RomerinModule();
//                             module->tab->setModule(module);
//                             module->reset_wifi_watchdog();
//                         }

//                         updateTable();
//                     }
//                 }
//             }
//         }
//     }
// }
