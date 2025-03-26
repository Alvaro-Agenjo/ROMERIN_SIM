#include "motorwidget.h"
#include "ui_motorwidget.h"
#include "btserialport.h"

MotorWidget::MotorWidget(QWidget *parent, int m_id) :
    QWidget(parent),
    ui(new Ui::MotorWidget)
{
    ui->setupUi(this);
   updateMotorStatus();
   connect(ui->Torque, SIGNAL(toggled(bool)), this, SLOT(on_pushButton_toggled(bool)));
   connect(ui->slider_angle, SIGNAL(sliderMoved(int)), this, SLOT(on_angleSlider_sliderMoved(int)));
   connect(ui->dial_speed,SIGNAL(valueChanged(int)),this, SLOT(on_dial_speed_valueChanged(int)));
   motor_id=m_id;
}

MotorWidget::~MotorWidget()
{
    delete ui;
}
void MotorWidget::updateInfo(MotorInfo &minfo)
{
    info_motor=minfo;
    ui->angle->display(QString::number(minfo.position, 'f', 1));
    ui->velocity->display(QString::number(minfo.velocity, 'f', 1));
    ui->intensity->display(QString::number(minfo.intensity, 'f', 1));
    ui->temperature->display(minfo.temperature);
    ui->voltage->display(QString::number(minfo.voltage, 'f', 1));
     if(!ui->Torque->isChecked())ui->slider_angle->setValue((int)minfo.position);
   updateMotorStatus();
}
void MotorWidget::updateFixedInfo(FixedMotorInfo &minfo)
{
    ui->slider_angle->setMaximum(minfo.max_angle);
    ui->slider_angle->setMinimum(minfo.min_angle);
    ui->label_max->setText(QString::number(int(minfo.max_angle)));
    ui->label_min->setText(QString::number(int(minfo.min_angle)));


}
void MotorWidget::updateMotorStatus(){
    const char S_OK[]="QLabel { background-color: green; color: white; }";
    const char S_ER[]="QLabel { background-color: red; color: white; }";
    if(motor_id<4){//MX64
        ui->VoltageError->setStyleSheet((info_motor.status&0x01?S_ER:S_OK));
        ui->AngleError->setStyleSheet(info_motor.status&0x02?S_ER:S_OK);
        ui->TemperatureError->setStyleSheet(info_motor.status&0x04?S_ER:S_OK);
        ui->RangeError->setStyleSheet(info_motor.status&0x08?S_ER:S_OK);
        ui->ChecksumError->setStyleSheet(info_motor.status&0x10?S_ER:S_OK);
        ui->OverloadError->setStyleSheet(info_motor.status&0x20?S_ER:S_OK);
        ui->InstructionError->setStyleSheet(info_motor.status&0x40?S_ER:S_OK);
    }else
    {
        ui->VoltageError->setStyleSheet((info_motor.status&0x04?S_ER:S_OK));
        ui->TemperatureError->setStyleSheet(info_motor.status&0x02?S_ER:S_OK);
        ui->OverloadError->setStyleSheet(info_motor.status&0x01?S_ER:S_OK);
    }
    /*if(info_motor.temperature<40)ui->temperature->setPalette(Qt::black);
    else if(info_motor.temperature<45)ui->temperature->setPalette(Qt::yellow);
    else ui->temperature->setPalette(Qt::red);*/
    if((info_motor.status&0x80)&&(!ui->button_track->isChecked())){
        ui->Torque->setText("Torque OFF");
        ui->Torque->setChecked(true);
    }
    if((info_motor.status&0x80==0x00)&&(ui->button_track->isChecked())){
        ui->Torque->setText("Torque ON");
        ui->Torque->setChecked(false);
    }

}

void MotorWidget::on_pushButton_toggled(bool checked)
{
    QByteArray m;
     ui->Torque->setText(checked?"Torque OFF":"Torque ON");
    _port->sendTorqueMessage(motor_id,checked);
}

void MotorWidget::on_angleSlider_sliderMoved(int position)
{

    ui->lcd_goal->display(position);
    if(ui->button_track->isChecked())_port->sendGoalAngle(motor_id,position);
}

void MotorWidget::on_button_track_toggled(bool checked)
{
    ui->slider_angle->setValue((int)(info_motor.position));
    ui->button_go->setEnabled(!checked);
}

void MotorWidget::on_button_go_clicked()
{
    _port->sendGoalAngle(motor_id,ui->slider_angle->value());
}

void MotorWidget::on_button_reboot_clicked()
{
    _port->sendReboot(motor_id);
}

void MotorWidget::on_dial_speed_valueChanged(int value)
{
     ui->lcd_velocity->display(value);
    _port->sendVelocityProfile(motor_id,value);
}
