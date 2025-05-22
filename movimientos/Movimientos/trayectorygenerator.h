#ifndef TRAYECTORYGENERATOR_H
#define TRAYECTORYGENERATOR_H

#include "modulecontroller.h"
//#include "../include/RomerinMessage.h"

#define slowMove 12

class trayectoryGenerator
{
public:
    trayectoryGenerator();

    void setVel(float max_vel, int motor_id);

    void stand();
    void relax(int x, int y, int z);
private:
    bool moveLeg(QString leg, float x, float y, float z, bool fixed = false);
    void sendM(ModuleController *module, double *m);
};

#endif // TRAYECTORYGENERATOR_H
