#ifndef ROMERINMODEL_H
#define ROMERINMODEL_H

#include <list>
#include <string>
using std::list, std::string;

#include <Eigen/Dense>
using Eigen::Matrix4d, Eigen::Vector3d, Eigen::Matrix3d;

struct ModuleModel
{
   string name{};
   Matrix4d base_pose{{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}}; //relative position (constant)
   Matrix4d wr_base_pose{{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}}; //updates its time the body moves
   Matrix4d wr_sc_pose{}; //suction cup pose referred to wr

   bool attached_sc{};
   uint8_t torque_state{};//lower bit, motor 0

   double q_deg[6]{};
   double torques[6]{};
   operator bool(){return !name.empty();}


};

struct RomerinModel
{

    bool wr_reference_defined; //true if there is a initiated wr
    Matrix4d wr_body_pose{{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}}; //valid only if reference is defined
    Vector3d wr_gravity{}; //gracity vector in wr coordinates m/s^2
    double body_mass{};
    list<ModuleModel> modules{};
    ModuleModel &operator[](const string &name){for(auto &m:modules)if(m.name==name)return m;return none;}
    inline static ModuleModel none{};
};

#endif // ROMERINMODEL_H
