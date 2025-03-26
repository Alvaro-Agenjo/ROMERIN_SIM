#include "modulecontroller.h"
#include "../include/RomerinMessage.h"
#include "mainwindow.h"
#include "romerinmodule.h"
#include "configdlg.h"
QUdpSocket * ModuleController::ip_port=0;
void ModuleController::loop()
{
    if(bt_watch_dog<WATCHDOG)bt_watch_dog++;
    if(wifi_watch_dog<WATCHDOG)wifi_watch_dog++;
    if(port)
    while(port->thereIsMessage()){
        auto msg=port->getMessage();
        executeMessage(msg);
        reset_bt_watchdog();
    }
    tab->setEnabled(isConnected());
    //to detect reconeections  and initial connections
    if((!conected)&&(isConnected())){
        sendMessage(RomerinMsg(ROM_GET_CONFIG));
        for(int i=0;i<6;i++)sendMessage(RomerinMsg(ROM_GET_FIXED_MOTOR_INFO,i));
    }
    conected=isConnected();
}
void ModuleController::sendMessage(const RomerinMsg &m){
    //BT dedicated port has priority over WIFI
    if((port)&&(port->isOpen())){
        port->sendMessage(m);
    }else if((ip_port)&&(isConnected())){
        ip_port->writeDatagram((const char *)(m.data),m.size+3,ip,12000);
    }
}
void ModuleController::sendConfig(ConfigurationInfo &info)
{
    RomerinMsg m(ROM_CONFIG);
    uchar_t *punt=m.info;
    for(int i=0;i<4;i++)punt+=romerin_writeUChar(punt, info.ip[i]);
    for(int i=0;i<4;i++)punt+=romerin_writeUChar(punt, info.gateway[i]);
    for(int i=0;i<4;i++)punt+=romerin_writeUChar(punt, info.mask[i]);
    punt+=romerin_writeString(punt,info.name.toStdString().c_str());
    punt+=romerin_writeString(punt,info.wifi.toStdString().c_str());
    punt+=romerin_writeString(punt,info.key.toStdString().c_str());
    m.size=(punt-m.info)+1;
    sendMessage(m);
}
RomerinMsg ModuleController::executeMessage(const RomerinMsg &m)
{
    MainWindow::info("Message id: "+ QString::number(m.id));
    switch(m.id){
      case ROM_MOTOR_INFO:{
        int m_id=m.info[0];
            MotorInfoData &&minfo=romerin_getMotorInfo(m.info);
            if(tab)tab->getMotorUI(m_id)->updateInfo(minfo);
        }
      break;
      case ROM_ANALOG_INFO:{
            AnalogInfoData &&an=romerin_getAnalogInfo(m.info);
            if(tab)tab->updateInfo(an);
        }
      break;
    case ROM_STATE:
        robot_state=m.info[0];
        robot_cicle_time=m.info[1];
        if(tab)tab->updateRobotState();
    break;
    case ROM_SUCTIONCUP_INFO:{
        auto sc=romerin_getSuctionCupInfo(m.info);
        if(tab)tab->updateInfo(sc);
    }
    break;
    case ROM_TEXT:

            if(tab)tab->setText((char *)(m.info));
    break;

    case ROM_FIXED_MOTOR_INFO:{
            int m_id=m.info[0];
            if((m_id<0)||(m_id>6))break;
            FixedMotorInfoData &&minfo=romerin_getFixedMotorInfo(m.info+1);
            if(tab)tab->getMotorUI(m_id)->updateFixedInfo(minfo);
    }
    break;
    case ROM_CONFIG:{
           auto cad=QByteArray((const char*)(m.info),m.size-1);
           ConfigurationInfo &&conf=ConfigurationInfo::getFromQByte(cad);
           if(tab)tab->setConfigInfo(conf);
    }
    break;

    }
return RomerinMsg::none();
}
