#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "RomerinMsg.h"
#include "QtSerialPort/QSerialPortInfo"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
      port(this),config(this)
{

    ui->setupUi(this);
    connect(ui->portButton, SIGNAL(clicked(bool)), this, SLOT(onPortButton()));
    connect(&timer, &QTimer::timeout, this, &MainWindow::loop);
    connect(ui->slider_suctioncup, SIGNAL(valueChanged(int)),SLOT(on_dial_SuctionCup_valueChanged(int )) );
    connect(ui->button_scanWiFi, SIGNAL(clicked(bool)), this, SLOT(on_button_scanWiFi_clicked()));
    connect(ui->button_initWiFi, SIGNAL(clicked(bool)), this, SLOT(on_button_initWiFi_clicked()));
    connect(ui->button_testA, SIGNAL(clicked(bool)), this, SLOT(on_button_testA_clicked()));
    connect(ui->button_testB, SIGNAL(clicked(bool)), this, SLOT(on_button_testB_clicked()));
    for(int i = 0; i<7; i++)
    {
        motors[i]=new MotorWidget(this, i);
        motors[i]->setPort(&port);
        ui->motorsLayOut->addWidget(motors[i]);


    }
    foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts())
        {
        ui->comboBoxPorts->addItem(serialPortInfo.portName());
        }
    timer.start(50);


     //ui->angle->setStyleSheet("QLCDNumber { background-color: black; color: green; }");

}

MainWindow::~MainWindow()
{
    port.close();
    delete ui;
}

void MainWindow::onPortButton()
{
    if(!port.isOpen()){
        if(port.open(ui->comboBoxPorts->currentText())){
                for(int i=0;i<7;i++)port.sendGetFixedMotorInfo(i);
                port.sendGetName();
                port.sendGetConfiguration();
        }
    } else port.close();


    if(port.isOpen()){
        ui->label->setText("is open");
        ui->portButton->setText("CLOSE");
    }
    else {
        ui->portButton->setText("OPEN");
        ui->label->setText("");
    }
}

