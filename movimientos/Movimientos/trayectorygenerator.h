#ifndef TRAYECTORYGENERATOR_H
#define TRAYECTORYGENERATOR_H

#include <QTimer>
#include "modulecontroller.h"
#include "MTHomogenea.h"

#define standby 5
enum class command_t { STAND, RELAX, FIXED_ROTATION, RESET, MOVE_TO_POINT};
enum class order{};
class trayectoryGenerator : public QObject
{

public:
    trayectoryGenerator();

    void setVel(float max_vel, int motor_id);
    void setMotorAngle(ModuleController *module, double angle[]);
    void setAdhesion(ModuleController *module, int percentaje);

    bool validateMovement(double angle[],ModuleController *module, double x, double y, double z, float RPY[3], bool elbow = true);
    bool moveLeg(QString leg, double x, double y, double z, bool elbow = true, bool fixed = false);
    bool moveLeg(QString leg, double x, double y, double z, float RPY[3], bool elbow = true, bool fixed = false);
    bool moveLeg(ModuleController *module, double x, double y, double z, float RPY[], bool elbow, bool fixed);

    void Calc3x3ROT(float a, float b, float c, double ortientacion[][3]);

    bool moveBotAbsolute(Vector3D new_center, float RPY[3] = nullptr);
    bool moveBotRelative(Vector3D new_center, float RPY[3] = nullptr);
    void reset();
    void stand();
    void relax();

    void addOrder(command_t order){ order_list.push_back(order);}
    bool nextOrder();
    void test(bool elbow);
    bool isMoving();
private:
    float a = 0;
    QTimer timer;
    std::list<command_t> order_list{};

    Vector3D center;
    Matriz_Transformacion centro2leg_DU, centro2leg_IU, centro2leg_ID, centro2leg_DD;
};

#endif // TRAYECTORYGENERATOR_H
