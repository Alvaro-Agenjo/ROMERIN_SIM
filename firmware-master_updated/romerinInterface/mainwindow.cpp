#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "moduleshandler.h"
#include "romerinmodule.h"
#include "QtSerialPort/QSerialPortInfo"
#include <QNetworkInterface>
#include <QNetworkDatagram>

QStatusBar * MainWindow::sbar;
MainWindow * MainWindow::_this;
QPalette MainWindow::red_pal;
QPalette MainWindow::green_pal;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),tmp_port(0),ip_port(0)
{
    ui->setupUi(this);
    sbar=ui->statusbar;
    _this=this;

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

    timer.start(50);
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
        tb->setItem(i,3,new QTableWidgetItem("NDEF"));
        i++;
    }

}
MainWindow::~MainWindow()
{

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
        ModuleController::ip_port=ip_port;
        QHostAddress local(ui->comboBoxIPs->currentText());
        ip_port->bind(local, 12001);
        connect(ip_port, &QUdpSocket::readyRead,
                    this, &MainWindow::read_ip_port);
    }
    //regular BROADCAST message
    if(count==0){
        RomerinMsg ping(ROM_SET_MASTER_IP);
        ip_port->writeDatagram((const char *)(ping.data),ping.size+3,QHostAddress::Broadcast,12000);
    }

}
void MainWindow::read_ip_port()
{

    while (ip_port->hasPendingDatagrams()) {
        QNetworkDatagram datagram = ip_port->receiveDatagram();
        QHostAddress sender=datagram.senderAddress();
        QByteArray data=datagram.data();
        auto module=ModulesHandler::getWithAddress(sender);
        for (int i = 0; i < data.size(); ++i) {
            if(udp_reader.add_uchar(data[i])){
                auto &&msg=udp_reader.getMessage();
                if(module){
                    module->executeMessage(msg);
                    module->reset_wifi_watchdog();
                }
                else if((msg.size)&&(msg.id==ROM_NAME)){
                   char name[100]="";
                   romerin_getString(msg.info,name,99);
                   QString qname(name);
                   info(QString("Robot ")+qname+QString(" detected at IP ")+sender.toString());
                   auto module=ModulesHandler::getWithName(qname);
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
void MainWindow::loop()
{
//gestion del tmp_port
  loop_tmp_port();
  loop_wifi();
  ModulesHandler::loop();

  //update the tab view:
  for(auto m:ModulesHandler::module_list){
      int ind=ui->tabWidget->indexOf(m->tab);
      if(m->isConnected())ui->tabWidget->setTabText(ind, m->name);
      else ui->tabWidget->setTabText(ind, m->name+"[OFF]");

  }
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

}
