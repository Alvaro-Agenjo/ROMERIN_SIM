#ifndef TRAYECTORYGENERATOR_H
#define TRAYECTORYGENERATOR_H

#include <QTimer>
#include <QElapsedTimer>

#include "modulecontroller.h"
#include "MTHomogenea.h"

#define standby 2
#define operating 35
constexpr float counterTG2MW = 100/40;
constexpr bool simple[6] = {1,1,1,0,0,0};
constexpr bool full[6] = {1,1,1,1,1,1};

struct Movimiento{
    ModuleController* module;
    double angulos[6]{180,180,90,180,180,180};
    double vel[6]{1,1,1,1,1,1};
    int suctionPercentaje{};
    unsigned long time_code{};
    bool completo;
    Movimiento(ModuleController* module, double angulos[], int suctforce, int time_code, bool full);
    Movimiento(ModuleController* module, double angulos[], double vel [], int suctforce, int time_code, bool full);
};

class trayectoryGenerator : public QObject
{

public:
    trayectoryGenerator();

    bool isMoving();
    void setMatrizTransformacion(ModuleController * modulo);
    void refreshTCPs();

    void setTime(unsigned long counter){time = counter;}
    void setTorque(ModuleController* modulo, int motor_id, const bool torque = true);
    void setTorque(ModuleController* modulo, const bool torques []);
    void setMotorVel(ModuleController * modulo, float max_vel, int motor_id);
    void setMotorVel(ModuleController * modulo, float max_vels[]);
    void setMotorAngles(ModuleController *module, double angle, int motor_id);
    void setMotorAngles(ModuleController *module, double angle[]);

    void setAdhesion(ModuleController *module, int percentaje);

    void addMovement(ModuleController *module, double angulo[6], int suctForce, int batch, bool full);
    bool validateMovement(double angle[],ModuleController *module, double x, double y, double z, bool elbow = true);
    bool validateMovement(double angle[],ModuleController *module, double x, double y, double z, float RPY[3], bool elbow = true);

    bool moveLeg(QString leg, double x, double y, double z, bool elbow = true, bool fixed = false);
    bool moveLeg(QString leg, double x, double y, double z, float RPY[], bool elbow = true, bool fixed = false);
    bool moveLeg(ModuleController *module, double x, double y, double z, float RPY[], bool elbow, bool fixed);

    bool moveBotAbsolute(Vector3D new_center, float RPY[], int tiempo, bool fixed = true);
    bool moveBotRelative(Vector3D desplazamiento, float RPY[], int tiempo, bool fixed);
    bool moveBot(Vector3D new_center, float RPY[], int batch, bool fixed = false);
    bool chopper(Vector3D coord, float RPY[],int tiempo, bool fixed);


    void reset();
    void stand();
    void relax();
    void fixed_rotation(int n = 1);
    void nextOrder();


private:
    unsigned long time= 0;

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
