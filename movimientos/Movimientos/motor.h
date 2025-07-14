#ifndef MOTOR_H
#define MOTOR_H

#include <QWidget>

namespace Ui {
class Motor;
}
#include "../include/RomerinMessage.h"
class ModuleController;

class Motor : public QWidget
{
    Q_OBJECT

public:
    explicit Motor(QWidget *parent = nullptr, int m_id = 0);
    ~Motor();
    void updateInfo(MotorInfoData &minfo);
    void updateFixedInfo(FixedMotorInfoData &minfo){limits_motor = minfo;}
    void updateMotorStatus();
    void setModuleController(ModuleController *module){_module=module;}
    MotorInfoData get_motor_info(){return info_motor;}
    FixedMotorInfoData get_motor_limits(){return limits_motor;}

    bool isMoving() const {return moving;}
    void setTorque(bool check);

private slots:

    void on_chk_torque_clicked(bool checked);

private:
    Ui::Motor *ui;
    MotorInfoData info_motor;
    FixedMotorInfoData limits_motor;
    ModuleController *_module;
    int motor_id;
    bool moving = false;
};

#endif // MOTOR_H
