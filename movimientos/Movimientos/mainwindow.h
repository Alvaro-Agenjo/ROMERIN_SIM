#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

//--------------------------------------------//
#include <QUdpSocket>
#include <list>
#include <QTimer>

#include "../include/RomerinMessage.h"
#include "modulecontroller.h"




constexpr int  MAIN_TIMER_MS=40; //ms before 50
constexpr int WATCHDOG=4000/MAIN_TIMER_MS;  // 4 secs


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    //--------------------------------------------//
    void updateTable();
    static void info(const QString &mens);

private slots:

    //--------------------------------------------//
    void loop();
    void read_ip_port();

    void on_txt_motor1_maxvel_editingFinished();
    void on_txt_motor2_maxvel_editingFinished();
    void on_txt_motor3_maxvel_editingFinished();
    void on_txt_motor4_maxvel_editingFinished();
    void on_txt_motor5_maxvel_editingFinished();
    void on_txt_motor6_maxvel_editingFinished();
    void on_txt_masterVel_editingFinished();

    void on_btn_enableMotors_clicked(bool checked);





private:
    Ui::MainWindow *ui;

    //--------------------------------------------//
    QUdpSocket * ip_port;
    QTimer timer;
    MsgReader udp_reader;
    //RomerinModel robot;
    static QStatusBar *sbar;
    static MainWindow *_this;

    void loop_wifi();
    void sendVel(float max_vel, int motor_id);
};
#endif // MAINWINDOW_H
