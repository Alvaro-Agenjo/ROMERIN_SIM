#ifndef TRAYECTORYGENERATOR_H
#define TRAYECTORYGENERATOR_H

#include <QTimer>
#include <QElapsedTimer>
#include "modulecontroller.h"
#include "MTHomogenea.h"

#define standby 15
constexpr float counterTG2MW = 100/40;
//enum class state_t { STAND, RELAX, FIXED_ROTATION, RESET};

struct Movimiento{
    ModuleController* module;
    double angulos[6]{180,180,90,180,180,180};
    double vel[6]{1,1,1,1,1,1};
    int suctionPercentaje{};
    int time_code{};
    Movimiento(ModuleController* module, double angulos[], int suctforce, int time_code);
    Movimiento(ModuleController* module, double angulos[], double vel [], int suctforce, int time_code);
};

class trayectoryGenerator : public QObject
{

public:
    trayectoryGenerator();

    bool isMoving();
    void setMatrizTransformacion(ModuleController * modulo);
    void resetTCPs();

    void setTime(long counter){time = counter;}
    void setTorque(ModuleController* modulo, int motor_id, bool torque = true);
    void setTorque(ModuleController* modulo, bool torques []);
    void setMotorVel(ModuleController * modulo, float max_vel, int motor_id);
    void setMotorVel(ModuleController * modulo, float max_vels[]);
    void setMotorAngles(ModuleController *module, double angle[]);
    void setAdhesion(ModuleController *module, int percentaje);

    void addMovement(ModuleController *module, double angulo[6], int suctForce, int batch);
    bool validateMovement(double angle[],ModuleController *module, double x, double y, double z, bool elbow = true);
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

private:
    long time= 0;
    //QTimer timer;
    //QElapsedTimer millis;

    QString legs[4];

    std::list<Movimiento> orders_list;
    Vector3D center, TCPs[4];
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
