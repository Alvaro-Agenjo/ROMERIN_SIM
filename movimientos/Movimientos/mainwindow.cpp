
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "moduleshandler.h"
#include <QDebug>
#include <QNetworkInterface>
#include <QNetworkDatagram>
#include <QDateTime>
#include <QTextStream>


#include "module.h"



QStatusBar * MainWindow::sbar;
MainWindow * MainWindow::_this;
static bool activo = false;

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent),    ui(new Ui::MainWindow), ip_port(0)
{
    ui->setupUi(this);
    sbar=ui->statusbar;
    _this=this;

    //Actualizacion de la tabla donde se informa de los módulos conectados
    QTableWidget *tb=ui->table_modules;
    QStringList m_TableHeader;
    tb->setColumnCount(4);
    tb->setColumnWidth(0, 110);
    tb->setColumnWidth(1, 110);
    tb->setColumnWidth(2, 80);
    tb->setColumnWidth(3, 110);
    tb->setStyleSheet("QHeaderView::section { background-color:lightGray }");
    m_TableHeader<<"Module"<<"IP"<<"BT COM"<<"Status";
    tb->setHorizontalHeaderLabels(m_TableHeader);

    //Temporizador de la funcion loop
    info("Initialized");
    connect(&timer, &QTimer::timeout, this, &MainWindow::loop);
    connect(&test_timer, &QTimer::timeout, this, &MainWindow::on_btn_fixRot_clicked);
    //Obtencion de las IPs disponibles para la conexion
    foreach (const QNetworkInterface &netInterface, QNetworkInterface::allInterfaces()) {
        QNetworkInterface::InterfaceFlags flags = netInterface.flags();
        if( (bool)(flags & QNetworkInterface::IsRunning) && !(bool)(flags & QNetworkInterface::IsLoopBack)){
            foreach (const QNetworkAddressEntry &address, netInterface.addressEntries()) {
                if(address.ip().protocol() == QAbstractSocket::IPv4Protocol)
                    ui->comboBoxIPs->addItem(address.ip().toString());
            }
        }
    }
    ui->comboBoxIPs->addItem("127.0.0.1");

    timer.start(MAIN_TIMER_MS);//antes 50ms
}

MainWindow::~MainWindow()
{
    delete ui;
    ModulesHandler::clear();
    if(activo) file.close();
}



/* Funcion para actualizar la tabla de los modulos conectados*/
void MainWindow::updateTable(){
    QTableWidget *tb=ui->table_modules;
    tb->clearContents();
    tb->setRowCount(ModulesHandler::module_list.size());
    int i=0;
    for(auto mod : ModulesHandler::module_list){
        tb->setItem(i,0,new QTableWidgetItem(mod->name));
        tb->setItem(i,1,new QTableWidgetItem(mod->ip.toString()));
        // tb->setItem(i,2,new QTableWidgetItem(mod->port_name));
        tb->setItem(i,3,new QTableWidgetItem(mod->simulated ? QString("SIM[%1]").arg( QString::number(mod->virtualid) ) : QString("REAL")));
        i++;
    }

}

/* Funcion para mostrar mensajes en la esquina inferior */
void MainWindow::info(const QString &mens){
    sbar->showMessage(mens,5000);
    _this->update();
}


void MainWindow::loop(){
    static unsigned long counter = 0;
    counter ++;
    ui->timerInfo->setText(QString::number(counter));

    loop_wifi();
    ModulesHandler::loop();

    //update the tab view:
    for(auto m:ModulesHandler::module_list){
        int ind=ui->tabWidget->indexOf(m->mod);
        if(m->isConnected())ui->tabWidget->setTabText(ind, m->name);
        else ui->tabWidget->setTabText(ind, m->name+"[OFF]");

    }
    commander.setTime(counter);
    commander.nextOrder();
}
void MainWindow::loop_wifi(){
    //si está activa, entonces manda el ping broadcast y gestiona los mensajes pasándoselos a cada modulo
    static int count=0;
    if(count++>20)count=0;
    if(!ui->ck_wifi->isChecked())return;
    if(!ip_port){
        ip_port=new QUdpSocket(this);
        ip_port->setSocketOption(QAbstractSocket::LowDelayOption, 1);
        ModuleController::ip_port=ip_port;
        QHostAddress local(ui->comboBoxIPs->currentText());
        ip_port->bind(local, 12001);
        connect(ip_port, &QUdpSocket::readyRead, this, &MainWindow::read_ip_port);
    }
    //regular BROADCAST message
    if(count==0){
        RomerinMsg ping(ROM_SET_MASTER_IP);
        ip_port->writeDatagram((const char *)(ping.data),ping.size+3,QHostAddress::Broadcast,12000);
    }
    if(ip_port) read_ip_port();
}

