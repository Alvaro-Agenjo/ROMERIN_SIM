#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "moduleshandler.h"
#include "romerinmodule.h"
#include "QtSerialPort/QSerialPortInfo"
#include <QNetworkInterface>
#include <QNetworkDatagram>
#include <QFileDialog>
#include "emulationwnd.h"


QStatusBar * MainWindow::sbar;
MainWindow * MainWindow::_this;
ApoloConection *MainWindow::apolo{};
QPalette MainWindow::red_pal;
QPalette MainWindow::green_pal;
QGamepad *  MainWindow::gamepad;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),tmp_port(0),ip_port(0)
{
    ui->setupUi(this);
    sbar=ui->statusbar;
    _this=this;
    gamepad=nullptr;

    apolo = new ApoloConection(12000);
    on_toolButton_pressed();//updates ports detected on the system

    QTableWidget *tb=ui->table_modules;
    QStringList m_TableHeader;
    tb->setColumnCount(4);
    tb->setColumnWidth(0, 50);
    tb->setColumnWidth(1, 110);
    tb->setColumnWidth(2, 50);
    tb->setColumnWidth(3, 40);
    tb->setStyleSheet("QHeaderView::section { background-color:lightGray }");
    m_TableHeader<<"Module"<<"IP"<<"BT COM"<<"Status";
    tb->setHorizontalHeaderLabels(m_TableHeader);

    red_pal.setColor(QPalette::Window, Qt::white);
    red_pal.setColor(QPalette::WindowText, Qt::red);
    green_pal.setColor(QPalette::Window, Qt::black);
    green_pal.setColor(QPalette::WindowText, Qt::green);

    info("Initialized");
    //m_pTableWidget->setItem(0, 1, new QTableWidgetItem("Hello"))

    connect(&timer, &QTimer::timeout, this, &MainWindow::loop);

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
    ui->l_log_dir->setText(QCoreApplication::applicationDirPath());

    ////////////////////////// Comunicacion mediante fichero ////////////////////////////////
    fd = creat("../../../../movimientos/build/com.txt", 0666);
    ////////////////////////////////////////////////////////////////////////////////////////
    timer.start(MAIN_TIMER_MS);//antes 50ms
}
void MainWindow::info(const QString &mens)
{
    sbar->showMessage(mens,5000);
    _this->update();
}
void MainWindow::updateTable()
{
    QTableWidget *tb=ui->table_modules;
    tb->clearContents();
    tb->setRowCount(ModulesHandler::module_list.size());
    int i=0;
    for(auto mod : ModulesHandler::module_list){
        tb->setItem(i,0,new QTableWidgetItem(mod->name));
        tb->setItem(i,1,new QTableWidgetItem(mod->ip.toString()));
        tb->setItem(i,2,new QTableWidgetItem(mod->port_name));
        tb->setItem(i,3,new QTableWidgetItem(mod->simulated ? QString("SIM[%1]").arg( QString::number(mod->virtualid) ) : QString("REAL")));
        i++;
    }

}
MainWindow::~MainWindow()
{
    ::close(fd);
    delete ui;
    ModulesHandler::clear();
}

void MainWindow::loop_wifi()
{
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
        
        ////////////////////////// Comunicacion mediante fichero ////////////////////////////////
        if(::write(fd, &local, sizeof(QHostAddress))<0){
            perror("Error al escribir en el fichero");
            ::close(fd);
        }
        ////////////////////////////////////////////////////////////////////////////////////////
        
        connect(ip_port, &QUdpSocket::readyRead,
                    this, &MainWindow::read_ip_port);
    }
    //regular BROADCAST message
    if(count==0){
        RomerinMsg ping(ROM_SET_MASTER_IP);
        ip_port->writeDatagram((const char *)(ping.data),ping.size+3,QHostAddress::Broadcast,12000);
    }
    if(ip_port) read_ip_port();

}
void MainWindow::read_ip_port()
{

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
                //Si encapsula un sistema simulado debe:
                // 1.- desencapsular para convertirlo en un mensaje normal
                // 2.- si no existe el robot simulado, generar el tab
                // con el nombre del robot y un [ID] al final
                // 3.- ejecutar el mensaje

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
                   if(!module->tab){
                      module->tab=new RomerinModule;
                      ui->tabWidget->addTab((QWidget *)(module->tab),qname);
                      module->tab->setModule(module);
                      module->reset_wifi_watchdog();
                   }
                   updateTable();

                }
            }
      }
  }
}

