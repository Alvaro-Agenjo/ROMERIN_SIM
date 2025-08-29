#include "trayectorygenerator.h"

#include "moduleshandler.h"
#include "module.h"
#include <QTime>

trayectoryGenerator::trayectoryGenerator() {
    //-------------------------Temporal---------------------//
    //Hasta que se haya implementado el guardado de la posicion y orientacion del módulo respecto al centro
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
}
bool trayectoryGenerator::isMoving(){
    for(auto modulo :ModulesHandler::module_list){
        if(!modulo->mod->objetiveReached()) return true;
    }
    return false;
}

void trayectoryGenerator::setMatrizTransformacion(ModuleController *modulo)
{
    //-------------------------Temporal---------------------//
    //Hasta que se haya implementado el guardado de la posicion y orientacion del módulo respecto al centro

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

void trayectoryGenerator::setTorque(ModuleController *modulo, int motor_id, const bool torque)
{
    modulo->mod->updateTorque(motor_id,torque);
}
void trayectoryGenerator::setTorque(ModuleController *modulo, const bool torques[])
{
    modulo->mod->updateTorque(torques);
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
void trayectoryGenerator::setMotorAngles(ModuleController *module, double angle, int motor_id)
{
    RomerinMsg m = romerinMsg_ServoGoalAngle(motor_id, angle);
    module->sendMessage(m);
}
void trayectoryGenerator::setMotorAngles(ModuleController *module, double angle[])
{
    qDebug()<<module->name;
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



void trayectoryGenerator::addMovement(ModuleController *module, double angulo[], int suctForce, int batch, bool full)
{
    Movimiento new_mov = Movimiento(module, angulo, suctForce, batch, full);
    orders_list.push_back(new_mov);
}
bool trayectoryGenerator::validateMovement(double angle[], ModuleController *module, double x, double y, double z, bool elbow)
{
    double m[6]={}, q[3]= {}, q_last[6];
    module->mod->get_qs(q_last);

    //Obtencion de valores articulares y comprobacion de rango
    if(!module->mod->romkin.IKwrist(q,x, y, z, elbow)){
        qDebug()<<"Out of range position";
        return false;
    }

    //Mantenimiento de orientacion de la muñeca
    for(int i = 0; i< 3; i++){
        q_last[i] = q[i];
    }

    // Conversion q->m
    module->mod->romkin.q2m(m,q_last, false);

    //Check if joint physical limits are not surpassed
    if(module->mod->checkJointsLimits(m, true)) {
        qDebug()<<"Joint limit surpassed";
        return false;
    }

    //Escritura de resultados
    for(int i = 0; i< 6; i++){
        angle[i] = m[i];
    }
    return true;
}
bool trayectoryGenerator::validateMovement(double angle[], ModuleController *module, double x, double y, double z, float RPY[3], bool elbow)
{
    double m[6]={}, q[6]= {}, p[3] = {x,y,z}, orientation[3][3];
    Calc3x3ROT(RPY[0], RPY[1], RPY[2], orientation);

    //Obtencion de valores articulares y comprobacion de rango
    if(!module->mod->romkin.IKfast(q, orientation, p, elbow, true)) {
        qDebug()<<"Fuera de rango";
        return false;
    }
    // for(int i= 0; i<6; i++){
    //     qDebug()<<"Q"<<i+1<<" = "<<q[i]* RomKin::rad2deg;
    // }

    // Conversion q->m
    module->mod->romkin.q2m(m,q);


    /************************* CHANGE *******************************
    * Provisional hasta resolver problema con angulos muñeca        *
    * if(module->mod->checkJointsLimits(m, true))    return false;  *
    ****************************************************************/
    //Check if joint physical limits are not surpassed
    if(module->mod->checkJointsLimits(m, false))    return false;

    //Escritura de resultados
    for(int i = 0; i< 6; i++){
        angle[i] = m[i];
    }
    return true;
}

/* Funcion de test probablemente será eliminada ya que no se usa en movimientos precargados */
bool trayectoryGenerator::moveLeg(QString leg, double x, double y, double z, bool elbow, bool fixed)
{
    ModuleController *module = ModulesHandler::getWithName(leg);
    //deactivate wrist motors
    module->mod->updateTorque(simple);

    //check movement
    double m[6];
    validateMovement(m, module, x, y, z, elbow);

    //Sends suction power command if necessary
    RomerinMsg msg;
    if(fixed)   msg = romerinMsg_SuctionCupPWM(operating);
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
    //reactivate wrist motors
    module->mod->updateTorque(full);

    double m[6]={};
    if(!validateMovement(m, module, x, y, z, RPY, elbow)) return false;

    //Sends suction power command if necessary
    RomerinMsg msg;
    if(fixed)   msg = romerinMsg_SuctionCupPWM(operating);
    else msg = romerinMsg_SuctionCupPWM(standby);
    module->sendMessage(msg);


    //Sends movement commands
    setMotorAngles(module, m);
    // qDebug()<<"Q1: "<<q[0]<<" Q2: "<<q[1]<<" Q3: "<<q[2];

    return true; //Return true movement command successfull
}
bool trayectoryGenerator::moveLeg(ModuleController *module, double x, double y, double z, float RPY[], bool elbow, bool fixed)
{
    double m[6]={};
    if(!validateMovement(m, module, x, y, z, RPY, elbow)) return false;

    int power;
    addMovement(module, m,power = fixed? operating : standby, time + 100.0/40.0, true);

    return true; //Return true movement command successfull
}

/* New_center in meters */
bool trayectoryGenerator::moveBotAbsolute(Vector3D new_center, float RPY[], int tiempo, bool fixed)
{
    refreshTCPs();
    Vector3D diff = new_center - center;
    if(!chopper(diff, RPY, tiempo, fixed))
        return false;
    center = new_center;
    return true;
}

bool trayectoryGenerator::moveBotRelative(Vector3D desplazamiento, float RPY[], int tiempo, bool fixed)
{
    refreshTCPs();
    return chopper(desplazamiento, RPY, tiempo, fixed);
}

bool trayectoryGenerator::chopper(Vector3D coord, float RPY[],int tiempo, bool fixed )
{
    int n_orders = tiempo / 100.0; // 100ms per order
    unsigned long request_time = (orders_list.size() == 0) ? time : orders_list.back().time_code;
    if(coord.module() > 0.01){
        for(int i= 0; i< n_orders; i++){
            //und40 * 40ms/und40 + (i+1)*100ms)
            if(!moveBot(coord/n_orders, RPY, request_time + (i + 1) * counterTG2MW, fixed))
                qDebug() << "Movimiento no valido";
        }
        return true;
    }
    return false;
}
bool trayectoryGenerator::moveBot(Vector3D new_center, float RPY[3], int batch, bool fixed)
{
    Matriz_Transformacion movimiento(new_center);   //movimiento a aplicar sobre el centro del cuerpo

    std::list<MotorsAngles> points; //variable para almacenar las posiciones de los motores de todos los módulos
    bool oka = true;    //indicador de movimientos alcanzables

    Vector3D newTCPs[4];
    int n = 0;
    for(auto modulo :ModulesHandler::module_list){
        //Se calcula las nuevas coordenadas del TCP
        Vector3D TCP;
        modulo->mod->newTCP_mov(TCPs[n], &TCP, movimiento);
        newTCPs[n] = TCP;
        n++;

        //Validacioón del movimiento
        double angle[6];
        oka &= validateMovement(angle, modulo,TCP.x, TCP.y, TCP.z,RPY ,true);

        //En caso de que algún módulo no pueda completar el movimiento, finalizar ejecucion.
        if(!oka)    return false;
        //Si la orden es posible, añadir el movimiento
        else    points.push_back(MotorsAngles(angle));
    }

    //una vez comprobadas todas las ordenes poner en cola para su realizacion
    n = 0;
    for(auto module : ModulesHandler::module_list){
        addMovement(module, points.front().angle, fixed? operating : standby , batch, true);
        points.pop_front();
        TCPs[n] = newTCPs[n];
        n++;
    }
    return true;
}

void trayectoryGenerator::reset()
{
    qDebug()<<"Reset";
    center = {0,0,0};
    double m[6] = {180,257,200,180,123,236};
    for(const auto module : ModulesHandler::module_list){
        setTorque(module, full);
        setMotorAngles(module, m);
        setAdhesion(module, standby);
    }
    refreshTCPs();
}
void trayectoryGenerator::stand()
{
    constexpr int ms = 2000;
    float rot[3] = {0,180,90};
    Vector3D up{0,0,0.2};

    refreshTCPs();

    moveBotRelative(up, rot, ms, false);
}
void trayectoryGenerator::relax()
{
    constexpr int ms = 3000;

    float rot[3] = {0,180,90};
    Vector3D up{0,0,-0.2};

    refreshTCPs();

    moveBotRelative(up, rot, ms, false);
}

void trayectoryGenerator::fixed_rotation(int n)
{
    refreshTCPs();

    constexpr double r = 0.08;
    Vector3D pos = {r, 0, center.z};
    float RPY[3] = {0,180,90};
    moveBotAbsolute(pos, RPY, 1000);

    for(int i = 0; i < n; i++ ){
        for(int m = 1; m <= 360; m+= 10 ){ //0-360, 5
            pos.x = r * cos(m / RomKin::rad2deg); pos.y = r * sin(m / RomKin::rad2deg);
            moveBotAbsolute(pos, RPY, 200); //100
        }
    }
    pos.x = 0; pos.y = 0;
    moveBotAbsolute(pos, RPY, 2000);
}

void trayectoryGenerator::nextOrder()
{
    static unsigned long last_time=0;
    if(time - last_time < 100.0/40.0)  return;    //und40 * ms/und40 < ms <====> und40 < ms/ms
    last_time = time;

    //if(orders_list.empty()) return false;
    while(!orders_list.empty() && orders_list.front().time_code <= last_time){
        Movimiento movement = orders_list.front();

        if(movement.suctionPercentaje != standby && movement.module->mod->isAttached()){
            setTorque(movement.module, simple);
            setAdhesion(movement.module, movement.suctionPercentaje);
        }
        else if(!movement.completo){
            setTorque(movement.module, simple);
            setAdhesion(movement.module, standby);
        }
        else {
            setTorque(movement.module, full);
            setAdhesion(movement.module, standby);
        }
        setMotorAngles(movement.module, movement.angulos);
        orders_list.pop_front();
    }
}




Movimiento::Movimiento(ModuleController *module, double angulos[], int suctforce, int batch, bool full)
{
    this->module = module;
    suctionPercentaje = suctforce;
    for(int i= 0; i<6; i++){
        this->angulos[i] = angulos[i];
    }
    time_code = batch;
    completo = full;
}
Movimiento::Movimiento(ModuleController *module, double angulos[], double vel[], int suctforce, int batch, bool full)
{
    this->module = module;
    suctionPercentaje = suctforce;
    for(int i= 0; i<6; i++){
        this->angulos[i] = angulos[i];
        this->vel[i] = vel[i];
    }
    time_code = batch;
    completo = full;
}
