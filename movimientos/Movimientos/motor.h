#ifndef MOTOR_H
#define MOTOR_H

#include <QWidget>

#include "../include/RomerinMessage.h"


namespace Ui {
class Motor;
}

class ModuleController;

class Motor : public QWidget
{
    Q_OBJECT

public:
    explicit Motor(QWidget *parent = nullptr, int m_id = 0);
    ~Motor();
    void updateInfo(MotorInfoData &minfo);
    void updateFixedInfo(FixedMotorInfoData &minfo){limits_motor = minfo;}

    void setModuleController(ModuleController *module){_module=module;}

    MotorInfoData get_motor_info(){return info_motor;}
    FixedMotorInfoData get_motor_limits(){return limits_motor;}
    void setTorque(bool check);

    bool isMoving() const {return moving;}


private slots:

    void on_chk_torque_clicked(bool checked);

private:
    Ui::Motor *ui;
    ModuleController *_module;

    MotorInfoData info_motor;
    FixedMotorInfoData limits_motor;    
    int motor_id;
    bool moving = false;
    bool torque = false;
};

#endif // MOTOR_H
