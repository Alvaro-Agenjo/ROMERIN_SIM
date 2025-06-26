#include "trayectorygenerator.h"

#include "moduleshandler.h"
#include "module.h"
#include <QTime>

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

    //connect(&timer, &QTimer::timeout, this, &trayectoryGenerator::nextOrder);
    //timer.start(50);//antes 50ms
    //millis.start();
}
bool trayectoryGenerator::isMoving(){
    for(auto modulo :ModulesHandler::module_list){
        if(!modulo->mod->objetiveReached()) return true;
    }
    return false;
}

void trayectoryGenerator::setMatrizTransformacion(ModuleController *modulo)
{
    static int id = 0;
    switch(id){
    case 0:
        modulo->mod->setMatrizTransformacion(centro2leg_DU);
        break;
    case 1:
        modulo->mod->setMatrizTransformacion(centro2leg_IU);
        break;
    case 2:
        modulo->mod->setMatrizTransformacion(centro2leg_ID);
        break;
    case 3:
        modulo->mod->setMatrizTransformacion(centro2leg_DU);
        break;
    case 4:
        id = -1;
        //modulo->mod->setMatrizTransformacion();
        break;
    case 5:
        //moulo->mod->setMatrizTransformacion();
        break;
    default:
        id = 0;
        break;
    }

    id++;
}

void trayectoryGenerator::resetTCPs()
{
    double pos[3];
    int n = 0;
    for(auto module : ModulesHandler::module_list){
        module->mod->get_pos_TCP(pos);
        TCPs[n] = pos;
        n++;
    }
}

void trayectoryGenerator::setTorque(ModuleController *modulo, int motor_id, bool torque)
{
    RomerinMsg m = romerinMsg_Torque(motor_id, torque);
    modulo->sendMessage(m);
}
void trayectoryGenerator::setTorque(ModuleController *modulo, bool torques[])
{
    for(int i = 0; i< 6; i++){
        RomerinMsg m = romerinMsg_Torque(i, torques[i]);
        modulo->sendMessage(m);
    }
}
void trayectoryGenerator::setMotorVel(ModuleController *modulo, float max_vel, int motor_id)
{
    RomerinMsg m = romerinMsg_VelocityProfile(motor_id, max_vel);
    modulo->sendMessage(m);
}
void trayectoryGenerator::setMotorVel(ModuleController *modulo, float max_vels[])
{
    for(int i = 0; i< 6; i++){
        RomerinMsg m = romerinMsg_VelocityProfile(i, max_vels[i]);
        modulo->sendMessage(m);
    }
}
void trayectoryGenerator::setMotorAngles(ModuleController *module, double angle[])
{
    QElapsedTimer millis;
    qDebug()<<module->name;
    for(int i = 0; i<6; i++){
        millis.start();
        RomerinMsg m = romerinMsg_ServoGoalAngle(i, angle[i]);
        module->sendMessage(m);
        qDebug()<<"Motor "<< i<< ": "<<angle[i];
        while(millis.elapsed() < 20){}

    }
}
void trayectoryGenerator::setMotorAngles(ModuleController *module, double angle, int motor_id)
{

    RomerinMsg m = romerinMsg_ServoGoalAngle(motor_id, angle);
    module->sendMessage(m);

}
void trayectoryGenerator::setAdhesion(ModuleController *module, int percentaje)
{
    RomerinMsg m = romerinMsg_SuctionCupPWM(percentaje);
    module->sendMessage(m);
}

