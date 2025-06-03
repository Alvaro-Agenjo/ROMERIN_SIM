#include "trayectorygenerator.h"

#include "moduleshandler.h"
#include "module.h"


trayectoryGenerator::trayectoryGenerator() {}

void trayectoryGenerator::setVel(float max_vel, int motor_id){
    RomerinMsg m = romerinMsg_VelocityProfile(motor_id, max_vel);
    for(auto module : ModulesHandler::module_list){
        module->sendMessage(m);
    }
}
void trayectoryGenerator::setMotorAngle(ModuleController *module, double angle[])
{
    for(int i = 0; i<6; i++){
        RomerinMsg m = romerinMsg_ServoGoalAngle(i, angle[i]);
        module->sendMessage(m);
        qDebug()<<"Motor "<< i<< ": "<<angle[i];
    }

}
void trayectoryGenerator::setAdhesion(ModuleController *module, int percentaje)
{
    RomerinMsg m = romerinMsg_SuctionCupPWM(percentaje);
    module->sendMessage(m);
}


bool trayectoryGenerator::moveLeg(QString leg, double x, double y, double z, bool elbow, bool fixed)
{
    ModuleController *module = ModulesHandler::getWithName(leg);
    double m[6], q[6];

    //Check if position is geometricaly possible
    if(!module->mod->romkin.IKwrist(q,x, y, z, elbow)){
        qDebug()<<"Out of range position";
        return false;
    }
    module->mod->romkin.q2m(m,q, true);

    //Check if joint physical limits are not surpassed
    if(module->mod->checkJointsLimits(m, true)) {
        qDebug()<<"Joint limit surpassed";
        return false;
    }

    //Sends suction power command if necessary
    RomerinMsg msg;
    if(fixed) msg = romerinMsg_SuctionCupPWM(50);
    else msg = romerinMsg_SuctionCupPWM(standby);
    module->sendMessage(msg);


    //Sends movement commands
    setMotorAngle(module, m);
    // qDebug()<<"Q1: "<<q[0]<<" Q2: "<<q[1]<<" Q3: "<<q[2];

    return true; //Return true movement command successfull
}
bool trayectoryGenerator::moveLeg(QString leg, double x, double y, double z, double orientation[][3], bool elbow, bool fixed)
{
    ModuleController *module = ModulesHandler::getWithName(leg);
    double m[6]={}, q[6]= {}, p[3] = {x,y,z};

    if(!module->mod->romkin.IKfast(q, orientation, p, elbow, true)) {
        qDebug()<<"Fuera de rango";
        return false;
    }
    module->mod->romkin.q2m(m,q);

    //Check if joint physical limits are not surpassed
    if(module->mod->checkJointsLimits(m, true)) {
        qDebug()<<"Joint limit surpassed";
        return false;
    }

    //Sends suction power command if necessary
    RomerinMsg msg;
    if(fixed) msg = romerinMsg_SuctionCupPWM(50);
    else msg = romerinMsg_SuctionCupPWM(standby);
    module->sendMessage(msg);


    //Sends movement commands
    setMotorAngle(module, m);
    // qDebug()<<"Q1: "<<q[0]<<" Q2: "<<q[1]<<" Q3: "<<q[2];

    return true; //Return true movement command successfull
}
void trayectoryGenerator::Calc3x3ROT(float giro[3], double orientacion[3][3])
{
    // Convertir grados a radianes
    for (int i = 0; i < 3; i++) {
        giro[i] /= RomKin::rad2deg;
    }

    double cx = cos(giro[0]), sx = sin(giro[0]);
    double cy = cos(giro[1]), sy = sin(giro[1]);
    double cz = cos(giro[2]), sz = sin(giro[2]);

    // Matriz de rotación sobre ejes globales (Rx * Ry * Rz)
    orientacion[0][0] = cy * cz;
    orientacion[0][1] = sy * sz;
    orientacion[0][2] = -sy;

    orientacion[1][0] = -sz * cx +cz * sx * sy;
    orientacion[1][1] = cz * cx + sz * sy * sx;
    orientacion[1][2] = sx * cy;

    orientacion[2][0] = sz * sx + cz * sy * cx;
    orientacion[2][1] = -cz * sx + sz * sy * cx;
    orientacion[2][2] = cx * cy;
}


void trayectoryGenerator::reset()
{
    qDebug()<<"Reset";
    double m[6] = {185,246,197,180,233,127};
    for(const auto module : ModulesHandler::module_list){
        setMotorAngle(module, m);
        setAdhesion(module, standby);
    }
}
void trayectoryGenerator::stand()
{

}
void trayectoryGenerator::relax(float x, float y, float z, float ori[3] )
{

}


void trayectoryGenerator::test(bool elbow)
{
    ModuleController *module = ModulesHandler::getWithName("THOR");
    qDebug()<<"pos: "<<a;

    if(moveLeg("THOR", a/1000.0, 0,0, elbow)){
        qDebug()<<"Valida";
    }
    else qDebug()<<"Fuera de alcance";
    a += 5;
}


