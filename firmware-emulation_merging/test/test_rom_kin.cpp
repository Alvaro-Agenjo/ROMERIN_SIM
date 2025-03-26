#include <iostream>
#include <iomanip>
#include "../shared/romkin.h"
using Eigen::Matrix4d;

using std::cout, std::endl;
template <class REAL>
void print(REAL m[3][3], REAL p[3]) {
    for(int i =0;i<3;i++)
        cout << "\n" << m[i][0] << ", " << m[i][1] << ", " << m[i][2] << ", " << p[i] << endl;

}
int main() {
    // Parámetros DH (theta, d, a, alpha) para cada articulación
    RomKin::init();
    RomKin::Real q_test[][6] = {
        {0,0.2, 0.2,0,0.2,0.2},                 // 0.6586,0,-006804
        {0.3,0.2, 0.2,0.4,0.2,0.2},          //0.0000    0.6586   -0.0068
        {-0.45,0.5, 0.5,0,0.2,0.2},   // 0.0000    0.0788    0.5866
        {0,1, 0.5,0,0.2,0.2}, //0.0000    0.4376    0.2141
        {0,1, 1.5,1.2,0.5,0.2}, //0.0000    0.4376    0.2141
        { 0,1, 1.5,1.2,-0.4,0.2 } //0.0000    0.4376    0.2141
    };
    cout << std::fixed << std::setprecision(4);
    auto m = RomKin::DH_data;
    for (auto& q : q_test) {
        auto p1 = RomKin::FKwrist(q[0], q[1], q[2]);
        auto p2 = RomKin::FKwristDH(q[0], q[1], q[2]);
        cout << q[0] << ", " << q[1] << ", " << q[2] << "-->" << endl;
        cout << "FK --> " << p1(0) << ", " << p1(1) << ", " << p1(2) << endl;
        cout << "DH --> " << p2(0) << ", " << p2(1) << ", " << p2(2) << endl;
        RomKin::Real qi[3]{};
        if (RomKin::IKwrist(qi, p1(0), p1(1), p1(2)))
            cout << "-IK->" << qi[0] << ", " << qi[1] << ", " << qi[2] << endl;
        else cout << "SIN SOLUCION\n";

        cout << "---------------------" << endl;
        RomKin::Real m[6],qc[6];
        for (auto v : q)cout << v << ", ";
        cout << "--> to motor " << endl;
        RomKin::q2m(m, q);
        for (auto v : m)cout << v << ", ";
        cout << "--> to q " << endl;
        RomKin::m2q(qc, m);
        for (auto v : qc)cout << v << ", ";
        cout << "\n---------------------" << endl;
    }
    
    for (auto& q : q_test) {
        RomKin::Real m[3][3], p[3];
        auto T1 = RomKin::FK(q[0], q[1], q[2], q[3], q[4], q[5]);
        
        cout << endl <<"Q -->";
        for (auto qq : q) cout << qq << ", ";
        
        RomKin::Real qi[6]{};
        if (RomKin::IK(qi, T1)) {
            cout << "\n-IK->";
            for (auto qq : qi) cout << qq << ", ";
        }
        else cout << "SIN SOLUCION\n";

        cout << endl << T1 << endl<<"----------------";
        RomKin::Real qf[6] = { q[0],q[1],q[2], q[3], q[4],q[5] };
        RomKin::FKfast(qf, m, p);
        print(m, p);
        RomKin::Real qfi[6];
        if (RomKin::IKfast(qfi, m,p)) {
            cout << "\n-IKfast->";
            for (auto qq : qfi) cout << qq << ", ";
        }
        else cout << "SIN SOLUCION\n";
    }
    return 0;
}