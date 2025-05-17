#ifndef MODULE_H
#define MODULE_H

#include <QWidget>

#include "../shared/romkin.h"
#include "../include/RomerinMessage.h"
class ModuleController;

QT_BEGIN_NAMESPACE
namespace Ui {
class Module;
}
QT_END_NAMESPACE

class Module : public QWidget
{
    Q_OBJECT
public:
    explicit Module(QWidget *parent = nullptr);
    ~Module();

    void setModule(ModuleController * mod){module = mod;}
    void setMotorInfo(int id, const MotorInfoData &info) { motors[id] = info; }

    MotorInfoData getMotorInfo(int id) { return motors[id]; }



private:
    Ui::Module *ui;
    ModuleController *module;
    MotorInfoData motors[6];
    SuctionCupInfoData suction_cup;

public:
    RomKin romkin{};
};

#endif // MODULE_H
