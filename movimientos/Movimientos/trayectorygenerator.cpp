#include "trayectorygenerator.h"

#include "moduleshandler.h"
#include "module.h"
#include <vector>

trayectoryGenerator::trayectoryGenerator() {
    double m[3][3], p[] ={0.08,0.0825,0};
    Calc3x3ROT(0,0,45.8814, m);
    centro2leg_DU = *new Matriz_Transformacion(m, p);

    p[0] = -0.08;   p[1] = 0.0825;
    Calc3x3ROT(0,0,134.1186, m);
    centro2leg_IU = *new Matriz_Transformacion(m,p);

    p[0] = -0.08;   p[1] = -0.0825;
    Calc3x3ROT(0,0,-134.1186, m);
    centro2leg_ID = *new Matriz_Transformacion(m, p);

    p[0] = 0.08;   p[1] = -0.0825;
    Calc3x3ROT(0,0,-45.8814, m);
    centro2leg_DD = centro2leg_DD* *new Matriz_Transformacion(m, p);

    connect(&timer, &QTimer::timeout, this, &trayectoryGenerator::nextOrder);
    timer.start(30);//antes 50ms
}

bool trayectoryGenerator::isMoving(){
    for(auto modulo :ModulesHandler::module_list){
        if(!modulo->mod->objetiveReached()) return true;
    }
    return false;
}

void trayectoryGenerator::setVel(float max_vel, int motor_id){
    RomerinMsg m = romerinMsg_VelocityProfile(motor_id, max_vel);
    for(auto module : ModulesHandler::module_list){
        module->sendMessage(m);
    }
}
void trayectoryGenerator::setMotorAngle(ModuleController *module, double angle[])
{
    // qDebug()<<module->name;
    for(int i = 0; i<6; i++){
        RomerinMsg m = romerinMsg_ServoGoalAngle(i, angle[i]);
        module->sendMessage(m);
        // qDebug()<<"Motor "<< i<< ": "<<angle[i];
    }

}
void trayectoryGenerator::setAdhesion(ModuleController *module, int percentaje)
{
    RomerinMsg m = romerinMsg_SuctionCupPWM(percentaje);
    module->sendMessage(m);
}

