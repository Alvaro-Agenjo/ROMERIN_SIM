#ifndef ROMERINMODULE_H
#define ROMERINMODULE_H

#include <QWidget>
#include <QTimer>
#include "motorwidget.h"
#include "configdlg.h"
#include "btport.h"
#include "modulecontroller.h"
#include "../include/RomerinMessage.h"
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
    void sendConfig(ConfigurationInfo &info){if(module)module->sendConfig(info);}
private slots:
    void on_dial_SuctionCup_valueChanged(int value);
    void on_button_scanWiFi_clicked();
    void on_button_initWiFi_clicked();
    void on_button_configuration_clicked();
    void on_button_testA_clicked();
    void on_button_testB_clicked();

private:
    Ui::RomerinModule *ui;
    MotorWidget *motors[7];
    BTport *port;
    ModuleController *module;
    QTimer timer;
    ConfigDlg config;

};

#endif // ROMERINMODULE_H