void MainWindow::read_ip_port(){
    while (ip_port->hasPendingDatagrams()) {
        QNetworkDatagram datagram = ip_port->receiveDatagram();
        QHostAddress sender=datagram.senderAddress();
        QByteArray data=datagram.data();
        ModuleController *module=nullptr;
        for (int i = 0; i < data.size(); ++i) {
            if(udp_reader.add_uchar(data[i])){
                auto msg=udp_reader.getMessage();
                bool simulated = (msg.id==ROM_SIMULATION);
                uint8_t virtualid{};
                if(simulated){
                    virtualid = msg.info[0];
                    msg=get_simulatedMsg(msg);
                    module=ModulesHandler::getWithSimulatedId(virtualid);
                }
                else module=ModulesHandler::getWithAddress(sender);
                //             //Si encapsula un sistema simulado debe:
                //             // 1.- desencapsular para convertirlo en un mensaje normal
                //             // 2.- si no existe el robot simulado, generar el tab
                //             // con el nombre del robot y un [ID] al final
                //             // 3.- ejecutar el mensaje

                if(module){
                    module->executeMessage(msg);
                    module->reset_wifi_watchdog();
                }
                else if((msg.size)&&(msg.id==ROM_NAME)){
                    char name[100]="";
                    romerin_getString(msg.info,name,99);
                    QString qname(name);
                    //QString qname = !simulated?QString(name):QString("%1[%2]").arg(name,QString::number(virtualid));
                    info(QString("Robot ")+qname+QString(" detected at IP ")+sender.toString());
                    module=ModulesHandler::getWithName(qname);
                    if(simulated){
                        module->simulated=true;
                        module->virtualid=virtualid;
                    }
                    module->ip=sender;
                    if(!module->mod){
                        module->mod=new Module;
                        ui->tabWidget->addTab((QWidget *)(module->mod),qname);
                        module->mod->setModule(module);
                        module->reset_wifi_watchdog();
                    }
                    module->setFile(&file);
                    updateTable();
                    commander.setMatrizTransformacion(module);
                }
            }
        }
    }
}


void MainWindow::on_txt_motor1_maxvel_editingFinished()
{
    float velMax = ui->txt_motor1_maxvel->text().toFloat();
    for(auto modulo : ModulesHandler::module_list){
        commander.setMotorVel(modulo, velMax, 1);
    }
}

void MainWindow::on_txt_motor2_maxvel_editingFinished()
{
    float velMax = ui->txt_motor2_maxvel->text().toFloat();
    for(auto modulo : ModulesHandler::module_list){
        commander.setMotorVel(modulo, velMax, 1);
    }
}

void MainWindow::on_txt_motor3_maxvel_editingFinished()
{
    float velMax = ui->txt_motor3_maxvel->text().toFloat();
    for(auto modulo : ModulesHandler::module_list){
        commander.setMotorVel(modulo, velMax, 1);
    }
}

void MainWindow::on_txt_motor4_maxvel_editingFinished()
{
    float velMax = ui->txt_motor4_maxvel->text().toFloat();
    for(auto modulo : ModulesHandler::module_list){
        commander.setMotorVel(modulo, velMax, 1);
    }
}

void MainWindow::on_txt_motor5_maxvel_editingFinished()
{
    float velMax = ui->txt_motor5_maxvel->text().toFloat();
    for(auto modulo : ModulesHandler::module_list){
        commander.setMotorVel(modulo, velMax, 1);
    }
}

void MainWindow::on_txt_motor6_maxvel_editingFinished()
{
    float velMax = ui->txt_motor6_maxvel->text().toFloat();
    for(auto modulo : ModulesHandler::module_list){
        commander.setMotorVel(modulo, velMax, 1);
    }
}

void MainWindow::on_txt_masterVel_editingFinished()
{
    float velMax = ui->txt_masterVel->text().toFloat();
    float master[6];
    for(int i= 0; i< 6; i++) master[i] = velMax;

    ui->txt_motor1_maxvel->setText(QString::number(velMax));
    ui->txt_motor2_maxvel->setText(QString::number(velMax));
    ui->txt_motor3_maxvel->setText(QString::number(velMax));
    ui->txt_motor4_maxvel->setText(QString::number(velMax));
    ui->txt_motor5_maxvel->setText(QString::number(velMax));
    ui->txt_motor6_maxvel->setText(QString::number(velMax));

    for(auto modulo : ModulesHandler::module_list){
        commander.setMotorVel(modulo, master);
    }
}

void MainWindow::on_btn_enableMotors_clicked(bool checked)
{
    for(auto mod : ModulesHandler::module_list){
        mod->activateMotors(checked);
    }
}

