#include <pthread.h>
#include <semaphore.h>
#include <mujoco/mujoco.h>
#include <unistd.h>
#include "stdio.h"
#include "Posicion/Posicion.h"
#include <Comunicacion/RomerinMessage.h>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>  
#include <chrono> //Para temporizador en milisegundos
#include <thread> //Para temporizador en milisegundos
#include <bits/types.h>  
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>

#include <Module/modulesimulator.h>

#include <QFileDialog>
#include <QMainWindow>
#include <QUdpSocket>
#include <QObject>
#include <QTimer>
#include <QTime>
#include <QJsonObject>
#include <QJsonDocument> //para el fromJson
#include <QJsonArray> //para el fromJson
#include <QFile> //para el fromJson
#include <QNetworkDatagram>


#define NUM_THREADS 5


#define PUERTO_ESCUCHA 12001
#define PUERTO_ENVIO 12000

typedef __uint8_t uint8_t;

struct ParametrosControl {
    mjModel* m = NULL;
    mjData* d = NULL;
};

struct InfoPatas{
    int id;
    float Q1;
    float Q2;
    float Q3;
    float Q4;
    float Q5;
    float Q6;
    float Q7;
    float Q8;
};

struct Reposo{
    mjtNum Q1;
    mjtNum Q2;
    mjtNum Q3;
    mjtNum Q4;
    mjtNum Q5; 
    mjtNum Q6; 
    mjtNum suct;
};

class Control{
private:

    bool moviendo;
    bool ventanaAbierta;
    bool hiloControlIniciado;

    bool primerMovimientoAcabado;
    bool segundoMovimientoAcabado;
    bool tercerMovimientoAcabado;
    bool cuartoMovimientoAcabado;

    ParametrosControl* param;
    Reposo reposo;

    pthread_attr_t hilo;
    pthread_t hiloMovimientoPatas[NUM_THREADS];
    pthread_t hiloControlInfo;
    pthread_t HiloLoop;
    sem_t primeraPata;
    sem_t segundaPata;
    sem_t terceraPata;
    sem_t cuartaPata;


    QUdpSocket * ip_port;
    QHostAddress master_address{};
    MsgReader UDP_msg;

    int numPata;

    QList<ModuleSimulator> modules;

    Posicion posActual, posObjetivo;   

public:

    bool hilosCreados;
    
    // Constructor
    Control();
    void setParametros(mjModel* M, mjData *D);
    void setVentanaAbierta(bool apertura);
    bool getVentanaAbierta();
    void getPosicion(const char* site);//--------------------------------------------------- Usa el objeto de tipo control
    static void* getInfoPatas(void* args);
    static void* moverPata1(void* args);
    static void* moverPata2(void* args);
    static void* moverPata3(void* args);
    static void* moverPata4(void* args);
    bool enPosicion(void* args); //---------------------------------------------------
    void iniciarHilosMovimiento(void* args);
    void iniciarSocket(void* args);
    void pararHilosMovimiento();
    void enMovimiento(bool cond);
    bool getMoviendo();
    void recibirInfoSocket(void* args);
    void openJson();
    static void* loop(void* args);

    RomerinMsg executeMessage(uint8_t robot_id, const RomerinMsg &m, void* args);
    void sendVirtualMessage(uint8_t vid, const RomerinMsg &m, void* args);

    // Destructor para limpiar los recursos
    ~Control() {
        pthread_attr_destroy(&hilo);
        sem_destroy(&primeraPata);
        sem_destroy(&segundaPata);
        sem_destroy(&terceraPata);
        sem_destroy(&cuartaPata);
        delete(param);
    }
};
