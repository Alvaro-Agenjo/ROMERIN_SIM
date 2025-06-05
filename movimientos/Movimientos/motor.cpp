#include "motor.h"
#include "ui_motor.h"
#include "modulecontroller.h"
#include "romkin.h"

Motor::Motor(QWidget *parent, int m_id) : QWidget(parent), ui(new Ui::Motor)
{
    ui->setupUi(this);

    motor_id = m_id;
    ui->txt_title->setAlignment(Qt::AlignCenter);
    ui->txt_title->setText("Motor " + QString::number(motor_id + 1));
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

    float torque = minfo.intensity;
    if(minfo.id == 0) torque/= (4.0 / 6.0);  // 4A / 6Nm
    else if( minfo.id < 3)  torque= torque * RomKin::factor_4 / (5.0/10.0); // 5A / 10 Nm
    else  torque = torque * RomKin::factor_6 / (1.4/1.9); // 1.4A / 1.9Nm

    torque /= 1000.0; //conversion to Nm
    ui->num_torque->display(QString::number(torque, 'f', 2));
    //QDebug info(minfo.intensity);
    //ui->intensity->display(QString::number(minfo.intensity, 'f', 1));
    //ui->temperature->display(minfo.temperature);
    //ui->voltage->display(QString::number(minfo.voltage, 'f', 1));
}
