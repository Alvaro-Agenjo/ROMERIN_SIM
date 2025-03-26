#ifndef MOTORWIDGET_H
#define MOTORWIDGET_H

#include <QWidget>
class BTserialPort;
namespace Ui {
class MotorWidget;
}
struct MotorInfo
{
 float position = 0;
 float velocity = 0;
 float intensity = 0;
 uint8_t temperature = 0;
 float voltage = 0;
 uint8_t status = 0;
};
struct FixedMotorInfo
{
    float max_angle =0;
    float min_angle=10.0F;
};

class MotorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MotorWidget(QWidget *parent = nullptr, int i=0);
    ~MotorWidget();
    void updateInfo(MotorInfo &minfo);
    void updateFixedInfo(FixedMotorInfo &minfo);
    void updateMotorStatus();
    void setPort(BTserialPort *port){_port=port;};
private slots:
    void on_pushButton_toggled(bool checked);

    void on_angleSlider_sliderMoved(int position);

    void on_button_track_toggled(bool checked);

    void on_button_go_clicked();

    void on_button_reboot_clicked();

    void on_dial_speed_valueChanged(int value);

private:
    Ui::MotorWidget *ui;
    MotorInfo info_motor;
    BTserialPort *_port;
    int motor_id;
};

#endif // MOTORWIDGET_H
