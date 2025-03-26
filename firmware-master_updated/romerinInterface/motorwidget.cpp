#include "motorwidget.h"
#include "ui_motorwidget.h"
#include "modulecontroller.h"

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
void MotorWidget::updateInfo(MotorInfoData &minfo)
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
void MotorWidget::updateFixedInfo(FixedMotorInfoData &minfo)
{
    ui->slider_angle->setMaximum(minfo.max_angle);
    ui->slider_angle->setMinimum(minfo.min_angle);
    ui->label_max->setText(QString::number(int(minfo.max_angle)));
    ui->label_min->setText(QString::number(int(minfo.min_angle)));


}

#include "mainwindow.h"
void MotorWidget::updateMotorStatus(){
    auto &red=MainWindow::red_pal;
    auto &green=MainWindow::green_pal;
    const char S_OK[]="QLabel { background-color: green; color: white; }";
    const char S_ER[]="QLabel { background-color: red; color: white; }";
ui->VoltageError->setAutoFillBackground(true);
ui->AngleError->setAutoFillBackground(true);
        ui->TemperatureError->setAutoFillBackground(true);
        ui->RangeError->setAutoFillBackground(true);
        ui->ChecksumError->setAutoFillBackground(true);
        ui->OverloadError->setAutoFillBackground(true);
        ui->InstructionError->setAutoFillBackground(true);
    if(motor_id<4){//MX64

        ui->VoltageError->setPalette(info_motor.status&0x01?red:green);
        ui->AngleError->setPalette(info_motor.status&0x02?red:green);
        ui->TemperatureError->setPalette(info_motor.status&0x04?red:green);
        ui->RangeError->setPalette(info_motor.status&0x08?red:green);
        ui->ChecksumError->setPalette(info_motor.status&0x10?red:green);
        ui->OverloadError->setPalette(info_motor.status&0x20?red:green);
        ui->InstructionError->setPalette(info_motor.status&0x40?red:green);
        /*ui->VoltageError->setStyleSheet((info_motor.status&0x01?S_ER:S_OK));
        ui->AngleError->setStyleSheet(info_motor.status&0x02?S_ER:S_OK);
        ui->TemperatureError->setStyleSheet(info_motor.status&0x04?S_ER:S_OK);
        ui->RangeError->setStyleSheet(info_motor.status&0x08?S_ER:S_OK);
        ui->ChecksumError->setStyleSheet(info_motor.status&0x10?S_ER:S_OK);
        ui->OverloadError->setStyleSheet(info_motor.status&0x20?S_ER:S_OK);
        ui->InstructionError->setStyleSheet(info_motor.status&0x40?S_ER:S_OK);*/
    }else
    {
        ui->VoltageError->setPalette((info_motor.status&0x04?red:green));
        ui->TemperatureError->setPalette(info_motor.status&0x02?red:green);
        ui->OverloadError->setPalette(info_motor.status&0x01?red:green);
        /*ui->VoltageError->setStyleSheet((info_motor.status&0x04?S_ER:S_OK));
        ui->TemperatureError->setStyleSheet(info_motor.status&0x02?S_ER:S_OK);
        ui->OverloadError->setStyleSheet(info_motor.status&0x01?S_ER:S_OK);*/
    }
ui->temperature->setAutoFillBackground(true);
    if(info_motor.temperature<45)ui->temperature->setPalette(green);
    else ui->temperature->setPalette(red);
    if(info_motor.status&0x80){ui->Torque->setText("Torque OFF");
        ui->Torque->setChecked(true);}
    else {ui->Torque->setText("Torque ON");
        ui->Torque->setChecked(false);}

    /*if((info_motor.status&0x80)&&(!ui->button_track->isChecked())){
        ui->Torque->setText("Torque OFF");
        ui->Torque->setChecked(true);
    }
    if(((info_motor.status&0x80)==0x00)&&(ui->button_track->isChecked())){
        ui->Torque->setText("Torque ON");
        ui->Torque->setChecked(false);
    }*/

}

void MotorWidget::on_pushButton_toggled(bool checked)
{
     //ui->Torque->setText(checked?"Torque OFF":"Torque ON");
     //_module->sendMessage(romerinMsg_Torque(motor_id,info_motor.status&0x80?false:true));

}

void MotorWidget::on_angleSlider_sliderMoved(int position)
{

    ui->lcd_goal->display(position);
    if(ui->button_track->isChecked())
        _module->sendMessage(romerinMsg_ServoGoalAngle(motor_id,position));
}

void MotorWidget::on_button_track_toggled(bool checked)
{
    ui->slider_angle->setValue((int)(info_motor.position));
    ui->button_go->setEnabled(!checked);
}

void MotorWidget::on_button_go_clicked()
{
    _module->sendMessage(romerinMsg_ServoGoalAngle(motor_id,ui->slider_angle->value()));
}

void MotorWidget::on_button_reboot_clicked()
{
    _module->sendMessage(romerinMsg_ServoReboot(motor_id));
}

void MotorWidget::on_dial_speed_valueChanged(int value)
{
     ui->lcd_velocity->display(value);
     _module->sendMessage(romerinMsg_VelocityProfile(motor_id,value));

}

void MotorWidget::on_Torque_clicked()
{
    _module->sendMessage(romerinMsg_Torque(motor_id,info_motor.status&0x80?false:true));
}
