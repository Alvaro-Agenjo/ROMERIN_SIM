#ifndef MODULE_H
#define MODULE_H

#include <QWidget>
#include <QTimer>

#include "MTHomogenea.h"
#include "modulecontroller.h"
#include "motor.h"
#include "configdlg.h"
#include "../shared/romkin.h"
#include "../include/RomerinMessage.h"



class ModuleController;

QT_BEGIN_NAMESPACE
namespace Ui {
class Module;
}
QT_END_NAMESPACE

class Module : public QWidget
{
    Q_OBJECT
public:
    explicit Module(QWidget *parent = nullptr);
    ~Module();


    void setModule(ModuleController * mod);
    void updateModule();
    void loop();
    Motor *getMotorUI(int id){return motors[id];}
    void updateInfo(SuctionCupInfoData &data);

    void updateRobotState();
    void setConfigInfo(ConfigurationInfo &info){config.setConfigInfo(info);
    //--------------------------Temporal-------------------------//
        // ConfigurationInfoV2 tmp;
        // config.setConfigInfo(info);
        // romkin.set_lenghts(tmp.lenghts);
    }
    void setConfigInfo(ConfigurationInfoV2 &info){
        config.setConfigInfo(info);
        romkin.set_lenghts(info.lenghts);
    }
    void sendConfig(ConfigurationInfo &info){if(module)module->sendConfig(info);}
    void sendConfigV2(ConfigurationInfoV2 &info){if(module)module->sendConfigV2(info);}
    void get_qs(double *q);
    void get_torques(double *t);
    void get_pos(double pos[3]);
    void get_pos_TCP(double pos[3]);
    void get_motor_info(MotorInfoData *m);
    ConfigurationInfoV2 getConfigInfoV2(){return config.getConfigInvoV2();}

    void setMatrizTransformacion();
    bool checkJointsLimits(double m[], bool simple = false);
    bool objetiveReached();
    bool newTCP_mov(Vector3D actualTCP, Vector3D * futureTCP, Matriz_Transformacion movimiento);
    void updateTorque(int motor_id, bool torque);
    bool isAttached();

private:
    Ui::Module *ui;
    ModuleController *module;
    Motor *motors[6];
    SuctionCupInfoData suction_cup;

    QTimer timer;
    ConfigDlg config;
    uchar_t robot_state{};

    Matriz_Transformacion T;

public:
    RomKin romkin{};
private slots:
    void on_btn_refresh_clicked();
};

#endif // MODULE_H