//loop exclusivo para el control y coordinación de módulos
//sigo este esquema para desacoplar al maximo el control del organismo del resto
Matrix4d MatrixPRPY(const Vector3d& pos, const Vector3d& ori){
    Matrix3d rotation;
    rotation = Eigen::AngleAxisd(ori(0), Eigen::Vector3d::UnitX())
             * Eigen::AngleAxisd(ori(1), Eigen::Vector3d::UnitY())
             * Eigen::AngleAxisd(ori(2), Eigen::Vector3d::UnitZ());

    // Matriz homogénea
    Matrix4d ret = Matrix4d::Identity();
    ret.block<3,3>(0,0) = rotation;
    ret.block<3,1>(0,3) = pos;
    return ret;
}
void MainWindow::loop_robot_controller()
{
    //1-Actualización del modelo: actualización de entradas
    for(auto &mod_controller:ModulesHandler::module_list){
        auto module=mod_controller->tab;
        auto name = mod_controller->name.toStdString();
        if(robot[name]==RomerinModel::none) {
            ModuleModel aux{name};
            auto conf = module->getConfigInfoV2();
            Vector3d pos, ori;
            for(int i=0;i<3;i++){
                pos(i)=conf.position[i]*0.001; //meters
                ori(i)=conf.orientation[i]*M_PI/180.0;//rads
            }
            aux.base_pose=MatrixPRPY(pos,ori);
            robot.modules.push_back(aux);

        }
        auto & mod_model=robot[name];

        //TODO: actualizar la info del modelo con la info de los robots
        module->get_qs(mod_model.q_deg);
        for(int i=0;i<6;i++)mod_model.q_deg[i]*=M_PI/180.0;


    }
    //2-Control: ejecución del controlador que corresponda en base a la info del modelo

    //3.- Actualziación de salidas: envio de comandos a cada modulo

}

void MainWindow::loop()
{
    static long counter=0;
    counter++;
    ui->timerInfo->setText(QString::number(counter));
//gestion del tmp_port
  loop_tmp_port();
  loop_wifi();
  loop_game_pad();
  ModulesHandler::loop();
  loop_apolo_update();

  //update the tab view:
  for(auto m:ModulesHandler::module_list){
      int ind=ui->tabWidget->indexOf(m->tab);
      if(m->isConnected())ui->tabWidget->setTabText(ind, m->name);
      else ui->tabWidget->setTabText(ind, m->name+"[OFF]");

  }
  //update logging buttons
  if(recording){
      static bool b_start_color=false;
      bool secs=((millis()-init_t)/700)%2;
      if(secs!=b_start_color){
          if(!secs){
              ui->b_start->setStyleSheet("QPushButton { background-color: red; }");
               ui->tabWidget->setStyleSheet("QTabBar::tab { color: red; }");
          }
          else {
              ui->b_start->setStyleSheet("QPushButton { background-color: yellow; }");
              ui->tabWidget->setStyleSheet("QTabBar::tab { color: black; }");
          }
          b_start_color=secs;
      }

  }


//gamepad connect
#if QT_VERSION < 0x060000
    if(!gamepad){

        auto gamepads = QGamepadManager::instance()->connectedGamepads();
        if (!gamepads.isEmpty()){
            qDebug() << "gamepad present";
            gamepad=new QGamepad(gamepads[0], this);
        }
    }

#endif
}
void MainWindow::loop_tmp_port()
{
    static int port_time_out=0;
    if(!tmp_port){
        port_time_out=0;
        return;
    }
    if(!port_time_out)port_time_out=50;
    port_time_out--;
    if(!port_time_out){
       tmp_port->close();
       delete tmp_port;
       tmp_port=nullptr;
       port_time_out=0;
       ui->portButton->setDisabled(false);
       ui->portButton->setText("OPEN");
       info("port closed. It is not a robot module, or the robot is not working properly");
       return;
    }

    while(tmp_port->thereIsMessage()){
        auto msg=tmp_port->getMessage();
        if((msg.size)&&(msg.id==ROM_NAME)){

           char name[100]="";
           romerin_getString(msg.info,name,99);
           QString qname(name);
           info(QString("Robot ")+qname+QString(" detected at port ")+tmp_port->getPortName());
           auto module=ModulesHandler::getWithName(qname);
           if(!module->tab){
               module->tab=new RomerinModule;
               ui->tabWidget->addTab((QWidget *)(module->tab),qname);
               module->tab->setModule(module);
           }
           if(module->port!=tmp_port){
             delete module->port;
             module->port=tmp_port;
             module->port_name=tmp_port->getPortName();
             tmp_port=0;
             ui->portButton->setDisabled(false);
             ui->portButton->setText("CLOSE");
             updateTable();
             if(module->tab)module->tab->updateModule();
             return;
           }
        }
    }
}
void MainWindow::on_toolButton_pressed()
{
    //solo es para actualizar la lista de puertos.
    ui->comboBoxPorts->clear();
    foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()){
        ui->comboBoxPorts->addItem(serialPortInfo.portName());
        }
}

