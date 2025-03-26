#include "configdlg.h"
#include "ui_configdlg.h"
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include "romerinmodule.h"

ConfigDlg::ConfigDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigDlg)
{
    ui->setupUi(this);
    QString ipRange = "(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])";
    QRegularExpression ipRegex ("^" + ipRange
                     + "\\." + ipRange
                     + "\\." + ipRange
                     + "\\." + ipRange + "$");

    auto *ipValidator = new QRegularExpressionValidator(ipRegex, this);
    ui->edit_ip->setValidator(ipValidator);
    auto *gatewayValidator = new QRegularExpressionValidator(ipRegex, this);
    ui->edit_ip->setValidator(gatewayValidator);
    auto *maskValidator = new QRegularExpressionValidator(ipRegex, this);
    ui->edit_ip->setValidator(maskValidator);
    connect(ui->button_flash, SIGNAL(clicked(bool)), this, SLOT(on_button_flash_clicked()));
    connect(ui->button_ok, SIGNAL(clicked(bool)), this, SLOT(on_button_ok_clicked()));
}

ConfigDlg::~ConfigDlg()
{
    delete ui;
}
void ConfigDlg::setConfigInfo(ConfigurationInfo &info)
{
    _info=info;
    //update controls
    ui->edit_name->setText(info.name);
    ui->edit_wifi->setText(info.wifi);
    ui->edit_passwd->setText(info.key);
    ui->edit_ip->setText(QString("%1.%2.%3.%4").arg(info.ip[0]).arg(info.ip[1]).arg(info.ip[2]).arg(info.ip[3]));
    ui->edit_gateway->setText(QString("%1.%2.%3.%4").arg(info.gateway[0]).arg(info.gateway[1]).arg(info.gateway[2]).arg(info.gateway[3]));
    ui->edit_mask->setText(QString("%1.%2.%3.%4").arg(info.mask[0]).arg(info.mask[1]).arg(info.mask[2]).arg(info.mask[3]));
}

/*#ifdef _WIN32
    #include <Ws2tcpip.h>
#else
    #include <sys/socket.h>
    #include <sys/types.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
#endif*/

#include <QHostAddress>
static void QString2ipv4(const QString &sip, uint8_t ip[]){
    QHostAddress ipAddress(sip);
    auto ip32=ipAddress.toIPv4Address();
    ip[0]=(ip32 >> 24) & 0xFF;
    ip[1]=(ip32 >> 16) & 0xFF;
    ip[2]=(ip32 >> 8) & 0xFF;
    ip[3]= ip32  & 0xFF;
}
void ConfigDlg::on_button_flash_clicked()
{
    QString2ipv4(ui->edit_ip->text(),_info.ip);
    QString2ipv4(ui->edit_gateway->text(),_info.gateway);
    QString2ipv4(ui->edit_mask->text(),_info.mask);
    /*inet_pton(AF_INET,ui->edit_ip->text().toStdString().c_str(),_info.ip);
    inet_pton(AF_INET,ui->edit_gateway->text().toStdString().c_str(),_info.gateway);
    inet_pton(AF_INET,ui->edit_mask->text().toStdString().c_str(),_info.mask);*/
    _info.name=ui->edit_name->text();
    _info.wifi=ui->edit_wifi->text();
    _info.key=ui->edit_passwd->text();
    ((RomerinModule *)parent())->sendConfig(_info);
}

void ConfigDlg::on_button_ok_clicked()
{
    this->close();
}
