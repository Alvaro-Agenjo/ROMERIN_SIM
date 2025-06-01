#ifndef TRAYECTORYGENERATOR_H
#define TRAYECTORYGENERATOR_H

#include "modulecontroller.h"
//#include "../include/RomerinMessage.h"
#include <QTimer>
#define slowMove 15

class trayectoryGenerator
{
public:
    trayectoryGenerator();

    void setVel(float max_vel, int motor_id);

    void reset();
    bool moveLeg(QString leg, double x, double y, double z, bool elbow = true, bool fixed = false);
    bool moveLeg(QString leg, double x, double y, double z, double orientation [][3], bool elbow = true, bool fixed = false);

    void stand();
    void relax(float x, float y, float z, float ori[3]);
    bool moveLeg(QString leg, float x, float y, float z, double ori[][3], bool fixed = false);
    bool moveLeg(QString leg, float x, float y, float z, bool fixed = false);
    void rotation(double orientation[][3], float rot[]);
    void sendM(ModuleController *module, double angle[]);
    bool objetiveReached(ModuleController *module);
    float a = 0;
    void test(bool elbow);
};

#endif // TRAYECTORYGENERATOR_H
