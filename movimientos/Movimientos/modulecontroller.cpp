#include "modulecontroller.h"
#include "../include/RomerinMessage.h"
#include "mainwindow.h"
#include "module.h"
//#include "romerinmodule.h"
#include "configdlg.h"
//#include <QDir>
#include <QTextStream>

QUdpSocket * ModuleController::ip_port=0;
void ModuleController::loop()
{
    //if(bt_watch_dog<WATCHDOG)bt_watch_dog++;
    if(wifi_watch_dog<WATCHDOG)wifi_watch_dog++;
    /*
    if(port)
    while(port->thereIsMessage()){
        auto msg=port->getMessage();
        executeMessage(msg);
        reset_bt_watchdog();
    }
    */
    mod->setEnabled(isConnected());
    // to detect reconeections  and initial connections
    if((!conected)&&(isConnected()))getConfig();
    conected=isConnected();
    /*
    //log handler
    if(file)save_data();
    */

    save_data();
}
 void ModuleController::getConfig()
{
    sendMessage(RomerinMsg(ROM_GET_CONFIG));
    sendMessage(RomerinMsg(ROM_GET_CONFIG_V2));
    for(int i=0;i<6;i++)sendMessage(RomerinMsg(ROM_GET_FIXED_MOTOR_INFO,i));
}
void ModuleController::sendMessage(const RomerinMsg &m){

static int num{};
    //BT dedicated port has priority over WIFI
    if(!m.size)return;

    /*
    if((port)&&(port->isOpen())){
        port->sendMessage(m);
    }else if((ip_port)&&(isConnected())){
    */
    if((ip_port)&&(isConnected())){
    //Si es un controlador de un modulo simulado, aquí encpasulo el mensaje
        if(simulated){
            auto &&mv=romerinMsg_simulate(virtualid,m);
            ip_port->writeDatagram((const char *)(mv.data),mv.size+3,ip,12000);
        }
        else ip_port->writeDatagram((const char *)(m.data),m.size+3,ip,12000);
        // qDebug()<<"mensaje: "<<++num;
    }
}
bool  ModuleController::isConnected(){
    //return((bt_watch_dog<WATCHDOG)||(wifi_watch_dog<WATCHDOG));
    return(wifi_watch_dog<WATCHDOG);
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
void ModuleController::sendConfigV2(ConfigurationInfoV2 &info)
{
    RomerinMsg m(ROM_CONFIG_V2);
    uchar_t *punt=m.info;
    for(int i=0;i<6;i++){ //6*(2+2+2+1+1+1)
        punt+=romerin_writeUInt16(punt, info.lenghts[i]);
        punt+=romerin_writeUInt16(punt, info.min[i]);
        punt+=romerin_writeUInt16(punt, info.max[i]);
        punt+=romerin_writeUChar(punt,info.offset[i]);
        punt+=romerin_writeUChar(punt,info.vel_profile[i]);
        punt+=romerin_writeUChar(punt,info.acc_profile[i]);
    }
    for(int i=0;i<3;i++)punt+=romerin_writeInt16(punt, info.position[i]);
    for(int i=0;i<3;i++)punt+=romerin_writeInt16(punt, info.orientation[i]);
    uint8_t init_configuration=0;
    // if(info.diRsable_bt)init_configuration|=0x10;
    if(info.compact_mode)init_configuration|=0x01;
    punt+=romerin_writeUChar(punt,init_configuration);
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
            // if(tab)tab->getMotorUI(m_id)->updateInfo(minfo);
            mod->getMotorUI(m_id)->updateInfo(minfo);
        }
        break;
        case ROM_ANALOG_INFO:{
            AnalogInfoData &&an=romerin_getAnalogInfo(m.info);
            //if(tab)tab->updateInfo(an);
        }
        break;
    case ROM_STATE:
        robot_state=m.info[0];
        robot_cicle_time=m.info[1];
        //if(tab)tab->updateRobotState();
        mod->updateRobotState();
    break;
    case ROM_SUCTIONCUP_INFO:{
        auto sc=romerin_getSuctionCupInfo(m.info);
        //if(tab)tab->updateInfo(sc);
        mod->updateInfo(sc);
    }
    break;
    case ROM_TEXT:

            //if(tab)tab->setText((char *)(m.info));
    break;

    case ROM_FIXED_MOTOR_INFO:{
            int m_id=m.info[0];
            if((m_id<0)||(m_id>6))break;
            FixedMotorInfoData &&minfo=romerin_getFixedMotorInfo(m.info+1);
            //if(tab)tab->getMotorUI(m_id)->updateFixedInfo(minfo);
            mod->getMotorUI(m_id)->updateFixedInfo(minfo);
            qDebug()<< name <<": m"<<m_id<<" = ["<<minfo.min_angle<<" "<<minfo.max_angle<<"]";
    }
    break;
    case ROM_CONFIG:{
           auto cad=QByteArray((const char*)(m.info),m.size-1);
           ConfigurationInfo &&conf=ConfigurationInfo::getFromQByte(cad);
           //if(tab)tab->setConfigInfo(conf);
           mod->setConfigInfo(conf);
    }
    break;
    case ROM_CONFIG_V2:{
           ConfigurationInfoV2 &&conf=ConfigurationInfoV2::getFromBuffer(m.info);
           //if(tab)tab->setConfigInfo(conf);
           mod->setConfigInfo(conf);
           mod->setMatrizTransformacion();
    }
    break;
    case ROM_COMPACT_ROBOT_DATA:{
        RobotCompactData data{};
        if(romerin_get_robot_compact_data(m,data)){
            for(uint8_t i=0;i<6;i++){
                MotorInfoData &&minfo=compact_data2motor_info_data(data,i);
                //if(tab)tab->getMotorUI(minfo.id)->updateInfo(minfo);
                if(mod)mod->getMotorUI(minfo.id)->updateInfo(minfo);
            }
            //analogously with suction cup info
            auto &&sc=compact_data2sc_info_data(data);
            //if(tab)tab->updateInfo(sc);
        }
        break;
    }

    }
