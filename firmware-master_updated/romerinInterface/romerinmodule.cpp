#include "romerinmodule.h"
#include "ui_romerinmodule.h"
#include "QtSerialPort/QSerialPortInfo"
RomerinModule::RomerinModule(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RomerinModule),
    config(this)
{
    ui->setupUi((QWidget *)this);
    for(int i = 0; i<6; i++)
    {
        motors[i]=new MotorWidget((QWidget *)this, i);
        ui->motorsLayOut->addWidget(motors[i]);
    }


  connect(&timer, &QTimer::timeout, this, &RomerinModule::loop);
  connect(ui->slider_suctioncup, SIGNAL(valueChanged(int)),SLOT(on_dial_SuctionCup_valueChanged(int )) );
  connect(ui->button_scanWiFi, SIGNAL(clicked(bool)), this, SLOT(on_button_scanWiFi_clicked()));
  connect(ui->button_initWiFi, SIGNAL(clicked(bool)), this, SLOT(on_button_initWiFi_clicked()));
  connect(ui->button_testA, SIGNAL(clicked(bool)), this, SLOT(on_button_testA_clicked()));
  connect(ui->button_testB, SIGNAL(clicked(bool)), this, SLOT(on_button_testB_clicked()));
  timer.start(50);
}
void RomerinModule::loop()
{


}
RomerinModule::~RomerinModule()
{
    delete ui;
}
void RomerinModule::setModule(ModuleController * mod)
{
    module=mod;
    for(int i = 0; i<6; i++)motors[i]->setModuleController(module);

}
void RomerinModule::updateInfo(AnalogInfoData &data){
    ui->lcd_vbat->display(QString::number(data.vbat, 'f', 1));
    ui->lcd_ibat->display(QString::number(data.ibat, 'f', 1));
    ui->lcd_vbus->display(QString::number(data.vbus, 'f', 1));
    ui->lcd_ibus->display(QString::number(data.ibus, 'f', 1));
}
void RomerinModule::updateInfo(SuctionCupInfoData & data)
{
    ui->lcd_pressure->display(QString::number(data.pressure, 'f', 0));
    ui->lcd_force->display(QString::number(data.force, 'f', 1));
    ui->lcd_temperature->display(QString::number(data.temperature, 'f', 0));
    ui->lcd_distance1->display(QString::number(data.distance[0], 'f', 0));
    ui->lcd_distance2->display(QString::number(data.distance[1], 'f', 0));
    ui->lcd_distance3->display(QString::number(data.distance[2], 'f', 0));
}
void RomerinModule::updateModule()
{
    ui->l_portname->setText(module->port_name);
    if(module->port)ui->b_port->setText(module->port->isOpen()?"CLOSE":"OPEN");
}

#include "mainwindow.h"
void RomerinModule::updateRobotState()
{
    auto &red=MainWindow::red_pal;
    auto &green=MainWindow::green_pal;

    ui->label_bt->setAutoFillBackground(true);
    ui->label_wifi->setAutoFillBackground(true);
    ui->label_can->setAutoFillBackground(true);
    ui->label_power->setAutoFillBackground(true);
    ui->label_cicle->setAutoFillBackground(true);
    const char S_TRUE[]="QLabel { background-color: green; color: white; }";
    const char S_FALSE[]="QLabel { background-color: red; color: white; }";
    if(!module)return;
    uchar_t robot_state=module->robot_state;
    uchar_t robot_cicle_time=module->robot_cicle_time;

    ui->label_bt->setPalette(robot_state&0x01?green:red);
    ui->label_wifi->setPalette((robot_state&0x02?green:red));
    ui->label_can->setPalette((robot_state&0x04?green:red));
    ui->label_power->setPalette((robot_state&0x08?green:red));
    ui->label_cicle->setText(QString("cycle time: %1").arg(robot_cicle_time));
}
void RomerinModule::setText(char *text){
    //ui->serial_data->setText(QString(mens));
    ui->info_area->append(QString((char *)text));
}

void RomerinModule::on_dial_SuctionCup_valueChanged(int value)
{
    ui->lcd_suctionCup->display(value);
    module->sendMessage(romerinMsg_SuctionCupPWM(value));
}

void RomerinModule::on_button_scanWiFi_clicked()
{
    module->sendMessage(romerinMsg_ScanWiFi());
}

void RomerinModule::on_button_initWiFi_clicked()
{
    module->sendMessage(romerinMsg_InitWiFi());
}

void RomerinModule::on_button_configuration_clicked()
{
    config.show();
}

void RomerinModule::on_button_testA_clicked()
{
    module->sendMessage(RomerinMsg(ROM_TEST_A_MESSAGE));

}

void RomerinModule::on_button_testB_clicked()
{
      module->sendMessage(RomerinMsg(ROM_TEST_B_MESSAGE));
}
