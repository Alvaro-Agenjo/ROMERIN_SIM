#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

//--------------------------------------------//
#include <QUdpSocket>
#include <list>
#include <QTimer>
#include <QFile>

#include "../include/RomerinMessage.h"
#include "modulecontroller.h"
#include "module.h"
#include "trayectorygenerator.h"
#include "components/utils.h"

#include <QtCharts>
#include <QChartView>



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

    void on_btn_thor_test_simple_clicked();
    void on_btn_thor_test_complete_clicked();




    void on_btn_stand_clicked();

    void on_btn_reset_clicked();

    void on_btn_test1_clicked();

    void on_btn_test_2_clicked();


    void on_btn_record_clicked();

    void on_btn_relax_clicked();

    void on_btn_fixRot_clicked();

private:
    Ui::MainWindow *ui;

    //--------------------------------------------//
    QUdpSocket * ip_port;
    QTimer timer;
    MsgReader udp_reader;
    //RomerinModel robot;

    static QStatusBar *sbar;
    static MainWindow *_this;

    trayectoryGenerator commander;
    QFile file;

    void loop_wifi();

    QString test_subject = "FREYJA";
};
#endif // MAINWINDOW_H
