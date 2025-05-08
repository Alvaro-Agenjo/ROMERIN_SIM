/*



1. Obtener velocidad maxima (por consola) y enviar activacion(torque on) y velocidad (max_vel)

2. Obtener comando de movimiento (por consola)

3. Obtener desde la interfaz la direccion para enviar datagrama a los cuatro modulos

4. Obtener posicion de las patas

5. Calcular nueva posicion de las patas

6. Enviar valores de posicion y succion a las patas

7. Obtener informacion al recibir mensaje de la posicion actual de los motores con el fin de conocer cuando se ha terminado el movimiento y puede comenzar el siguiente


*/

#include <iostream>
#include <math.h>

#include <unistd.h>
#include <fcntl.h>


#include <QNetworkInterface>
#include <QNetworkDatagram>
#include <QUdpSocket>

#include "Modulo.h"
#include "include/RomerinMessage.h"

struct Data
{
    int id;
    float data[6];
};

void getData(Data &datos, int n);
// void writeVel(Data &datos, int fd);
// void writeNewPos(Data &datos, int fd);
// void ActivateTorque(Modulo &Pata);


QUdpSocket *interfaz = new QUdpSocket();

int main()
{

    int fd;
    bool fin = false;
    bool newCommand = false;
    int UI;

    
    QHostAddress ip;
    Data datos;

    // crear el fichero por el cual se comunican la interfaz y el generador de trayectorias
    fd = open("com.txt", O_RDONLY | O_CREAT , 0666);
    if (fd < 0)
    {
        perror("error al crear el archivo");
        return -1;
    }

    while (read(fd, &ip, sizeof(QHostAddress)) < 1)
    {
        lseek(fd, SEEK_SET, 0);
    }

    std::cout << "IP obtenida" << std::endl;
    //interfaz->bind(ip, 12001);

    std::cout << "Datos a introducir: " << std::endl;
    std::cout << "[1] Velocidad de Movimiento. " << std::endl;
    std::cout << "[2] Comando de movimiento. " << std::endl;
    std::cout << "[3] Succion. " << std::endl;
    std::cout << "[5] Torque off. " << std::endl;
    std::cout << "[6] Demo vuelta al mundo " << std::endl;
    std::cout << "[-1] Cerrar " << std::endl;

    while(1){
        // Seleccion de modulo --> id [THOR, LOKI, ODIN, FRIGG]
        // Velocidad: id v1 v2 v3 v4 v5 v6 [m1 m2 m3 m4 m5 m6]
        // Posicion: id m1 m2 m3 m4 m5 m6 [m1 m2 m3 m4 m5 m6]
        // Psicion efector final: id x y z
        // Ventosa: id p1 [porcentaje potencia]
        // Demo: 6 (vuelta fija)
        // Torque off 5
        // Salir: -1
        std::cout << "Introduzca el comando: " << std::endl;
        std::cin >> UI;
        switch (UI){
        case 1:
        
            std::cout << "introduzca la velocidad máxima para cada motor con el siguiente formato: " << std::endl;
            std::cout << "id v1 v2 v3 v4 v5 v6 \n" << std::endl;
            getData(datos, 7);

            //enviar datos a los modulos
            //writeVel(datos, fd);
            break;
        
        case 2:
           std::cout << "introduzca el comando de movimiento para cada motor con el siguiente formato: " << std::endl;
            std::cout << "id m1 m2 m3 m4 m5 m6 \n"
                 << std::endl;
            getData(datos, 7);

            //enviar datos a los modulos
            //writeNewPos(datos, fd);
            break;
        
        case 3:
        

            std::cout << "introduzca el valor de potencia de succion [0-100] con el siguiente formato" << std::endl;
            std::cout << "id p1 \n"
                 << std::endl;
            getData(datos, 2);
            break;
        
        case 4:
           std::cout << "introduzca la posicion del efector final con el siguiente formato" << std::endl;
            std::cout << "id x y z \n" << std::endl;
            getData(datos, 4);
            break;
        
        case 5:
            // modifica torque a 0;
            break;
        
        case 6:
           // Ejecuta bucle de movimiento
            break;
        
        default:
            close(fd);
            exit(0);
        }
    }
    
    
}
void getData(Data &datos, int n){
    std::cin >> datos.id;
    for (int i = 0; i < n-1; i++){
        std::cin >> datos.data[i];
    }
}
// void writeVel(Data &datos, int fd){
//     Modulo Pata;
//     lseek(fd, SEEK_SET, sizeof(QUdpSocket));
//     lseek(fd, SEEK_CUR, datos.id * sizeof(Modulo));
//     read(fd, &Pata, sizeof(Modulo));

//     for (int i = 0; i < 6; i++){
//         RomerinMsg m = romerinMsg_VelocityProfile(i, datos.data[i]);
//         interfaz->writeDatagram(m.info, m.size + 3, Pata.ip, 12000);
//     }
// }
// void writeNewPos(Data &datos, int fd){
//     Modulo Pata;
//     lseek(fd, SEEK_SET, sizeof(QUdpSocket));
//     lseek(fd, SEEK_CUR, datos.id * sizeof(Modulo));
//     read(fd, &Pata, sizeof(Modulo));

//     ActivateTorque(Pata);
//     for (int i = 0; i < 6; i++){
//         RomerinMsg m = romerinMsg_ServoGoalAngle(i, datos.data[i]);
//         interfaz->writeDatagram(m.info, m.size + 3, Pata.ip, 12000);
//     }
// }
// void ActivateTorque(Modulo &Pata){
//     for(int i = 0; i < 6; i++){
//         RomerinMsg m = romerinMsg_Torque(i);
//         interfaz->writeDatagram(m.info, m.size + 3, Pata.ip, 12000);
//     }
// }