#ifndef MODULEEMULATOR_H
#define MODULEEMULATOR_H


#include "../../include/RomerinMessage.h"
#include <QJsonObject>
#include <QJsonArray>
#include <list>
#include <time.h>

#include "../../shared/romkin.h"

#include "rom_adapted/RomJointsA.h"
#include "rom_adapted/RomerinDefinitionsA.h"
#include "rom_adapted/RomSuctionCupA.h"

using std::list;
struct MinMaxA{uint16_t min,max;};
/*class RomKinMod{
    double L1, L2, L3, L4, L5, L6;
public:
    void set_lenghts(uint16_t l[6] ){
        L1=static_cast<double>(l[0]);L2=static_cast<double>(l[1]);
        L3=static_cast<double>(l[2]);L4=static_cast<double>(l[3]);
        L5=static_cast<double>(l[4]);L6=static_cast<double>(l[5]);
    }
    bool IKwrist(double* q, double x, double y, double z, bool elbow = true){
        RomKin::set_lenghts(L1, L2, L3, L4, L5, L6);
        return RomKin::IKwrist(q,x,y,z,elbow);
    }
    void q2m(double m[], double q[], bool gdl3 = false){RomKin::q2m(m,q,gdl3);}
    void m2q(double q[], double m[], bool gdl3 = false){RomKin::m2q(q,m,gdl3);}
    void FKfast(const double* q, double m[][3], double p[]){
        RomKin::set_lenghts(L1, L2, L3, L4, L5, L6);
        RomKin::FKfast(q,m,p);
    }
    bool IKfast(double* q, const double m[][3], const double p[], bool elbow = true, bool wrist = true){
        RomKin::set_lenghts(L1, L2, L3, L4, L5, L6);
        RomKin::IKfast(q,m,p,elbow,wrist);
    }
};*/

 void operator << (char out[], const QString &in);

class MainWindow;
struct ModuleEmulator:RomDefs{
 MainWindow *mainwnd=nullptr;
 int id;

 ulong millis(){
     return static_cast<long>(clock()/ (0.001*CLOCKS_PER_SEC));
 }
 ulong ltime=0;
//MODULE MAIN COMPONENTS

 RomJoints joints;
 RomState  state;
 RomSuctionCup suction_cup;
 RomKin romkin{};
 RobotCompactData robot_compact_data{};
 bool attached{};
 //MODULE FUNCTIONS EMULATION (main.cpp)
 void fill_robot_compact_data();
 void setup();
 void loop();
 RomerinMsg executeMessage(const RomerinMsg &m);
 void updateState();
 void sendRegularMessages();

//MODULE utilities emulation (RomDebug.h)
 void sendText(const char *text);
 void sendPrint(const char *fmt, ...);
 void sendMessage(const RomerinMsg &m);
 //OBJECT CONSTRUCTION
 void fromJson(const QJsonObject &json);
 ModuleEmulator(MainWindow *mw=nullptr,QString _name="NONE", int _id=0):mainwnd{mw},id{_id}{MODULE_NAME << _name;}

} ;

#endif //MODULEEMULATOR_H
