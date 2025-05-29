#ifndef TRAYECTORYGENERATOR_H
#define TRAYECTORYGENERATOR_H

#include "modulecontroller.h"

#define standby 5

class trayectoryGenerator
{
public:
    trayectoryGenerator();

    void setVel(float max_vel, int motor_id);
    void setMotorAngle(ModuleController *module, double angle[]);
    void setAdhesion(ModuleController *module, int percentaje);

    void reset();
    bool moveLeg(QString leg, double x, double y, double z, bool elbow = true, bool fixed = false);

    void relax(float x, float y, float z, float ori[3]);
    bool moveLeg(QString leg, float x, float y, float z, double ori[][3], bool fixed = false);
    bool moveLeg(QString leg, float x, float y, float z, bool fixed = false);
    void rotation(double orientation[][3], float rot[]);

    bool objetiveReached(ModuleController *module);
    float a = 0;
    void loopTest(bool elbow);
};

#endif // TRAYECTORYGENERATOR_H
