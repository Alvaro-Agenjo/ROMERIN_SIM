#ifndef CONFIGDLG_H
#define CONFIGDLG_H

#include <QDialog>

struct ConfigurationInfo
{
    QString name;
    QString wifi;
    QString key;
    uint8_t ip[4];
    uint8_t gateway[4];
    uint8_t mask[4];

};

namespace Ui {
class ConfigDlg;
}

class ConfigDlg : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigDlg(QWidget *parent = nullptr);
    ~ConfigDlg();
    void setConfigInfo(ConfigurationInfo &info);
private slots:
    void on_button_flash_clicked();

    void on_button_ok_clicked();

private:
    Ui::ConfigDlg *ui;
    ConfigurationInfo _info;

};

#endif // CONFIGDLG_H
