#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QTimer>
#include <QTime>
#include "../../include/RomerinMessage.h"
#include "moduleemulator.h"
#include <QJsonObject>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE




class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


    static void info(const QString &mens);

    static QPalette red_pal,green_pal;

    void sendVirtualMessage(uint8_t vid, const RomerinMsg &m);
    RomerinMsg executeMessage(uint8_t robot_id, const RomerinMsg &m);

private slots:

    void loop();

    void read_ip_port();



    void on_ck_wifi_stateChanged(int arg1);

    void on_B_robotConf_clicked();

private:
    Ui::MainWindow *ui;
    QHostAddress master_address{};
    QUdpSocket * ip_port;

    QTimer timer;
    QTime watchdog{};
    MsgReader udp_reader;
    static QStatusBar *sbar;
    static MainWindow *_this;
    QList<ModuleEmulator> modules;
    void updateModulesTable();



};
#endif // MAINWINDOW_H