MotorInfo getMotorInfoFromQByte(QByteArray &m, int pos)
{
    MotorInfo mot;
    mot.status=m[pos++];
    mot.position=getFloatFromQByte(m,pos);pos+=4;
    mot.velocity=getFloatFromQByte(m,pos);pos+=4;
    mot.intensity=getFloatFromQByte(m,pos);pos+=4;
    mot.temperature=m[pos++];
    mot.voltage=getFloatFromQByte(m,pos);
    return mot;
}
FixedMotorInfo getFixedMotorInfoFromQByte(QByteArray &m, int pos)
{
   FixedMotorInfo mot;
   mot.max_angle=getFloatFromQByte(m,pos);pos+=4;
   mot.min_angle=getFloatFromQByte(m,pos);pos+=4;
   return mot;
}
AnalogInfo getAnalogInfoFromQByte(QByteArray &m, int pos)
{
    AnalogInfo aux;
    aux.vbat=getFloatFromQByte(m,pos);pos+=4;
    aux.ibat=getFloatFromQByte(m,pos);pos+=4;
    aux.vbus=getFloatFromQByte(m,pos);pos+=4;
    aux.ibus=getFloatFromQByte(m,pos);pos+=4;
    return aux;
}
SuctionCupInfo getSuctioncupInfoFromQByte(QByteArray &m, int pos)
{
    SuctionCupInfo aux;
    aux.pressure=getFloatFromQByte(m,pos);pos+=4;
    aux.force=getFloatFromQByte(m,pos);pos+=4;
    aux.temperature=getFloatFromQByte(m,pos);pos+=4;
    aux.distance[0]=m[pos++];
    aux.distance[1]=m[pos++];
    aux.distance[2]=m[pos++];
    return aux;
}
ConfigurationInfo getConfigurationInfoFromQByte(QByteArray &m, int pos)
{
    ConfigurationInfo aux;
    for(int i=0;i<4;i++)aux.ip[i]=m[pos++];
    for(int i=0;i<4;i++)aux.gateway[i]=m[pos++];
    for(int i=0;i<4;i++)aux.mask[i]=m[pos++];
    m.remove(0,pos);
    QList<QByteArray> texts = m.split(0);
    aux.name=texts[0];
    aux.wifi=texts[1];
    aux.key=texts[2];
    return aux;
}
void MainWindow::sendConfig(ConfigurationInfo &info)
{
    port.sendConfiguration(info);
}
void MainWindow::loop()
{
    static int cont=0;

    while(port.isOpen()){
        QByteArray && mens=port.getMessage();
        if(mens.isEmpty())return;
        else{
            //interpreta mensaje... en este caso es prueba sin mas
            switch(mens[0]){
                case ROM_MOTOR_INFO:
                    {int m_id=mens[1];
                    MotorInfo &&minfo=getMotorInfoFromQByte(mens, 2);

                    QString tStr = QString("motor: %1 pos: %2 temp:%3").arg(m_id).arg(minfo.position).arg(minfo.temperature);
                    if(m_id<7)motors[m_id]->updateInfo(minfo);
                    }
                break;
                case ROM_ANALOG_INFO:

                    analog=getAnalogInfoFromQByte(mens, 1);
                     ui->lcd_vbat->display(QString::number(analog.vbat, 'f', 1));
                     ui->lcd_ibat->display(QString::number(analog.ibat, 'f', 1));
                     ui->lcd_vbus->display(QString::number(analog.vbus, 'f', 1));
                     ui->lcd_ibus->display(QString::number(analog.ibus, 'f', 1));

                break;
                case ROM_TEXT:
                    mens.remove(0,1);
                    ui->serial_data->setText(QString(mens));
                    ui->info_area->append(QString(mens));
                break;
                case ROM_FIXED_MOTOR_INFO:{
                    int m_id=mens[1];
                    FixedMotorInfo &&minfo=getFixedMotorInfoFromQByte(mens, 2);
                    if(m_id<7)motors[m_id]->updateFixedInfo(minfo);
                    }
                case ROM_NAME:
                    mens.remove(0,1);
                    ui->label_name->setText(QString(mens));
                break;
                case ROM_CONFIG:{
                   ConfigurationInfo conf=getConfigurationInfoFromQByte(mens,1);
                   config.setConfigInfo(conf);
                }
                break;
                case ROM_STATE:
                    robot_state=mens[1];
                    robot_cicle_time=mens[2];
                    updateRobotState();
                break;
                case ROM_SUCTIONCUP_INFO:
                    suction=getSuctioncupInfoFromQByte(mens,1);
                    ui->lcd_pressure->display(QString::number(suction.pressure, 'f', 0));
                    ui->lcd_force->display(QString::number(suction.force, 'f', 1));
                    ui->lcd_temperature->display(QString::number(suction.temperature, 'f', 0));
                    ui->lcd_distance1->display(QString::number(suction.distance[0], 'f', 0));
                    ui->lcd_distance2->display(QString::number(suction.distance[1], 'f', 0));
                    ui->lcd_distance3->display(QString::number(suction.distance[2], 'f', 0));
                break;
            }


        }

    }

}



void MainWindow::updateRobotState()
{
    const char S_TRUE[]="QLabel { background-color: green; color: white; }";
    const char S_FALSE[]="QLabel { background-color: red; color: white; }";

    ui->label_bt->setStyleSheet((robot_state&0x01?S_TRUE:S_FALSE));
    ui->label_wifi->setStyleSheet((robot_state&0x02?S_TRUE:S_FALSE));
    ui->label_can->setStyleSheet((robot_state&0x04?S_TRUE:S_FALSE));
    ui->label_power->setStyleSheet((robot_state&0x08?S_TRUE:S_FALSE));
    ui->label_cicle->setText(QString("cycle time: %1").arg(robot_cicle_time));

}



void MainWindow::on_dial_SuctionCup_valueChanged(int value)
{
    ui->lcd_suctionCup->display(value);
    port.sendSuctionCupMessage(value);
}

void MainWindow::on_button_scanWiFi_clicked()
{
    port.sendScanWiFi();
}

void MainWindow::on_button_initWiFi_clicked()
{
    port.sendInitWiFi();
}

void MainWindow::on_button_configuration_clicked()
{
    //ConfigDlg *config = new ConfigDlg(this);
    config.show();


}

void MainWindow::on_button_testA_clicked()
{
    port.send1byteCommand(ROM_TEST_A_MESSAGE);

}

void MainWindow::on_button_testB_clicked()
{
     port.send1byteCommand(ROM_TEST_B_MESSAGE);
}