void MainWindow::on_portButton_clicked()
{
    //Si esta en proceso de gestión de otro puerto no hace caso:
    //compruebo si el puerto seleccionado está abierto ya en la lista de puertos
    QString port_name=ui->comboBoxPorts->currentText();
    auto module=ModulesHandler::getWithPort(port_name);
    if((module)&&(module->port)){
        info(QString("The port: ")+port_name+" is asigned to the module "+module->name);
        if(module->port->isOpen())module->port->close();
            else module->port->open(port_name);
        if(module->port->isOpen())ui->portButton->setText("CLOSE");
            else ui->portButton->setText("OPEN");
        return;
    }
    tmp_port=new BTport(this);

    ui->portButton->setDisabled(true);
    info(QString("Trying to open the port: ")+port_name);

    tmp_port->open(port_name);
    if(tmp_port->isOpen())info("port opened. Checking if it is a robot module");
    RomerinMsg getname(ROM_GET_NAME);
    tmp_port->sendMessage(getname);






}

void  MainWindow::loop_game_pad()
{
    if((MainWindow::gamepad)&&(MainWindow::gamepad->isConnected())){
        ui->virtualJoyPadL->setX(MainWindow::gamepad->axisLeftX());
        ui->virtualJoyPadL->setY(-MainWindow::gamepad->axisLeftY());
        ui->virtualJoyPadR->setX(MainWindow::gamepad->axisRightX());
        ui->virtualJoyPadR->setY(-MainWindow::gamepad->axisRightY());
    }
    //AQUI SE PONEN LAS ACCIONES O LLAMADAS A lo que se desee hacer con el joy pad.

}
void MainWindow::on_comboBoxPorts_currentTextChanged(const QString &arg1)
{
    info("Selection Changed to "+arg1);
    auto mod=ModulesHandler::getWithPort(arg1);
    if((mod)&&(mod->port)&&(mod->port->isOpen())){
        ui->portButton->setText("CLOSE");
    }
    else ui->portButton->setText("OPEN");
}


void MainWindow::on_B_emulateRobots_clicked()
{
    if(!emulation_wnd){
        qDebug() << "CREANDO";
        emulation_wnd = new EmulationWnd(this);
        emulation_wnd->setWindowModality(Qt::NonModal);
        emulation_wnd->setAttribute(Qt::WA_DeleteOnClose);
        connect(emulation_wnd, &QObject::destroyed,[this](){emulation_wnd=nullptr;qDebug() << "EmulationWND DEstroyed";}
        );
    }
    emulation_wnd->show();
}

void MainWindow::loop_apolo_update()
{
    if(!apolo)return;
    //apolo button
    ui->b_apolo->setChecked(apolo->is_connected());
    //if there is no conection do nothing
    if(!apolo->is_connected())return;
    char message[400],world[1]{}, thor[]{"THOR"};
    int size;

   /* auto module=ModulesHandler::getWithName(thor);
    if(module){
        double qs[6]{};
        module->tab->get_qs(qs);
        size=ApoloMessage::writeSetRobotJoints(message,world,thor,6,qs);
        apolo->send_message(message,size);

    }*/
    for(auto m:ModulesHandler::module_list){
        double qs[6]{};
        m->tab->get_qs(qs);
        size=ApoloMessage::writeSetRobotJoints(message,world,m->name.toStdString().c_str(),6,qs);
        apolo->send_message(message,size);
    }
    size=ApoloMessage::writeUpdateWorld(message,world);
    apolo->send_message(message,size);

}
void MainWindow::on_b_apolo_clicked()
{
    if(!apolo->is_connected())apolo->connect_to_apolo();
}

void MainWindow::on_b_log_dir_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Seleccionar Directorio", ui->l_log_dir->text(),QFileDialog::DontUseNativeDialog | QFileDialog::ShowDirsOnly);
    if (!dir.isEmpty())ui->l_log_dir->setText(dir);

}



void MainWindow::on_b_start_clicked()
{
    if(recording){
        //stop recording
        for(auto m:ModulesHandler::module_list)m->stop_rec();
        recording=false;
        ui->b_start->setText("START");
        ui->b_start->setStyleSheet("");
        ui->tabWidget->setStyleSheet("");

    }else{
        init_t=millis();
        for(auto m:ModulesHandler::module_list)m->init_rec(init_t,ui->l_log_dir->text());
        ui->b_start->setText("STOP");
        recording = true;
    }
}
