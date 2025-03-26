#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "btserialport.h"
#include <QTimer>
#include "motorwidget.h"
#include "configdlg.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
struct AnalogInfo{
    float vbat;
    float ibat;
    float vbus;
    float ibus;
};
struct SuctionCupInfo{
    float pressure;
    float temperature;
    float force;
    uint8_t distance[3];
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void sendConfig(ConfigurationInfo &info);
public slots:
    void onPortButton();
private slots:
    void loop();



    void on_dial_SuctionCup_valueChanged(int value);

    void on_button_scanWiFi_clicked();

    void on_button_initWiFi_clicked();

    void on_button_configuration_clicked();
    void updateRobotState();

    void on_button_testA_clicked();

    void on_button_testB_clicked();

private:
    Ui::MainWindow *ui;
    MotorWidget *motors[7];
    BTserialPort port;
    QTimer timer;
    AnalogInfo analog;
    SuctionCupInfo suction;
    ConfigDlg config;
    uint8_t robot_state=0;
    uint8_t robot_cicle_time=0;
};
#endif // MAINWINDOW_H
