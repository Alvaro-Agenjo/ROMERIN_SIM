#ifndef TRAYECTORYGENERATOR_H
#define TRAYECTORYGENERATOR_H

#include <QTimer>
#include <QElapsedTimer>
#include "modulecontroller.h"
#include "MTHomogenea.h"

#define standby 15
// enum class command_t { STAND, RELAX, FIXED_ROTATION, RESET, MOVE_TO_POINT};

struct MovimientoV2{
    ModuleController* module;
    double angulos[6]{180,180,90,180,180,180};
    double vel[6]{1,1,1,1,1,1};
    int suctionPercentaje{};
    int batch{};
    MovimientoV2(ModuleController* module, double angulos[], int suctforce, int batch);
    MovimientoV2(ModuleController* module, double angulos[], double vel [], int suctforce, int batch);
};
struct Movimiento{
    union{
        QString name;
        ModuleController* module;
    };
    Vector3D pos;
    float rot[3];
    bool elbow, fixed;
    bool type_name;

    Movimiento(QString leg, double x, double y, double z, float RPY[3], bool elbow = true, bool fixed = false);
    Movimiento(ModuleController* modulo, double x, double y, double z, float RPY[3], bool elbow = true, bool fixed = false);
    Movimiento(const Movimiento& mov);
    ~Movimiento(){
        if(type_name) name.~QString();
    }
};

class trayectoryGenerator : public QObject
{

public:
    trayectoryGenerator();

    void setlegs(bool sim){
        if (sim){
            legs[1] = "FRIGG";    legs[0] = "THOR";
            legs[2] = "ODIN";     legs[3] = "LOKI";
        }
        else{
            legs[1] = "FRIGG";    legs[0] = "TYR";
            legs[2] = "FREYJA";   legs[3] = "LOKI";
        }
    }
    void setVel(float max_vel, int motor_id);
    void setMotorAngle(ModuleController *module, double angle[]);
    void setAdhesion(ModuleController *module, int percentaje);

    void addMovement(QString leg, double x, double y, double z, float RPY[3], bool elbow = true, bool fixed = false);
    void addMovement(ModuleController *module, double angulo[6], int suctForce, int batch);
    bool validateMovement(double angle[],ModuleController *module, double x, double y, double z, float RPY[3], bool elbow = true);
    bool moveLeg(QString leg, double x, double y, double z, bool elbow = true, bool fixed = false);
    bool moveLeg(QString leg, double x, double y, double z, float RPY[3], bool elbow = true, bool fixed = false);
    bool moveLeg(ModuleController *module, double x, double y, double z, float RPY[], bool elbow, bool fixed);

    void Calc3x3ROT(float a, float b, float c, double ortientacion[][3]);

    bool moveBotAbsolute(Vector3D new_center, float RPY[3], int batch);
    bool moveBotRelative(Vector3D new_center, float RPY[3], int batch);
    void reset();
    void stand();
    void relax();

    //void addOrder(command_t order){ order_list.push_back(order);}
    bool nextOrder();
    void test(bool elbow);
    bool isMoving();
private:
    float a = 0;
    QTimer timer;
    QElapsedTimer millis;

    QString legs[4];
    //std::list<command_t> order_list{};
    std::list<Movimiento> orders_list;
    std::list<MovimientoV2> orders_listV2;
    Vector3D center, THOR_TCP, FRIGG_TCP, ODIN_TCP, LOKI_TCP;
    Matriz_Transformacion centro2leg_DU, centro2leg_IU, centro2leg_ID, centro2leg_DD;
};

struct MotorsAngles{
    double angle[6];

    MotorsAngles(double angulo[6]){
        for(int i = 0; i<6; i++)
            angle[i] = angulo[i];
    }
};


#endif // TRAYECTORYGENERATOR_H