void MainWindow::on_btn_reset_clicked()
{
    commander.reset();
}
void MainWindow::on_btn_stand_clicked()
{
    commander.stand();
}
void MainWindow::on_btn_relax_clicked()
{
    commander.relax();
}
void MainWindow::on_btn_fixRot_clicked()
{
    commander.fixed_rotation(2);
    test_timer.stop();
}



void MainWindow::on_btn_thor_test_simple_clicked()
{
    bool elbow = ui->chk_elbow->isChecked();
    bool fix = ui->chk_fixed->isChecked();

    double x = ui->txt_THOR_X->text().toDouble();   x/=1000.0;
    double y = ui->txt_THOR_Y->text().toDouble();   y/=1000.0;
    double z = ui->txt_THOR_Z->text().toDouble();   z/=1000.0;

    commander.moveLeg(ModulesHandler::module_list.front()->name, x, y, z, elbow, fix);
}
void MainWindow::on_btn_thor_test_complete_clicked()
{
    bool elbow = ui->chk_elbow->isChecked();
    bool fix = ui->chk_fixed->isChecked();

    double x = ui->txt_THOR_X->text().toDouble();   x/=1000.0;
    double y = ui->txt_THOR_Y->text().toDouble();   y/=1000.0;
    double z = ui->txt_THOR_Z->text().toDouble();   z/=1000.0;

    float giro[3] = {ui->txt_THOR_alfa->text().toFloat(),
                     ui->txt_THOR_beta->text().toFloat(),
                     ui->txt_THOR_gamma->text().toFloat()};

    commander.moveLeg(ModulesHandler::module_list.front(), x, y, z, giro, elbow, fix);
}
void MainWindow::on_btn_test1_clicked()
{
//    float RPY[] = {0,180,0};
//    commander.moveLeg(ModulesHandler::getWithName("THOR"),0.332385, 0.10679,0.037,RPY, true, false);
//    commander.moveLeg(ModulesHandler::getWithName("FRIGG"),0.471615, 0.10679,0.037,RPY, true, false);
//    commander.moveLeg(ModulesHandler::getWithName("ODIN"),0.471615, -0.03679,0.037,RPY, true, false);
//    commander.moveLeg(ModulesHandler::getWithName("LOKI"),0.332385, -0.03679,0.037,RPY, true, false);

//    double m[6] = {210,255,195,180,180,180};
//    for(auto modulo : ModulesHandler::module_list){
//    commander.setMotorAngles(modulo, m);
    commander.setMotorAngles(ModulesHandler::module_list.front() , 180, 3);
    commander.setMotorAngles(ModulesHandler::module_list.front() , ui->txt_THOR_alfa->text().toFloat(), 4);
    commander.setMotorAngles(ModulesHandler::module_list.front() , ui->txt_THOR_beta->text().toFloat() , 5);
}
void MainWindow::on_btn_test_2_clicked()
{
   float RPY[] = {0,180,0};
   commander.moveLeg(ModulesHandler::getWithName("THOR"), 0.402, 0.035, 0.037 ,RPY, true, false);
   commander.moveLeg(ModulesHandler::getWithName("FRIGG"), 0.402, 0.035, 0.037 ,RPY, true, false);
   commander.moveLeg(ModulesHandler::getWithName("ODIN"), 0.402, 0.035, 0.037 ,RPY, true, false);
   commander.moveLeg(ModulesHandler::getWithName("LOKI"), 0.402, 0.035, 0.037 ,RPY, true, false);
}

void MainWindow::on_btn_test_3_clicked()
{
        double m[6] = {210,255,195,180,180,180};
        for(auto modulo : ModulesHandler::module_list){
            commander.setMotorAngles(modulo, m);
        }
//    commander.stand();
//    test_timer.start(2800);
}

void MainWindow::on_btn_record_clicked()
{
    if(activo){
        activo = false;
        file.close();
        ui->btn_record->setText("Record");
        qDebug()<<"Fichero cerrado";
    }
    else{
        activo = true;
        QDateTime fecha = QDateTime::currentDateTime();
        QString str_fecha = fecha.toString("log_yyyyMMdd_hhmmss");
        str_fecha = "../../../../Datalogs/" +str_fecha + ".txt";
        qDebug() << str_fecha;

        file.setFileName(str_fecha);        
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qDebug() << "Error al abrir el archivo: " << file.errorString();
            return;
        }

        ulong tiempo = millis();
        for(auto module : ModulesHandler::module_list){
            module->init_t = tiempo;
            //module->setFile(&file);
        }
        ui->btn_record->setText("Stop Recording");

    }
}

float RPY[] = {0,0,0};
int counter = 0;



