#include "motor.h"
#include "ui_motor.h"
#include "modulecontroller.h"

Motor::Motor(QWidget *parent, int m_id) : QWidget(parent), ui(new Ui::Motor)
{
    ui->setupUi(this);

    motor_id = m_id;
    ui->txt_title->setText("Motor" + QString::number(motor_id));
}

Motor::~Motor()
{
    delete ui;
}
void Motor::updateInfo(MotorInfoData &minfo)
{
    info_motor=minfo;
    ui->num_angulo->display(QString::number(minfo.position, 'f', 1));
    ui->num_vel->display(QString::number(minfo.velocity, 'f', 1));
    ui->num_torque->display(QString::number((minfo.intensity, 'f', 1)));
    //QDebug info(minfo.intensity);
    //ui->intensity->display(QString::number(minfo.intensity, 'f', 1));
    //ui->temperature->display(minfo.temperature);
    //ui->voltage->display(QString::number(minfo.voltage, 'f', 1));
}
