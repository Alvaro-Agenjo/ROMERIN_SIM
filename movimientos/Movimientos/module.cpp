#include "module.h"
#include "ui_module.h"

Module::Module(QWidget *parent) : QWidget(parent),ui(new Ui::Module), config(this)
{
    ui->setupUi((QWidget *)this);

    for(int i = 0; i<6; i++)
    {
        motors[i]=new Motor((QWidget *)this, i);
        ui->motorsLayOut->addWidget(motors[i], i<3? 0:1, i%3 );
    }

    //------------------------------------Temporal hasta actualización de software---------------------------------//
    config.setConfigInfo(*new ConfigurationInfoV2);
    for(int i= 0; i< 6; i++){
        ConfigurationInfoV2 info = config.getConfigInvoV2();
        FixedMotorInfoData limits; limits.max_angle = info.max[i]; limits.min_angle = info.min[i];
        motors[i]->updateFixedInfo(limits);
    }
    //------------------------------------Temporal hasta actualización de software---------------------------------//
    connect(&timer, &QTimer::timeout, this, &Module::loop);
    timer.start(40);
}
Module::~Module()
{
    delete ui;
}

void Module::setModule(ModuleController *mod)
{
    module=mod;
    for(int i = 0; i<6; i++)motors[i]->setModuleController(module);
}


void Module::loop()
{

}

void Module::updateInfo(SuctionCupInfoData &data)
{
    suction_cup = data;
    //Actualizacion valores adherencia
    ui->lcd_pressure->display(QString::number(data.pressure, 'f', 0));
    ui->lcd_force->display(QString::number(data.force, 'f', 1));
    ui->lcd_percentage->display(QString::number(data.temperature, 'f', 0));


    //Actualizacion proximidad
    ui->lcd_d1->display(QString::number(data.distance[0], 'f', 0));
    ui->lcd_d2->display(QString::number(data.distance[1], 'f', 0));
    ui->lcd_d3->display(QString::number(data.distance[2], 'f', 0));
}

void Module::updateRobotState()
{
    uchar_t robot_cicle_time=module->robot_cicle_time;

    double m[6],q[6]{};
    for(int i = 0; i<6; i++){
        m[i]=motors[i]->get_motor_info().position;
    }
    romkin.m2q(q,m);
    auto pos = romkin.FKwrist(q[0],q[1],q[2]);
    ui->lcd_x->display(QString::number(pos(0)*1000, 'f', 0));
    ui->lcd_y->display(QString::number(pos(1)*1000, 'f', 0));
    ui->lcd_z->display(QString::number(pos(2)*1000, 'f', 0));
}

void Module::get_qs(double *q)
{
    double m[6]{};
    for(int i = 0; i<6; i++)m[i]=motors[i]->get_motor_info().position;
    romkin.m2q(q,m);
}

void Module::get_torques(double *t)
{
    double mi[6]{};
    for(int i = 0; i<6; i++)mi[i]=motors[i]->get_motor_info().intensity;
    //TODO: ver si la info de signo es correcta
    romkin.mt2qt(t,mi);
}

void Module::get_pos(double pos[])
{
    double q[6];
    get_qs(q);
    auto posicion = romkin.FKwrist(q[0],q[1],q[2]);

    for(int i = 0; i< 3; i++){
        pos[i] = posicion(i);
    }
}

void Module::get_pos_TCP(double pos[])
{
    double q[6];
    get_qs(q);
    auto posicion = romkin.FK(q[0],q[1], q[2], q[3], q[4], q[5]);

    for(int i = 0; i< 3; i++){
        pos[i] = posicion(i, 3);
    }
}

void Module::get_motor_info(MotorInfoData *m)
{
    for(int i = 0; i<6; i++)m[i]=motors[i]->get_motor_info();
}

/* Funcion para comprobar que los valores articulares están dentro de límites.
 * Devuelve true si se superan los límites para alguna articulación */
bool Module::checkJointsLimits(double m[], bool simple)
{
    for( int i = 0; i < (6 - 3 * simple); i++){
        if(motors[i]->get_motor_limits().min_angle > m[i] || motors[i]->get_motor_limits().max_angle < m[i]){
            qDebug()<<"Joint "<< i << "limit surpassed";
            return true;
        }
    }
    return false;
}

bool Module::objetiveReached()
{
    for(const Motor *motor : motors){
        if(motor->isMoving()) return false;
    }
    return true;
}

bool Module::newTCP_mov(Vector3D actualTCP, Vector3D *futureTCP, Matriz_Transformacion movimiento)
{

    actualTCP = Transformacion(actualTCP, T);
    *futureTCP = Transformacion(actualTCP, (movimiento*T).Inversa());
    return true;
}

void Module::updateTorque(int motor_id, bool torque)
{
    motors[motor_id]->setTorque(torque);
}

bool Module::isAttached()
{
    uint8_t threshold = 18;
    return (suction_cup.distance[0]<threshold && suction_cup.distance[1] < threshold && suction_cup.distance[2] < threshold);
}