bool trayectoryGenerator::validateMovement(double angle[], ModuleController *module, double x, double y, double z, float RPY[3], bool elbow)
{
    double m[6]={}, q[6]= {}, p[3] = {x,y,z}, orientation[3][3];
    Calc3x3ROT(RPY[0], RPY[1], RPY[2], orientation);

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

    for(int i = 0; i< 6; i++){
        angle[i] = m[i];
    }
    return true;
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
bool trayectoryGenerator::moveLeg(QString leg, double x, double y, double z, float RPY[3], bool elbow, bool fixed)
{
    ModuleController *module = ModulesHandler::getWithName(leg);
    double m[6]={}, q[6]= {}, p[3] = {x,y,z}, orientation[3][3];
    Calc3x3ROT(RPY[0], RPY[1], RPY[2], orientation);

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
bool trayectoryGenerator::moveLeg(ModuleController *module, double x, double y, double z, float RPY[3], bool elbow, bool fixed)
{
    // double m[6]={}, q[6]= {}, p[3] = {x,y,z}, orientation[3][3];
    // Calc3x3ROT(RPY[0], RPY[1], RPY[2], orientation);

    // if(!module->mod->romkin.IKfast(q, orientation, p, elbow, true)) {
    //     qDebug()<<"Fuera de rango";
    //     return false;
    // }
    // module->mod->romkin.q2m(m,q);

    // //Check if joint physical limits are not surpassed
    // if(module->mod->checkJointsLimits(m, true)) {
    //     qDebug()<<"Joint limit surpassed";
    //     return false;
    // }

    double m[6]={};
    if(!validateMovement(m, module, x, y, z, RPY, elbow)) return false;

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

void trayectoryGenerator::Calc3x3ROT(float a, float b, float c, double orientacion[][3])
{

    // Convertir grados a radianes
    a /= RomKin::rad2deg;
    b /= RomKin::rad2deg;
    c /= RomKin::rad2deg;

    double cx = cos(a), sx = sin(a);
    double cy = cos(b), sy = sin(b);
    double cz = cos(c), sz = sin(c);

    // Matriz de rotación sobre ejes globales (Rx * Ry * Rz)
    orientacion[0][0] = cy * cz;
    orientacion[0][1] = -cy * sz;
    orientacion[0][2] = sy;

    orientacion[1][0] = sx * sy * cz + cx * sz;
    orientacion[1][1] = -sx * sy * sz + cx * cz;
    orientacion[1][2] = -sx * cy;

    orientacion[2][0] = -cx * sy * cz * + sx * sz;
    orientacion[2][1] = cx * sy * sz + sx * cz;
    orientacion[2][2] = cx * cy;
}

/* New_center in m*/
bool trayectoryGenerator::moveBotAbsolute(Vector3D new_center, float RPY[])
{
    Vector3D diff = new_center - center;
    if(diff.module() > 0.001){
        moveBotRelative(diff, RPY);
        // bool oka = true;
        // for(auto modulo :ModulesHandler::module_list){
        //     double pos[3]{};
        //     modulo->mod->get_pos(pos);
        //     Point_3D obj = pos - diff;
        //     //Point_3D obj = {400,0,0}; obj -= new_center;
        //     //obj/=1000.0;

        //     oka &= moveLeg(modulo,obj.x, obj.y, obj.z,plana ,true, false);
        // }
        // if(oka){
        //     qDebug()<<"Movimiento valido";
        //     center = new_center;
        //     Center.x = new_center.x;
        //     Center.y = new_center.y;
        //     Center.z = new_center.z;
        //     int counter = 0;
        //     while(isMoving() && counter <10){counter ++;}
        // }
        // else{
        //     qDebug()<<"Movimiento no alcanzable";
        // }
        center = new_center;
    }
}

bool trayectoryGenerator::moveBotRelative(Vector3D new_center, float RPY[3])
{
    std::list<MotorsAngles> points;
    Matriz_Transformacion movimiento(new_center);
    bool oka = true;

    for(auto modulo :ModulesHandler::module_list){
        double pos[3]{};
        modulo->mod->get_pos(pos);
        Vector3D TCP = {pos[0], pos[1], pos[2]};

        if(modulo->name == "THOR"){
            Vector3D TCP_global = Transformacion(TCP, centro2leg_DU);
            TCP = Transformacion(TCP_global, (movimiento*centro2leg_DU).Inversa());
        }
        else if(modulo->name == "FRIGG"){
            Vector3D TCP_global = Transformacion(TCP, centro2leg_IU);
            TCP = Transformacion(TCP_global, (movimiento*centro2leg_IU).Inversa());
        }
        else if(modulo->name == "ODIN"){
            Vector3D TCP_global = Transformacion(TCP, centro2leg_ID);
            TCP = Transformacion(TCP_global, (movimiento*centro2leg_ID).Inversa());
        }
        else if(modulo->name == "LOKI"){
            Vector3D TCP_global = Transformacion(TCP, centro2leg_DD);
            TCP = Transformacion(TCP_global, (movimiento*centro2leg_DD).Inversa());
        }
        else
            qDebug()<< "Leg not found";

        //obj /= 1000.0;
        double angle[6];
        oka &= validateMovement(angle, modulo,TCP.x, TCP.y, TCP.z,RPY ,true);

        if(!oka)    return false;
        points.push_back(MotorsAngles(angle));

    }

    for(auto module : ModulesHandler::module_list){
        setMotorAngle(module, points.front().angle);
        points.pop_front();
    }
    return true;
}

void trayectoryGenerator::reset()
{
    qDebug()<<"Reset";
    center = {0,0,0};
    double m[6] = {185,246,197,180,102,-12};
    for(const auto module : ModulesHandler::module_list){
        setMotorAngle(module, m);
        setAdhesion(module, standby);
    }
}
void trayectoryGenerator::stand()
{

    moveBotAbsolute(Vector3D{0,0, 0.2}, def_orientation);
}
void trayectoryGenerator::relax()
{
    moveBotAbsolute(Vector3D{0,0,-0.1}, def_orientation);
    // for(auto modulo :ModulesHandler::module_list){
    //     moveLeg(modulo, 0.5, 0.0, 0.2,RPY,true, false);
    // }
}

bool trayectoryGenerator::nextOrder()
{
    if(order_list.size() == 0) return false;
    if(isMoving()) return false;

    command_t input = *order_list.begin();
    switch(input){
    case command_t::STAND:
        stand();
        break;

    case command_t::RELAX:
        relax();
        break;
    case command_t::RESET:
        reset();
        break;
    case command_t::FIXED_ROTATION:
        break;
    case command_t::MOVE_TO_POINT:
        moveBotRelative(Vector3D{0,0,0}, def_orientation);
        break;
    }
    order_list.pop_front();
    return true;
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


