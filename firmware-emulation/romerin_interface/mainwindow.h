#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include "modulecontroller.h"
#include "apoloconection.h"
#include "romerinmodel.h"
#include "../include/RomerinMessage.h"
#include <list>
#include "components/utils.h"

constexpr int  MAIN_TIMER_MS=40; //ms before 50
constexpr int WATCHDOG=4000/MAIN_TIMER_MS;  // 4 secs

#if QT_VERSION < 0x060000
#include <QtGamepad/QGamepad>
#else
#include "components/QtGamepadFake.h"
#endif

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
class EmulationWnd;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void updateTable();
    static void info(const QString &mens);
    static QPalette red_pal,green_pal;
    static QGamepad *gamepad;

private slots:

    void loop();

    void on_toolButton_pressed();
    void on_portButton_clicked();

    void on_comboBoxPorts_currentTextChanged(const QString &arg1);
    void read_ip_port();

    void on_B_emulateRobots_clicked();



    void on_b_apolo_clicked();

    void on_b_log_dir_clicked();



    void on_b_start_clicked();

private:
    Ui::MainWindow *ui;
    BTport * tmp_port;
    QUdpSocket * ip_port;
    EmulationWnd *emulation_wnd=nullptr;
    QTimer timer;
    MsgReader udp_reader;
    RomerinModel robot;
    static QStatusBar *sbar;
    static MainWindow *_this;
    static ApoloConection *apolo;


    void loop_tmp_port();
    void loop_wifi();
    void loop_game_pad();
    void loop_apolo_update();
    void loop_robot_controller();
    ulong init_t{};
    bool recording{};
};
#endif // MAINWINDOW_H