return RomerinMsg::none();
}

void ModuleController::init_rec(ulong init_t,const QString &directory)
{
/*
    QDir dir(directory);
    QString fullPath = dir.filePath(name+".txt");
    file = std::make_unique<QFile>(fullPath);
    if (!file->open(QIODevice::WriteOnly | QIODevice::Text))
        qDebug()<<"No se pudo abrir el fichero para escritura";
    else{
        QTextStream out(file.get());
        out<<"Q1;Q2;Q3;Q$;Q5;Q6;T1;T2;T3;T4;T5;T6;";
        out<<"M1.P;M1.V;M1.I;M2.P;M2.V;M2.I;";
        out<<"M3.P;M3.V;M3.I;M4.P;M4.V;M4.I;";
        out<<"M5.P;M5.V;M5.I;M6.P;M6.V;M6.I;";
        out<<"T(ms);\n";
    }
    this->init_t=init_t;
*/
}
void ModuleController::stop_rec()
{
    //file = nullptr; //usamos RAII para que lo haga todo
}

void ModuleController::save_data()
{

    double qs[6],mt[6];
    MotorInfoData mi[6];
    if(!_file->isOpen())return;
    mod->get_qs(qs);
    mod->get_torques(mt);
    mod->get_motor_info(mi);
    QTextStream fichero(_file);

    fichero << name << ":\nQ ";
    for(auto q:qs)fichero<<q<<";";
    fichero << "\nTorque ";
    for(auto m:mt)fichero<<m<<";";
    fichero << "\nMotorD ";
    for(auto m:mi)fichero<<m.position<<";"<<m.velocity<<";" << m.intensity << ";\n ";
    fichero<<"\n" << millis()-init_t<<"\n";


}

void ModuleController::activateMotors(bool on)
{
    for(int i = 0; i< 6; i++){
        sendMessage(romerinMsg_Torque(i, on));
        mod->updateTorque(i, on);
    }
}
