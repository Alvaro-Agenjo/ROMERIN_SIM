#include "trayectorygenerator.h"

#include "moduleshandler.h"
#include "module.h"


trayectoryGenerator::trayectoryGenerator() {
    center = {0,0,-100};
    up_left = {-0.6961458360738, 0.7179003934511,-0.1};
    up_right = {0.6961458360738, 0.7179003934511,-0.1};
    down_left = {-0.6961458360738, -0.7179003934511,-0.1};
    down_right = {0.6961458360738, -0.7179003934511,-0.1};

    connect(&timer, &QTimer::timeout, this, &trayectoryGenerator::nextOrder);
    timer.start(30);//antes 50ms
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
    orientacion[0][1] = sy * sz;
    orientacion[0][2] = -sy;

    orientacion[1][0] = -sz * cx +cz * sx * sy;
    orientacion[1][1] = cz * cx + sz * sy * sx;
    orientacion[1][2] = sx * cy;

    orientacion[2][0] = sz * sx + cz * sy * cx;
    orientacion[2][1] = -cz * sx + sz * sy * cx;
    orientacion[2][2] = cx * cy;
}

void trayectoryGenerator::moveBot(Point_3D new_center)
{

    Point_3D diff = new_center - center;
    diff /= 1000.0;

    if(diff.module() > 0.008){
        float plana[3] = {0, -180,0};
        // up_left += diff; up_right += diff;
        // down_left += diff; down_right += diff;
        bool oka = true;
        for(auto modulo :ModulesHandler::module_list){
            double pos[3]{};
            modulo->mod->get_pos(pos);
            Point_3D obj = pos - diff;
            //Point_3D obj = {400,0,0}; obj -= new_center;
            //obj/=1000.0;

            oka &= moveLeg(modulo,obj.x, obj.y, obj.z,plana ,true, false);
        }
        if(oka){
            qDebug()<<"Movimiento valido";
            center = new_center;
            int counter = 0;
            while(isMoving() && counter <10){counter ++;}
        }
        else{
            qDebug()<<"Movimiento no alcanzable";
        }
    }
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
    moveBot(Point_3D{0,0, 200});
}
void trayectoryGenerator::relax()
{
    moveBot(Point_3D{0,0,-100});
    // for(auto modulo :ModulesHandler::module_list){
    //     moveLeg(modulo, 0.5, 0.0, 0.2,RPY,true, false);
    // }
}

bool trayectoryGenerator::nextOrder()
{
    if(order_list.size() == 0) return false;
    if(isMoving()) return false;

    order_t input = *order_list.begin();
    switch(input){
    case order_t::STAND:
        stand();
        break;

    case order_t::RELAX:
        relax();
        break;
    case order_t::RESET:
        reset();
        break;
    case order_t::FIXED_ROTATION:
        break;
    case order_t::MOVE_TO_POINT:
        moveBot(Point_3D{0,0,0});
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
bool trayectoryGenerator::isMoving(){
    for(auto modulo :ModulesHandler::module_list){
        if(!modulo->mod->objetiveReached()) return true;
    }
    return false;
}

