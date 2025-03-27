#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QNetworkInterface>
#include <QNetworkDatagram>
#include <QFileDialog>
#include <QJsonDocument>

#include <QJsonArray>
#include "RomerinMsgA.h"

QStatusBar * MainWindow::sbar;
MainWindow * MainWindow::_this;
QPalette MainWindow::red_pal;
QPalette MainWindow::green_pal;


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),ip_port(0)
{
    ui->setupUi(this);
    sbar=ui->statusbar;
    _this=this;



    QTableWidget *tb=ui->table_modules;
    QStringList m_TableHeader;
    tb->setColumnCount(10);
    tb->setColumnWidth(0, 50);
    for(int i = 1; i<8; i++)tb->setColumnWidth(i, 40);
    tb->setColumnWidth(8, 90);
     tb->setColumnWidth(9, 90);
    tb->setStyleSheet("QHeaderView::section { background-color:lightGray }");
    m_TableHeader<<"Module"<<"ID"<<"X"<<"Y"<<"Z"<<"RX"<<"RY"<<"RZ"<<"Attach"<<"Lay on";
    tb->setHorizontalHeaderLabels(m_TableHeader);

    red_pal.setColor(QPalette::Window, Qt::white);
    red_pal.setColor(QPalette::WindowText, Qt::red);
    green_pal.setColor(QPalette::Window, Qt::black);
    green_pal.setColor(QPalette::WindowText, Qt::green);

    info("Initialized");
    //m_pTableWidget->setItem(0, 1, new QTableWidgetItem("Hello"))

    connect(&timer, &QTimer::timeout, this, &MainWindow::loop);


    //minitest para ver si esta bien el encapsulado // BORRAR
    auto m1 = romerinMsg_ServoGoalAngle(1,12.2F);
    auto m2 = romerinMsg_simulate(2,m1);
    auto m3 = get_simulatedMsg(m2);
   /* qDebug() << "size m1 = " << m1.size  << "  id = " << m1.id;;
    qDebug() << "size m2 = " << m2.size << "  id = " << m2.id;
    qDebug() << "size m3 = " << m3.size << " id = " << m3.id;
    for(int i=0;i+1<m1.size;i++)qDebug()<< m1.info[i] << " -- "<<m3.info[i];*/
    //fin del minitest

    //default initial values
    modules.append(ModuleEmulator{this,"Thor",1});
    modules.append(ModuleEmulator{this,"ODIN",2});
    modules.append(ModuleEmulator{this,"FRIGG",3});
    modules.append(ModuleEmulator{this,"FREYA",4});
    //initialize the default modules
    for(auto &m:modules)m.setup();

    updateModulesTable();
    timer.start(20); //50 antes
    watchdog.start();
}
void MainWindow::info(const QString &mens)
{
    sbar->showMessage(mens,5000);
    _this->update();
}

MainWindow::~MainWindow()
{

    delete ui;

}
void MainWindow::sendVirtualMessage(uint8_t vid, const RomerinMsg &m){
 static int num{};
    if(!m.size)return;
    if(ip_port && !master_address.isNull()){
        auto &&mv=romerinMsg_simulate(vid,m);
        ip_port->writeDatagram((const char *)(mv.data),mv.size+3,master_address,12001);
        //qDebug()<<"mensaje: "<<++num;
    }
}
void MainWindow::read_ip_port()
{

    while (ip_port->hasPendingDatagrams()) {
        QNetworkDatagram datagram = ip_port->receiveDatagram();
        QHostAddress sender=datagram.senderAddress();
        QByteArray data=datagram.data();
        for (int i = 0; i < data.size(); ++i) {
            if(udp_reader.add_uchar(data[i])){
                auto msg=udp_reader.getMessage();
                //qDebug()<<"Recibidos: "<<msg.size<<" data bytes";


                if(msg.id==ROM_SET_MASTER_IP){
                    //TODO:
                    //comprueba si ya hay master y si es el mismo master
                    if(master_address.isNull()){
                        master_address=sender;

                    }
                    if(master_address==sender){
                        for(auto &mod:modules)
                            sendVirtualMessage(mod.id, name_message(mod.MODULE_NAME));
                    }

                }
                if(master_address==sender)watchdog.restart();
                //TODO:si la direccion de llegada NO coincide con la del master almacenado se descarta
                if(msg.id==ROM_SIMULATION){ //los unicos admitidos
                    //extraer y en base al ID ejecutar el mensaje,
                    uint8_t virtualid = msg.info[0];
                    msg=get_simulatedMsg(msg);

                    auto &&ret_msg=executeMessage(virtualid,msg);
                    sendVirtualMessage(virtualid,ret_msg);
                }


            }
      }
  }
}
void MainWindow::loop()
{
//comprueba el watchdog.. si ha pasado mas tiempo del admisible
//considera que no hay master y quita la IP para indicarlo

    if(!(master_address.isNull()) && (watchdog.elapsed()>3000)){
       master_address.clear();
        qDebug()<<"WATCHDOG ...CLEARING MASTER ADDRESS";
    }
    for(auto &m:modules)m.loop();
    if(ip_port)read_ip_port();
    QTableWidget *tb=ui->table_modules;
    for(int i=0;i<modules.size();i++){
        bool aux=modules[i].attached;
        QPushButton *button = qobject_cast<QPushButton*>(tb->cellWidget(i, 8));
        button->setChecked(aux);
        button->setText(aux?"Detach":"Attach");
        aux=modules[i].suction_cup.is_close_and_aligned();
        button=qobject_cast<QPushButton*>(tb->cellWidget(i, 9));
        button->setChecked(aux);
        button->setText(aux?"Move Away":"Approach");
    }
}


