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

void trayectoryGenerator::reset()
{
    qDebug()<<"Reset";
    double m[6] = {185,246,197,180,233,127};
    for(const auto mod : ModulesHandler::module_list){
        sendM(mod, m);
    }

    a = 0;

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


    /* Maintain angle of q4,q5,q6 */
    MotorInfoData minfo[6];
    module->mod->get_motor_info(minfo);
    for(int i = 3; i< 6;i++){
        m[i] = minfo[i].position;
    }

    //Check if joint physical limits are not surpassed
    if(module->mod->checkJointsLimits(m, true)) {
        qDebug()<<"Joint limit surpassed";
        return false;
    }

    //Sends suction power command if necessary
    if(fixed){
        RomerinMsg m = romerinMsg_SuctionCupPWM(50);
        module->sendMessage(m);
    }

    //Sends movement commands
    sendM(module, m);
    qDebug()<<"Q1: "<<q[0]<<" Q2: "<<q[1]<<" Q3: "<<q[2];
    //Return true movement command successfull
    return true;
}

void trayectoryGenerator::relax(float x, float y, float z, float ori[3] )
{

    double m[3][3];
    rotation(m,ori);
    if(moveLeg("THOR", x,y,z,m)){
        qDebug()<<"Movimiento: Viable";

        //ajustamos velocidad baja para movimiento suave
        for(int i = 1; i<= 6; i++){
            setVel(slowMove, i);
        }
    }
    else qDebug()<<"Movimiento: Fallido";

}

bool trayectoryGenerator::moveLeg(QString leg, float x, float y, float z, double ori[][3], bool fixed)
{
    ModuleController *module = ModulesHandler::getWithName(leg);
    double m[6]={}, q[6]= {}, p[3] = {x,y,z};

    if(!module->mod->romkin.IKfast(q, ori, p, true, true)) {
        qDebug()<<"Fuera de rango";
        return false;
    }

    // if(!module->mod->romkin.IKwrist(q,x, y, z, true)) return false;
    module->mod->romkin.q2m(m,q);
    sendM(module, m);

    if(fixed){
        RomerinMsg m = romerinMsg_SuctionCupPWM(50);
        module->sendMessage(m);
    }
    return true;
}

bool trayectoryGenerator::moveLeg(QString leg, float x, float y, float z, bool fixed)
{
    ModuleController *module = ModulesHandler::getWithName(leg);
    double m[6]={}, q[6]= {};

    //if(!module->mod->romkin.IKfast(q, ori, p, true, true)) return false;
    if(!module->mod->romkin.IKwrist(q,x, y, z, fixed)) return false;
    module->mod->romkin.q2m(m,q);
    sendM(module, m);

    // if(fixed){
    //     RomerinMsg m = romerinMsg_SuctionCupPWM(50);
    //     module->sendMessage(m);
    // }
    return true;
}

void trayectoryGenerator::rotation(double orientacion[][3], float rot[])
{
    double c1 = cos(rot[2]);
    double s1 = sin(rot[2]);
    double c2 = cos(rot[1]);
    double s2 = sin(rot[1]);
    double c3 = cos(rot[0]);
    double s3 = sin(rot[0]);

    orientacion[0][0] = c1 * c2;
    orientacion[0][1] = c1 * s2 * s3 - s1 * c3;
    orientacion[0][2] = c1 * s2 * c3 + s1 * s3;

    orientacion[1][0] = s1 * c2;
    orientacion[1][1] = s1 * s2 * s3 + c1 * c3;
    orientacion[1][2] = s1 * s2 * c3 - c1 * s3;

    orientacion[2][0] = -s2;
    orientacion[2][1] = c2 * s3;
    orientacion[2][2] = c2 * c3;
}

void trayectoryGenerator::sendM(ModuleController *module, double angle[])
{
    for(int i = 0; i<6; i++){
        RomerinMsg m = romerinMsg_ServoGoalAngle(i, angle[i]);
        module->sendMessage(m);
        qDebug()<<"Motor "<< i<< ": "<<angle[i];
    }

}

bool trayectoryGenerator::objetiveReached(ModuleController *module)
{
    MotorInfoData info[6];
    module->mod->get_motor_info(info);

    for(int i = 0; i< 6; i++){
        if(info[i].velocity != 0) return false;
    }
    return true;
}

void trayectoryGenerator::loopTest(bool elbow)
{
    ModuleController *module = ModulesHandler::getWithName("THOR");
    qDebug()<<"pos: "<<a;

    if(moveLeg("THOR", a/1000.0, 0,0, elbow)){
        qDebug()<<"Valida";
        if(objetiveReached(module)) qDebug()<<"continue";
    }
    else qDebug()<<"Fuera de alcance";
}


