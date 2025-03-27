#ifndef ROMERINMODULE_H
#define ROMERINMODULE_H

#include <QWidget>
#include <QTimer>
#include "motorwidget.h"
#include "configdlg.h"
#include "btport.h"
#include "modulecontroller.h"
#include "../include/RomerinMessage.h"
#include "../shared/romkin.h"
#include "configdlg.h"
namespace Ui {
class RomerinModule;
}

class RomerinModule : public QWidget
{
    Q_OBJECT

public:
    explicit RomerinModule(QWidget *parent = nullptr);
    ~RomerinModule();
    void setModule(ModuleController * mod);
    void updateModule();
    void loop();
    MotorWidget *getMotorUI(int id){return motors[id];}
    void updateInfo(AnalogInfoData & data);
    void updateInfo(SuctionCupInfoData & data);
    void setText(char *text);
    void updateRobotState();
    void setConfigInfo(ConfigurationInfo &info){config.setConfigInfo(info);}
    void setConfigInfo(ConfigurationInfoV2 &info){
        config.setConfigInfo(info);
        romkin.set_lenghts(info.lenghts);
    }
    void sendConfig(ConfigurationInfo &info){if(module)module->sendConfig(info);}
    void sendConfigV2(ConfigurationInfoV2 &info){if(module)module->sendConfigV2(info);}
    void get_qs(double *q);
    void get_torques(double *t);
    void get_motor_info(MotorInfoData *m);
    ConfigurationInfoV2 getConfigInfoV2(){return config.getConfigInvoV2();}
private slots:
    void on_dial_SuctionCup_valueChanged(int value);
    void button_scanWiFi_clicked();
    void button_initWiFi_clicked();
    void on_button_configuration_clicked();
    void button_testA_clicked();
    void button_testB_clicked();

     void on_button_compact_mode_clicked();


private:
    Ui::RomerinModule *ui;
    MotorWidget *motors[7];
    BTport *port;
    ModuleController *module;
    QTimer timer;
    ConfigDlg config;
    uchar_t robot_state{};
public:
    RomKin romkin{};
};

#endif // ROMERINMODULE_H
