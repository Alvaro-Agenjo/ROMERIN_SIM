
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "moduleshandler.h"
#include <QDebug>
#include <QNetworkInterface>
#include <QNetworkDatagram>


#include "module.h"



QStatusBar * MainWindow::sbar;
MainWindow * MainWindow::_this;

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent),    ui(new Ui::MainWindow), ip_port(0)
{
    ui->setupUi(this);
    sbar=ui->statusbar;
    _this=this;




    auto *series = new QLineSeries();
    series->append(0,3);
    series->append(3,6);
    series->append(4,9);
    series->append(6,5);
    series->append(6,3);
    series->append(43,1);
    series->append(1,7);

    auto *chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(series);
    chart->createDefaultAxes();
    //chart->setVisible(true);

    //ui->graphicsView = new QChartView(chart, ui->graphicsView);
    // ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    //ui->graphicsView->setVisible(true);

     QChartView *chartview = new QChartView(chart);
     chartview->setRenderHint(QPainter::Antialiasing);
     chartview->setVisible(true);



    //Actualizacion de la tabla donde se informa de los módulos conectados
    QTableWidget *tb=ui->table_modules;
    QStringList m_TableHeader;
    tb->setColumnCount(4);
    tb->setColumnWidth(0, 100);
    tb->setColumnWidth(1, 110);
    tb->setColumnWidth(2, 80);
    tb->setColumnWidth(3, 100);
    tb->setStyleSheet("QHeaderView::section { background-color:lightGray }");
    m_TableHeader<<"Module"<<"IP"<<"BT COM"<<"Status";
    tb->setHorizontalHeaderLabels(m_TableHeader);

    //Temporizador de la funcion loop
    info("Initialized");
    connect(&timer, &QTimer::timeout, this, &MainWindow::loop);

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
    static long counter = 0;
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
                    updateTable();

                }
            }
        }
    }
}


void MainWindow::on_txt_motor1_maxvel_editingFinished()
{
    float velMax = ui->txt_motor1_maxvel->text().toFloat();
    commander.setVel(velMax, 1);
}

void MainWindow::on_txt_motor2_maxvel_editingFinished()
{
    float velMax = ui->txt_motor2_maxvel->text().toFloat();
    commander.setVel(velMax, 2);
}

void MainWindow::on_txt_motor3_maxvel_editingFinished()
{
    float velMax = ui->txt_motor3_maxvel->text().toFloat();
    commander.setVel(velMax, 3);
}

void MainWindow::on_txt_motor4_maxvel_editingFinished()
{
    float velMax = ui->txt_motor4_maxvel->text().toFloat();
    commander.setVel(velMax, 4);
}

void MainWindow::on_txt_motor5_maxvel_editingFinished()
{
    float velMax = ui->txt_motor5_maxvel->text().toFloat();
    commander.setVel(velMax, 5);
}

void MainWindow::on_txt_motor6_maxvel_editingFinished()
{
    float velMax = ui->txt_motor6_maxvel->text().toFloat();
    commander.setVel(velMax, 6);
}

void MainWindow::on_txt_masterVel_editingFinished()
{
    float velMax = ui->txt_masterVel->text().toFloat();

    ui->txt_motor1_maxvel->setText(QString::number(velMax));
    ui->txt_motor2_maxvel->setText(QString::number(velMax));
    ui->txt_motor3_maxvel->setText(QString::number(velMax));
    ui->txt_motor4_maxvel->setText(QString::number(velMax));
    ui->txt_motor5_maxvel->setText(QString::number(velMax));
    ui->txt_motor6_maxvel->setText(QString::number(velMax));

    for(int i = 1; i<= 6; i++){
        commander.setVel(velMax, i);
    }

}

void MainWindow::on_btn_enableMotors_clicked(bool checked)
{
    for(auto mod : ModulesHandler::module_list){
        mod->activateMotors(checked);
    }
}

void MainWindow::on_btn_stand_clicked()
{
    commander.stand();
}


void MainWindow::on_btn_reset_clicked()
{
    commander.reset();
}

void MainWindow::on_btn_test1_clicked()
{
    commander.test(true);
}

void MainWindow::on_btn_test_2_clicked()
{
    commander.a= commander.a +5;
    //commander.loopTest(false);
}

void MainWindow::on_btn_thor_test_clicked()
{
    bool elbow = ui->chk_elbow->isChecked();
    bool fix = ui->chk_fixed->isChecked();

    double x = ui->txt_THOR_X->text().toDouble();   x/=1000.0;
    double y = ui->txt_THOR_Y->text().toDouble();   y/=1000.0;
    double z = ui->txt_THOR_Z->text().toDouble();   z/=1000.0;

    commander.moveLeg("THOR", x, y, z, elbow, fix);
}

