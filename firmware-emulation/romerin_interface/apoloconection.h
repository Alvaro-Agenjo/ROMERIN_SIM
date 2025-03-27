#ifndef APOLOCONECTION_H
#define APOLOCONECTION_H

#include <QObject>
#include <QTcpSocket>
#include <QDebug>
#include "apoloMessage.h"
class ApoloConection : public QObject
{
    Q_OBJECT
public:
    explicit ApoloConection(quint16 port=12000, QObject *parent = nullptr);
    void connect_to_apolo(){
        if(is_connected())return;
        socket.connectToHost("127.0.0.1", port);
    }
    bool is_connected() const {
        return socket.state() == QAbstractSocket::ConnectedState;
    }
    void send_message(char *buf, int size);
private slots:
    void onConnected();
    void onDisconnected();
    void onError(QTcpSocket::SocketError socketError);
private:
    QTcpSocket socket;
    quint16 port;
};

#endif // APOLOCONECTION_H
