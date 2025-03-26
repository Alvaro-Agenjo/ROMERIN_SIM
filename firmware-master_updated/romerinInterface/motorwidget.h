#ifndef MOTORWIDGET_H
#define MOTORWIDGET_H

#include <QWidget>
class BTserialPort;
namespace Ui {
class MotorWidget;
}
#include "../include/RomerinMessage.h"
class ModuleController;
class MotorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MotorWidget(QWidget *parent = nullptr, int i=0);
    ~MotorWidget();
    void updateInfo(MotorInfoData &minfo);
    void updateFixedInfo(FixedMotorInfoData &minfo);
    void updateMotorStatus();
    void setModuleController(ModuleController *module){_module=module;};
private slots:
    void on_pushButton_toggled(bool checked);

    void on_angleSlider_sliderMoved(int position);

    void on_button_track_toggled(bool checked);

    void on_button_go_clicked();

    void on_button_reboot_clicked();

    void on_dial_speed_valueChanged(int value);

    void on_Torque_clicked();

private:
    Ui::MotorWidget *ui;
    MotorInfoData info_motor;
    ModuleController *_module;
    int motor_id;
};

#endif // MOTORWIDGET_H
