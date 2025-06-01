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


    /* Maintain angle of q4,q5,q6 */
//    MotorInfoData minfo[6];
//    module->mod->get_motor_info(minfo);
//    for(int i = 3; i< 6;i++){
//        m[i] = minfo[i].position;
//    }


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
    qDebug()<<"Q1: "<<q[0]<<" Q2: "<<q[1]<<" Q3: "<<q[2];

    return true; //Return true movement command successfull
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




bool trayectoryGenerator::moveLeg(QString leg, double x, double y, double z, double orientation[][], bool elbow, bool fixed)
{

}

void trayectoryGenerator::stand()
{

}

void trayectoryGenerator::relax(float x, float y, float z, float ori[3] )
{

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
    setMotorAngle(module, m);

    if(fixed){
        RomerinMsg m = romerinMsg_SuctionCupPWM(50);
        module->sendMessage(m);
    }
    return true;
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

void trayectoryGenerator::test(bool elbow)
{
    ModuleController *module = ModulesHandler::getWithName("THOR");
    qDebug()<<"pos: "<<a;

    if(moveLeg("THOR", a/1000.0, 0,0, elbow)){
        qDebug()<<"Valida";
        if(objetiveReached(module)) qDebug()<<"continue";
    }
    else qDebug()<<"Fuera de alcance";
    a += 5;
}


