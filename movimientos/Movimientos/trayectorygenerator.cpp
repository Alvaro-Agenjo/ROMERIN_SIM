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
        modulo->mod->setMatrizTransformacion(centro2leg_DD);
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

void trayectoryGenerator::refreshTCPs()
{
    double pos[3];
    int n = 0;
    for(auto module : ModulesHandler::module_list){
        module->mod->get_pos_TCP(pos);
        TCPs[n] = pos;
        n++;
    }
    //center = 0,0,0;
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
        RomerinMsg m = romerinMsg_ServoGoalAngle(i, angle[i]);
        module->sendMessage(m);
        qDebug()<<"Motor "<< i<< ": "<<angle[i];
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


    /* CHANGE
    * Provisional hasta resolver problema con angulos muñeca
    */
    //Check if joint physical limits are not surpassed
    //if(module->mod->checkJointsLimits(m, false))    return false;
    if(module->mod->checkJointsLimits(m, true))    return false;

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
bool trayectoryGenerator::moveBotAbsolute(Vector3D new_center, float RPY[], int tiempo)
{
    Vector3D diff = new_center - center;
    int n_orders = tiempo / 100.0; // 100ms per order
    unsigned long request_time = (orders_list.size() == 0) ? time : orders_list.back().time_code; // (ms/40.0);
    if(diff.module() > 0.01){
        for(int i= 0; i< n_orders; i++){
            if(!moveBotRelative(diff/n_orders, RPY, request_time + (i + 1) * counterTG2MW));  //und40 * 40ms/und40 + (i+1)*100ms) return false;
        }
        center = new_center;
        return true;
    }
    return true;
}
bool trayectoryGenerator::moveBotRelative(Vector3D new_center, float RPY[3], int batch)
{
    std::list<MotorsAngles> points;
    Matriz_Transformacion movimiento(new_center);
    bool oka = true;

    int n = 0;
    for(auto modulo :ModulesHandler::module_list){
        Vector3D TCP;
        modulo->mod->newTCP_mov(TCPs[n], &TCP, movimiento);
        TCPs[n] = TCP;
        n++;

        double angle[6];
        oka &= validateMovement(angle, modulo,TCP.x, TCP.y, TCP.z,RPY ,true);
        if(!oka)    return false;
        points.push_back(MotorsAngles(angle));
    }

    for(auto module : ModulesHandler::module_list){

        addMovement(module, points.front().angle, 5, batch );
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
        setMotorAngles(module, m);
        setAdhesion(module, standby);
    }
    refreshTCPs();
}
void trayectoryGenerator::stand()
{
    constexpr int ms = 2000;
    unsigned long request_time = (orders_list.size() == 0) ? time : orders_list.back().time_code; // (ms/40.0);
    int n_orders = ms /100.0;
    float def[3] = {0,180,0};
    Vector3D up{0,0,0.2};
    double pos[3];

    refreshTCPs();


    for(int i= 0; i< n_orders; i++){
        //moveBotAbsolute(up, def, i);
        moveBotRelative(up/n_orders, def, request_time + (i + 1) * counterTG2MW );  //und40 * 40ms/und40 + (i+1)*100ms
    }
    center = center + up;
}
void trayectoryGenerator::relax()
{
    constexpr int ms = 3000;
    unsigned long request_time = (orders_list.size() == 0) ? time : orders_list.back().time_code; // (ms/40.0);
    int n_orders = ms /100.0;
    float def[3] = {0,180,0};
    Vector3D up{0,0,-0.2};
    double pos[3];

    refreshTCPs();


    for(int i= 0; i< ms/100.0; i++){
        //moveBotAbsolute(up, def, i);
        moveBotRelative(up/n_orders, def, request_time + (i + 1) * counterTG2MW );  //und40 * 40ms/und40 + (i+1)*100ms
    }
}

void trayectoryGenerator::fixed_rotation(int n)
{
    refreshTCPs();

    constexpr double r = 0.1;
    Vector3D pos = {r, 0, center.z};
    float RPY[3] = {0,180,0};
    moveBotAbsolute(pos, RPY, 1000);

    for(int i = 0; i < n; i++ ){
        for(int m = 1; m < 361; m+= 5 ){
            pos.x = r * cos(m / RomKin::rad2deg); pos.y = r * sin(m / RomKin::rad2deg);
            moveBotAbsolute(pos, RPY, 100);
        }
    }
    pos.x = 0; pos.y = 0;
    moveBotAbsolute(pos, RPY, 2000);
}

bool trayectoryGenerator::nextOrder()
{
    static unsigned long last_time=0;
    if(time - last_time < 100.0/40.0)  return false;    //und40 * ms/und40 < ms <====> und40 < ms/ms

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