void trayectoryGenerator::addMovement(ModuleController *module, double angulo[], int suctForce, int batch)
{
    Movimiento new_mov = Movimiento(module, angulo, suctForce, batch);
    orders_list.push_back(new_mov);
}
bool trayectoryGenerator::validateMovement(double angle[], ModuleController *module, double x, double y, double z, bool elbow)
{
    double m[3]={}, q[3]= {};
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

    for(int i = 0; i< 3; i++){
        angle[i] = m[i];
    }
    return true;
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

    validateMovement(m, module, x, y, z, elbow);
    //Sends suction power command if necessary
    RomerinMsg msg;
    if(fixed) msg = romerinMsg_SuctionCupPWM(50);
    else msg = romerinMsg_SuctionCupPWM(standby);
    module->sendMessage(msg);


    //Sends movement commands
    setMotorAngles(module, m);
    // qDebug()<<"Q1: "<<q[0]<<" Q2: "<<q[1]<<" Q3: "<<q[2];

    return true; //Return true movement command successfull
}
bool trayectoryGenerator::moveLeg(QString leg, double x, double y, double z, float RPY[3], bool elbow, bool fixed)
{
    ModuleController *module = ModulesHandler::getWithName(leg);
    double m[6]={};
    if(!validateMovement(m, module, x, y, z, RPY, elbow)) return false;


    //Sends suction power command if necessary
    RomerinMsg msg;
    if(fixed) msg = romerinMsg_SuctionCupPWM(50);
    else msg = romerinMsg_SuctionCupPWM(5);
    module->sendMessage(msg);


    //Sends movement commands
    setMotorAngles(module, m);
    // qDebug()<<"Q1: "<<q[0]<<" Q2: "<<q[1]<<" Q3: "<<q[2];

    return true; //Return true movement command successfull
}
bool trayectoryGenerator::moveLeg(ModuleController *module, double x, double y, double z, float RPY[3], bool elbow, bool fixed)
{
    double m[6]={};
    if(!validateMovement(m, module, x, y, z, RPY, elbow)) return false;

    int power;
    addMovement(module, m,power = fixed? 20 : standby, time + 100.0/40.0);

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

/* New_center in meters */
bool trayectoryGenerator::moveBotAbsolute(Vector3D new_center, float RPY[], int batch)
{
    Vector3D diff = new_center - center;
    if(diff.module() > 0.001){
        moveBotRelative(diff, RPY, batch);
        center = new_center;
    }
}
bool trayectoryGenerator::moveBotRelative(Vector3D new_center, float RPY[3], int batch)
{
    std::list<MotorsAngles> points;
    Matriz_Transformacion movimiento(new_center);
    bool oka = true;

    Vector3D TCPs_it[4];
    int n = 0;
    for(auto modulo :ModulesHandler::module_list){

        // double pos[3]{};
        // modulo->mod->get_pos_TCP(pos);
        // Vector3D TCP = {pos[0], pos[1], pos[2]};

        Vector3D TCP;
        modulo->mod->newTCP_mov(TCPs[n], &TCP, movimiento);
        TCPs[n] = TCP;
        n++;
        // if(modulo->name == legs[0]){
        //     TCP = THOR_TCP;
        //     Vector3D TCP_global = Transformacion(TCP, centro2leg_DU);
        //     TCP = Transformacion(TCP_global, (movimiento*centro2leg_DU).Inversa());
        //     TCPs[0] = TCP;
        // }
        // else if(modulo->name == legs[1]){
        //     TCP = FRIGG_TCP;
        //     Vector3D TCP_global = Transformacion(TCP, centro2leg_IU);
        //     TCP = Transformacion(TCP_global, (movimiento*centro2leg_IU).Inversa());
        //     TCPs[1] = TCP;
        // }
        // else if(modulo->name == legs[2]){
        //     TCP = ODIN_TCP;
        //     Vector3D TCP_global = Transformacion(TCP, centro2leg_ID);
        //     TCP = Transformacion(TCP_global, (movimiento*centro2leg_ID).Inversa());
        //     TCPs[2] = TCP;
        // }
        // else if(modulo->name == legs[3]){
        //     TCP = LOKI_TCP;
        //     Vector3D TCP_global = Transformacion(TCP, centro2leg_DD);
        //     TCP = Transformacion(TCP_global, (movimiento*centro2leg_DD).Inversa());
        //     TCPs[3] = TCP;
        // }
        // else
        //     qDebug()<< "Leg not found";

        //obj /= 1000.0;
        double angle[6];
        oka &= validateMovement(angle, modulo,TCP.x, TCP.y, TCP.z,RPY ,true);
        if(!oka)    return false;
        points.push_back(MotorsAngles(angle));
    }

    for(auto module : ModulesHandler::module_list){

        addMovement(module, points.front().angle, 5, batch );
        // setMotorAngle(module, points.front().angle);
        points.pop_front();
    }
    //THOR_TCP = TCPs[0]; FRIGG_TCP = TCPs[1]; ODIN_TCP = TCPs[2]; LOKI_TCP = TCPs[3];
    return true;
}

void trayectoryGenerator::reset()
{

    qDebug()<<"Reset";
    center = {0,0,0};
    double m[6] = {185,246,197,180,102,-12};
    for(const auto module : ModulesHandler::module_list){
        setMotorAngles(module, m);
        setAdhesion(module, standby);
    }
    resetTCPs();
}
void trayectoryGenerator::stand()
{
    constexpr int sec = 1500;
    long request_time = time + (sec/40.0); //3s at 40ms interval
    float def[3] = {0,180,0};
    Vector3D up{0,0,0.2};
    double pos[3];

    resetTCPs();


    for(int i= 0; i< sec/100.0; i++){
        //moveBotAbsolute(up, def, i);
        moveBotRelative(up/(sec/100), def, request_time + counterTG2MW * i);
    }
}
void trayectoryGenerator::relax()
{
    constexpr int sec = 3000;
    long request_time = time + (sec/40.0); //3s at 40ms interval
    float def[3] = {0,180,0};
    Vector3D down{0,0,-0.2};
    double pos[3];
    // ModulesHandler::getWithName(legs[0])->mod->get_pos_TCP(pos);
    // THOR_TCP = pos;

    // ModulesHandler::getWithName(legs[1])->mod->get_pos_TCP(pos);
    // FRIGG_TCP = pos;

    // ModulesHandler::getWithName(legs[2])->mod->get_pos_TCP(pos);
    // ODIN_TCP = pos;

    // ModulesHandler::getWithName(legs[3])->mod->get_pos_TCP(pos);
    // LOKI_TCP = pos;


    for(int i= 0; i< sec/100.0; i++){
        //moveBotAbsolute(down, def, i);
        moveBotRelative(down/(sec/100), def, request_time + counterTG2MW * i);
    }
}

bool trayectoryGenerator::nextOrder()
{
    static long last_time=0;
    if(time - last_time < 100.0/40.0)  return false;

    last_time = time;

    if(orders_list.size() == 0) return false;
    //if(isMoving()) return false;

    Movimiento movement = orders_list.front();
    int num = movement.time_code;

    setAdhesion(movement.module, movement.suctionPercentaje);
    setMotorAngles(movement.module, movement.angulos);

    orders_list.pop_front();
    while(orders_list.front().time_code == num){
        movement = orders_list.front();
        setAdhesion(movement.module, movement.suctionPercentaje);
        setMotorAngles(movement.module, movement.angulos);

        orders_list.pop_front();
    }

    //------------------------------------V1------------------------------------------//
    // if(orders_list.size() == 0) return false;
    // if(isMoving()) return false;

    // Movimiento movement = orders_list.front();
    // if(movement.type_name)
    //     moveLeg(movement.name, movement.pos.x, movement.pos.y, movement.pos.z, movement.rot, movement.elbow, movement.fixed);
    // else
    //     moveLeg(movement.module, movement.pos.x, movement.pos.y, movement.pos.z, movement.rot, movement.elbow, movement.fixed);

    // orders_list.pop_front();




    //------------------------------------V0------------------------------------------//
    // command_t input = *orders_list.begin();
    // switch(input){
    // case command_t::STAND:
    //     stand();
    //     break;

    // case command_t::RELAX:
    //     relax();
    //     break;
    // case command_t::RESET:
    //     reset();
    //     break;
    // case command_t::FIXED_ROTATION:
    //     break;
    // case command_t::MOVE_TO_POINT:
    //     //moveBotRelative(Vector3D{0,0,0}, def_orientation);
    //     break;
    // }
    // order_list.pop_front();
    return true;
}


Movimiento::Movimiento(ModuleController *module, double angulos[], int suctforce, int batch)
{
    this->module = module;
    suctionPercentaje = suctforce;
    for(int i= 0; i<6; i++){
        this->angulos[i] = angulos[i];
    }
    time_code = batch;
}
Movimiento::Movimiento(ModuleController *module, double angulos[], double vel[], int suctforce, int batch)
{
    this->module = module;
    suctionPercentaje = suctforce;
    for(int i= 0; i<6; i++){
        this->angulos[i] = angulos[i];
        this->vel[i] = vel[i];
    }
    time_code = batch;
}
