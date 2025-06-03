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

    bool moveLeg(QString leg, double x, double y, double z, bool elbow = true, bool fixed = false);
    bool moveLeg(QString leg, double x, double y, double z, double orientation [][3], bool elbow = true, bool fixed = false);
    void Calc3x3ROT(float giro[3], double ortientacion[][3]);

    void reset();
    void stand();
    void relax(float x, float y, float z, float ori[3]);


    void test(bool elbow);

private:
    float a = 0;
};

#endif // TRAYECTORYGENERATOR_H
