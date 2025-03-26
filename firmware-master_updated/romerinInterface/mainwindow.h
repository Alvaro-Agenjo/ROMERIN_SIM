#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include "modulecontroller.h"

#include "../include/RomerinMessage.h"
#include <list>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void updateTable();
    static void info(const QString &mens);
    static QPalette red_pal,green_pal;
private slots:

    void loop();

    void on_toolButton_pressed();
    void on_portButton_clicked();

    void on_comboBoxPorts_currentTextChanged(const QString &arg1);
    void read_ip_port();

    void on_B_emulateRobots_clicked();

private:
    Ui::MainWindow *ui;
    BTport * tmp_port;
    QUdpSocket * ip_port;
    QTimer timer;
    MsgReader udp_reader;
    static QStatusBar *sbar;
    static MainWindow *_this;


    void loop_tmp_port();
    void loop_wifi();
};
#endif // MAINWINDOW_H
