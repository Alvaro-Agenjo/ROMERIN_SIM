#include "trayectorygenerator.h"

#include "moduleshandler.h"
#include "module.h"


trayectoryGenerator::trayectoryGenerator() {}

void trayectoryGenerator::setVel(float max_vel, int motor_id){
    RomerinMsg m = romerinMsg_VelocityProfile(motor_id, max_vel);
    for(auto mod : ModulesHandler::module_list){
        mod->sendMessage(m);
    }
}

void trayectoryGenerator::stand()
{
    ModuleController *module = ModulesHandler::getWithName("THOR");
    double m[6] = {185,246,197,180,233,127};
    sendM(module, m);
}

void trayectoryGenerator::relax(int x, int y, int z)
{
    //ajustamos velocidad baja para movimiento suave
    for(int i = 1; i<= 6; i++){
        setVel(slowMove, i);
    }
    qDebug()<<"Movimiento: " << (moveLeg("THOR", x,y,z)? "Correcto" : "Fallido");

}

bool trayectoryGenerator::moveLeg(QString leg, float x, float y, float z, bool fixed)
{
    ModuleController *module = ModulesHandler::getWithName(leg);
    double m[6], q[6];

    if(!module->mod->romkin.IKwrist(q,x, y, z,true)) return false;
    module->mod->romkin.q2m(m,q);
    sendM(module, m);

    if(fixed){
        RomerinMsg m = romerinMsg_SuctionCupPWM(50);
        module->sendMessage(m);
    }
    return true;
}

void trayectoryGenerator::sendM(ModuleController *module, double m[])
{
    for(int i = 0; i<6; i++){
        RomerinMsg m = romerinMsg_ServoGoalAngle(i, m[i]);
        module->sendMessage(m);
        qDebug()<<m[i];
    }

}

