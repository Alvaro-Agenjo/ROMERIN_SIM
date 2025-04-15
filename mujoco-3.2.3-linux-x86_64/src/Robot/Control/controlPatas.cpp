#include "Control/ControlPatas.h"

Control::Control(){

    param = new ParametrosControl;
    primerMovimientoAcabado = true;
    segundoMovimientoAcabado = true;
    tercerMovimientoAcabado = true;
    cuartoMovimientoAcabado = true;
    ventanaAbierta = false;
    hiloControlIniciado = false;

    //Estado de reposo
    reposo.Q1 = 0;
    reposo.Q2 = 2;
    reposo.Q3 = -4.01;
    reposo.Q4 = 2;
    reposo.Q5 = 0; 
    reposo.Q6 = 0; 
    reposo.suct = 0;

    //Estado de reposo
    // reposo.Q1 = 0;
    // reposo.Q2 = 2;
    // reposo.Q3 = -4.01;
    // reposo.Q4 = 0;
    // reposo.Q5 = -1.45; 
    // reposo.Q6 = 0; 
    // reposo.suct = 0;

    // Inicialización de atributos del hilo
    pthread_attr_init(&hilo);
    pthread_attr_setdetachstate(&hilo, PTHREAD_CREATE_JOINABLE);

    // Inicialización de semáforos
    sem_init(&primeraPata, 0, 0);
    sem_init(&segundaPata, 0, 0);
    sem_init(&terceraPata, 0, 0);
    sem_init(&cuartaPata, 0, 0);

    numPata = 0;


    // modules.append(ModuleSimulator{0,"THOR",1});
    // modules.append(ModuleSimulator{6, "ODIN",2});
    // modules.append(ModuleSimulator{12, "LOKI",3});
    // modules.append(ModuleSimulator{18, "FRIGG",4});


    modules.append(ModuleSimulator{0,"THOR",1});
    modules.append(ModuleSimulator{6, "LOKI",2});
    modules.append(ModuleSimulator{12, "ODIN",3});
    modules.append(ModuleSimulator{18, "FRIGG",4});

    openJson();
    
}

void Control::openJson(){
    QString fileName = "../include/RomJoints/ROMERIN_config.json";
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("No se pudo abrir el fichero.");
        return;
    }
    QByteArray data = file.readAll();
    QJsonDocument doc(QJsonDocument::fromJson(data));
    QJsonArray jsonArray = doc.array();

    for(auto &mod:modules){
        for (const QJsonValue value : jsonArray) {
            QJsonObject jsonObject = value.toObject();
            mod.fromJson(jsonObject);
        }
    }
}

void Control::setParametros(mjModel* M, mjData* D) {
    param->m = M;
    param->d = D;

    for(auto &m:modules)m.setup(M, D);
}

void Control::setVentanaAbierta(bool apertura){
    this->ventanaAbierta = apertura;
}

void* Control::loop(void* args){
    Control* control = static_cast<Control*>(args);
    while(control->getVentanaAbierta()){
        for(auto &mod:control->modules)mod.loop();
    }
    pthread_exit(NULL);
}

void Control::iniciarHilosMovimiento(void* args){
    if(!hiloControlIniciado){
        std::cout << "hilo creado" << std::endl;
        pthread_create(&hiloControlInfo, NULL, getInfoPatas, args);
        pthread_create(&HiloLoop, NULL, loop, args);
    }
    hiloControlIniciado = true;
    hilosCreados = true;
}

//Esta función da la posición de las ventosas
void Control::getPosicion(const char* site){
    int idVentosa = mj_name2id(param->m, mjOBJ_SITE, site);
    if(idVentosa == -1){
        std::cout << "Site de ventosa no encontrado" << std::endl;
    }
    else{
        const double* posicion = param->d->site_xpos + 3 * idVentosa;
        posActual.setPosicion(posicion[0], posicion[1], posicion[2]);
    }
}

bool Control::getVentanaAbierta(){
    
    if(!ventanaAbierta){
        std::cout << "ventana cerrada" << std::endl;
    }
    return ventanaAbierta;
}

