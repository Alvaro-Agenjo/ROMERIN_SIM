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
    static ConfigurationInfo getFromQByte(QByteArray &m)
    {
        ConfigurationInfo aux;
        int pos=0;
        for(int i=0;i<4;i++)aux.ip[i]=m[pos++];
        for(int i=0;i<4;i++)aux.gateway[i]=m[pos++];
        for(int i=0;i<4;i++)aux.mask[i]=m[pos++];
        m.remove(0,pos);
        QList<QByteArray> texts = m.split(0);
        aux.name=texts[0];
        aux.wifi=texts[1];
        aux.key=texts[2];
        return aux;
    }

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
