#include "apoloconection.h"
#include <QDebug>
ApoloConection::ApoloConection(quint16 port, QObject *parent) :  QObject(parent),port(port)
{
    connect(&socket, &QTcpSocket::connected, this, &ApoloConection::onConnected);
    connect(&socket, &QTcpSocket::disconnected, this, &ApoloConection::onDisconnected);
    //connect(&socket, &QTcpSocket::errorerrorOccurred, this, &ApoloConection::onError);
}
void ApoloConection::onConnected(){
    qDebug() << "Connected to server";
    // Aquí puedes enviar mensajes o realizar otras operaciones
}

void ApoloConection::onDisconnected(){
    qDebug() << "Disconnected from server";
    // Aquí puedes intentar reconectar o manejar la desconexión
}

void ApoloConection::onError(QTcpSocket::SocketError socketError){
    qDebug() << "Socket error:" << socket.errorString();
    // Aquí puedes manejar el error y posiblemente intentar reconectar
}
void ApoloConection::send_message(char *buf, int size)
{
    if(socket.state() != QAbstractSocket::ConnectedState) return;
    socket.write(buf, size);
    socket.flush();
}