void* Control::getInfoPatas(void* args){
    Control* control = static_cast<Control*>(args);
    while(control->getVentanaAbierta()){
        //Posicion de succionadores
        // control->getPosicion("baseSuccionador_THOR");
        // control->getPosicion("baseSuccionador_ODIN");
        // control->getPosicion("baseSuccionador_LOKI");
        // control->getPosicion("baseSuccionador_FRIGG");

        control->getPosicion("THOR_prss");
        control->getPosicion("LOKI_prss");
        control->getPosicion("ODIN_prss");
        control->getPosicion("FRIGG_prss");

        //Succionadores
        mjtNum suction_force1 = control->param->d->actuator_force[24];
        mjtNum suction_force2 = control->param->d->actuator_force[25];
        mjtNum suction_force3 = control->param->d->actuator_force[26];
        mjtNum suction_force4 = control->param->d->actuator_force[27];
        control->iniciarSocket(control);
    }
    std::cout << "Saliendo de thread de control" << std::endl;
    pthread_exit(NULL);
}

bool Control::enPosicion(void* args){
    Control* control = static_cast<Control*>(args);
    return control->posActual.comprobarPosicion(posObjetivo);
}

void Control::pararHilosMovimiento(){
    /*for (int i=0; i<4; i++) {
        pthread_cancel(hiloMovimientoPatas[i]);
    }*/
    hilosCreados = false;
}

void Control::enMovimiento(bool cond){
    this->moviendo = cond;
}

bool Control::getMoviendo(){
    return moviendo;
}

void Control::iniciarSocket(void* args){
    Control* control = static_cast<Control*>(args);

    control->ip_port=new QUdpSocket();
    
    for(auto &mod:control->modules)mod.setIpPort(control->ip_port);
    
    if (!control->ip_port->bind(PUERTO_ENVIO, QUdpSocket::ShareAddress)) {
        qDebug() << "Error al enlazar el socket:" << control->ip_port->errorString();
    }

    while (true) {
        if (control->ip_port->hasPendingDatagrams()) {
            control->recibirInfoSocket(control);
        }
        if(!control->getVentanaAbierta())break;
    }

    control->ip_port->close();
    control->ip_port->disconnect();
    delete control->ip_port;
    control->ip_port=nullptr;
}

void Control::recibirInfoSocket(void* args){
    Control* control = static_cast<Control*>(args);
    char buffer[1024];

    QNetworkDatagram datagram = ip_port->receiveDatagram();
    QHostAddress sender=datagram.senderAddress();
    for(auto &mod:control->modules) mod.setSender(sender);
    QByteArray data=datagram.data();
    for (int i = 0; i < data.size(); ++i) {
        if(control->UDP_msg.add_uchar(data[i])){
            auto msg=control->UDP_msg.getMessage();

            if(msg.id==ROM_SET_MASTER_IP){

                if(master_address.isNull()){
                    master_address=sender;
                }
                if(master_address==sender){
                    for(auto &mod:control->modules){
                        sendVirtualMessage(mod.id, name_message(mod.MODULE_NAME), control);
                    }
                }

            }
            if(msg.id==ROM_SIMULATION){ //los unicos admitidos

                //extraer y en base al ID ejecutar el mensaje,
                uint8_t virtualid = msg.info[0];
                msg=get_simulatedMsg(msg);
                auto &&ret_msg=control->executeMessage(virtualid,msg, control);
                sendVirtualMessage(virtualid,ret_msg, control);
            }      
        }
    }
        
}

RomerinMsg Control::executeMessage(uint8_t robot_id, const RomerinMsg &m, void* args)
{
    Control* control = static_cast<Control*>(args);

    for(auto &module:control->modules){
        if(module.id == robot_id) return module.executeMessage(m);
    }
  return RomerinMsg::none();
}

void Control::sendVirtualMessage(uint8_t vid, const RomerinMsg &m, void* args){

    Control* control = static_cast<Control*>(args);

    static int num{};
    if(!m.size)return;
    if(ip_port && !master_address.isNull()){
        auto &&mv=romerinMsg_simulate(vid,m);
        control->ip_port->writeDatagram((const char *)(mv.data),mv.size+3,master_address,PUERTO_ESCUCHA);
    }
}