void MainWindow::on_ck_wifi_stateChanged(int arg1)
{
    qDebug()<<arg1;
    if((!arg1)&&ip_port){
        ip_port->close();
        ip_port->disconnect();
        delete ip_port;
        ip_port=nullptr;
        qDebug()<<"CLOSING the socket...";
    }
    else if ((arg1)&&(!ip_port))        //elimino el socket de escucha
    {
        qDebug()<<"OPENING the socket...";
        ip_port=new QUdpSocket(this);
        ip_port->bind(12000,QUdpSocket::ShareAddress);
        connect(ip_port, &QUdpSocket::readyRead,
                    this, &MainWindow::read_ip_port);
    }
}

void MainWindow::on_B_robotConf_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Abrir fichero JSON", "", "JSON Files (*.json)");
    if (!fileName.isEmpty()) {
        QFile file(fileName);
            if (!file.open(QIODevice::ReadOnly)) {
                qWarning("No se pudo abrir el fichero.");
                return;
            }

            QByteArray data = file.readAll();
            QJsonDocument doc(QJsonDocument::fromJson(data));
            QJsonArray jsonArray = doc.array();

            modules.clear();


            for (const QJsonValue value : jsonArray) {
                QJsonObject jsonObject = value.toObject();
                ModuleEmulator module(this);
                module.fromJson(jsonObject);
                modules.append(module);
            }

            //emulate module initialization
            for(auto &module:modules)module.setup();

            updateModulesTable();
    }
}
void MainWindow::updateModulesTable()
{
    QTableWidget *tb=ui->table_modules;
    tb->clearContents();
    tb->setRowCount(modules.size());
    for(int i=0;i<modules.size();i++){
        tb->setItem(i,0,new QTableWidgetItem(modules[i].MODULE_NAME));
        tb->setItem(i,1,new QTableWidgetItem(QString::number(modules[i].id)));
        for(int j=0;j<3;j++){
            tb->setItem(i,j+2,new QTableWidgetItem(QString::number(0.001*modules[i].position[j])));
            tb->setItem(i,j+5,new QTableWidgetItem(QString::number(modules[i].orientation[j])));

        }
        QPushButton *button = new QPushButton("Attach");
        button->setCheckable(true);
        button->setChecked(false);

        tb->setCellWidget(i, 8, button);

         // Conectar el botón a una función lambda que cambia el texto de la fila
         connect(button, &QPushButton::clicked, [i, this, button]() {
             if(modules[i].attached) modules[i].suction_cup.dettach();
             else modules[i].suction_cup.attach();

         });
         QPushButton *button2 = new QPushButton("Approach");
         button2->setCheckable(true);
         button2->setChecked(false);

         tb->setCellWidget(i, 9, button2);

          // Conectar el botón a una función lambda que cambia el texto de la fila
          connect(button2, &QPushButton::clicked, [i, this, button2]() {
              modules[i].suction_cup.emulate_close(!modules[i].suction_cup.is_close_and_aligned());

          });
    }
}

/*
#define BT_DEBUG(M) qDebug()<<robot_id<<"-->"<<M
#define BT_DEBUG_PRINT(...) {QString _s_;_s_.sprintf(__VA_ARGS__);qDebug()<<robot_id<<"-->"<<_s_;}
*/

RomerinMsg MainWindow::executeMessage(uint8_t robot_id, const RomerinMsg &m)
{
  for(auto &module:modules)
      if(module.id==robot_id)return module.executeMessage(m);
return RomerinMsg::none();
}
