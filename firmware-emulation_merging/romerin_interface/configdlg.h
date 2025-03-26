#ifndef CONFIGDLG_H
#define CONFIGDLG_H

#include <QDialog>
#include "../include/RomerinMessage.h"
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
struct ConfigurationInfoV2{
    uint16_t lenghts[6]{};
    uint16_t min[6]{};
    uint16_t max[6]{};
    int8_t offset[6]{};
    uint8_t vel_profile[6]{};
    uint8_t acc_profile[6]{};
    int16_t position[3];
    int16_t orientation[3];
    bool disable_bt{};
    bool compact_mode{};
    static ConfigurationInfoV2 getFromBuffer(const unsigned char *buffer)
    {
        ConfigurationInfoV2 aux;
        for(int i=0;i<6;i++){
            aux.lenghts[i]=romerin_getUInt16(buffer);buffer+=2;
            aux.min[i]=romerin_getUInt16(buffer);buffer+=2;
            aux.max[i]=romerin_getUInt16(buffer);buffer+=2;
            aux.offset[i]=*(buffer++);
            aux.vel_profile[i]=*(buffer++);
            aux.acc_profile[i]=*(buffer++);
        }
        for(int i=0;i<3;i++){aux.position[i]=romerin_getInt16(buffer);buffer+=2;}
        for(int i=0;i<3;i++){aux.orientation[i]=romerin_getInt16(buffer);buffer+=2;}
        uint8_t init_configuration = *(buffer++);
        aux.compact_mode=init_configuration&0x01;
        aux.disable_bt=init_configuration&0x10;
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
    void setConfigInfo(ConfigurationInfoV2 &info);
    const ConfigurationInfoV2& getConfigInvoV2(){return _infoV2;}
private slots:
    void on_button_flash_clicked();

    void on_button_ok_clicked();

private:
    Ui::ConfigDlg *ui;
    ConfigurationInfo _info;
    ConfigurationInfoV2 _infoV2;

};

#endif // CONFIGDLG_H
