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
    const int numrow=6;
    QString param[numrow]={"L1","L2", "L3", "L4", "L5", "L6"};
    ui->table_param->setRowCount(numrow);
    for(int row=0;row<numrow;row++){
        auto item1 = new QTableWidgetItem(param[row]);
        item1->setFlags(item1->flags() & ~Qt::ItemIsEditable);
        ui->table_param->setItem(row, 0, item1);

        // Segunda columna editable
        auto *item2 = new QTableWidgetItem(QString::number(0));
        item2->setTextAlignment(Qt::AlignCenter);
        ui->table_param->setItem(row, 1, item2);
    }
    ui->table_param->resizeColumnToContents(0);
    ui->table_param->horizontalHeader()->setStretchLastSection(true);


    ui->table_param_2->setRowCount(numrow);
    for(int row=0;row<numrow;row++)for(int j=0;j<5;j++){
         auto item=new QTableWidgetItem(QString::number(0));
        item->setTextAlignment(Qt::AlignCenter);
         ui->table_param_2->setItem(row, j, item);

    }
    ui->table_param_2->resizeColumnToContents(0);
    //ui->table_param->horizontalHeader()->setStretchLastSection(true);

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
void ConfigDlg::setConfigInfo(ConfigurationInfoV2 &info)
{
    _infoV2=info;
    for(int i=0;i<6;i++){
        ui->table_param->item(i,1)->setText(QString::number(info.lenghts[i]));
        ui->table_param_2->item(i,0)->setText(QString::number(info.min[i]));
        ui->table_param_2->item(i,1)->setText(QString::number(info.max[i]));
        ui->table_param_2->item(i,2)->setText(QString::number(info.offset[i]));
        ui->table_param_2->item(i,3)->setText(QString::number(info.vel_profile[i]));
        ui->table_param_2->item(i,4)->setText(QString::number(info.acc_profile[i]));
    }
    ui->e_X->setText(QString::number(info.position[0]));
    ui->e_Y->setText(QString::number(info.position[1]));
    ui->e_Z->setText(QString::number(info.position[2]));
    ui->e_RX->setText(QString::number(info.orientation[0]));
    ui->e_RY->setText(QString::number(info.orientation[1]));
    ui->e_RZ->setText(QString::number(info.orientation[2]));

    ui->cb_compact_mode->setChecked(info.compact_mode);
    ui->cb_disable_bt->setChecked(info.disable_bt);
}



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

    _info.name=ui->edit_name->text();
    _info.wifi=ui->edit_wifi->text();
    _info.key=ui->edit_passwd->text();

    //get and validate the table data:
    for(int i=0;i<6;i++){
        uint16_t val; bool ok; uchar_t val2;
        val=ui->table_param->item(i,1)->text().toInt(&ok); if(ok) _infoV2.lenghts[i]=val;
        val=ui->table_param_2->item(i,0)->text().toInt(&ok); if(ok) _infoV2.min[i]=val;
        val=ui->table_param_2->item(i,1)->text().toInt(&ok); if(ok) _infoV2.max[i]=val;
        val2=ui->table_param_2->item(i,2)->text().toInt(&ok); if(ok) _infoV2.offset[i]=val2;
        val2=ui->table_param_2->item(i,3)->text().toInt(&ok); if(ok) _infoV2.vel_profile[i]=val2;
        val2=ui->table_param_2->item(i,4)->text().toInt(&ok); if(ok) _infoV2.acc_profile[i]=val2;

    }

    int16_t val; bool ok;
    val=ui->e_X->text().toInt(&ok); if(ok) _infoV2.position[0]=val;
    val=ui->e_Y->text().toInt(&ok); if(ok) _infoV2.position[1]=val;
    val=ui->e_Z->text().toInt(&ok); if(ok) _infoV2.position[2]=val;
    val=ui->e_RX->text().toInt(&ok); if(ok) _infoV2.orientation[0]=val;
    val=ui->e_RY->text().toInt(&ok); if(ok) _infoV2.orientation[1]=val;
    val=ui->e_RZ->text().toInt(&ok); if(ok) _infoV2.orientation[2]=val;
    _infoV2.compact_mode=ui->cb_compact_mode->isChecked();
    _infoV2.disable_bt=ui->cb_disable_bt->isChecked();
    ((RomerinModule *)parent())->sendConfig(_info);
    ((RomerinModule *)parent())->sendConfigV2(_infoV2);
}

void ConfigDlg::on_button_ok_clicked()
{
    this->close();
}
