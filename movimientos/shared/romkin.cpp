#include <iostream>


#include <cmath>
#include "romkin.h"

//default romerin constants////////////////////////////////////////////////
//ROMERIN ARM CONSTANTS


#ifdef ARDUINO
  template<class REAL> 
  typename _RomKin<REAL>::DH _RomKin<REAL>::DH_data[6]{};
#endif // !ARDUINO

template<class REAL>
void _RomKin<REAL>::set_lenghts(REAL l1, REAL l2, REAL l3, REAL l4, REAL l5, REAL l6)
{
    L1 = l1; L2 = l2; L3 = l3; L4 = l4; L5 = l5; L6 = l6;
    Lc = sqrt(L2*L2+L3*L3);
    Ld = sqrt(L4*L4+L5*L5);
    alpha = atan2(L3,L2);
    beta = atan2(L5, L4);
    cosb = cos(beta);
    sinb = sin(beta);
    //actualizar la tabla de D-H despues del set
    DH_data[0] = DH{ 0, 0, L1, M_PI / 2 };
    DH_data[1] = DH{ -alpha, 0, Lc, M_PI };
    DH_data[2] = DH{ -M_PI / 2 - alpha,	0, L5, M_PI / 2 };
    DH_data[3] = DH{ 0,	-L4, 0, M_PI / 2 };
    DH_data[4] = DH{ 0,	0, 0, M_PI / 2 };
    DH_data[5] = DH{ -M_PI / 2,	L6,	0, 0};
}
template<class REAL>
void _RomKin<REAL>::set_lenghts(uint16_t l[6]){
    set_lenghts(static_cast<double>(0.001*l[0]),static_cast<double>(0.001*l[1]),
                static_cast<double>(0.001*l[2]),static_cast<double>(0.001*l[3]),
                static_cast<double>(0.001*l[4]),static_cast<double>(0.001*l[5]));
}
//COMMON METHODS //////////////////////////////////////////////////////////
template<class REAL>
bool _RomKin<REAL>::IKfast(REAL* q, const REAL m[][3], const REAL p[], bool elbow, bool wrist)
{
    REAL pm[3] = { p[0] - L6 * m[0][2],p[1] - L6 * m[1][2],p[2]-L6*m[2][2]};
    
    if (!IKwrist(q, pm[0], pm[1], pm[2], elbow))return false;
    REAL q1 = q[0], q2 = q[1] - alpha, q3 = q[2] - alpha;
    REAL s1 = sin(q1), c1 = cos(q1);
    REAL c2_3 = cos(q2 - q3), s3_2 = sin(q3 - q2);
    REAL A[3][3]={ {c1 * s3_2, -s1, -c1* c2_3}, //A03
                      {s1 * s3_2,  c1, -s1* c2_3},
                      {c2_3       ,   0, s3_2}};
    REAL R[3][3];//R = A.trasp * m
    for (int i = 0; i < 3; i++) {
        R[i][0] = A[0][i] * m[0][0] + A[1][i] * m[1][0] + A[2][i] * m[2][0];
        R[i][1] = A[0][i] * m[0][1] + A[1][i] * m[1][1] + A[2][i] * m[2][1];
        R[i][2] = A[0][i] * m[0][2] + A[1][i] * m[1][2] + A[2][i] * m[2][2];
    }

    //si q5!=0 ojo.
    if (abs(R[2][2]) < 0.99) {
        //dando prioridad a q4 (-pi/2, pi/2), por lo que c4>0
        if (R[0][2] > 0) {//q5 >0
            q[3] = atan2(R[1][2], R[0][2]);
            q[4] = acos(-R[2][2]);
            q[5] = atan2(R[2][0], R[2][1]);
        }
        else {
            q[3] = atan2(-R[1][2], -R[0][2]);
            q[4] = -acos(-R[2][2]); //esto impide abs(q5) > pi/2
            q[5] = atan2(-R[2][0], -R[2][1]);
        }
    }
    else //pata extendida, q4 y q6 acoplados, no se resuelve de momento
    {
        //for removing the unused warning
        if(!wrist)wrist=false;
        return false;
    }

    return true;

}
template<class REAL>
void _RomKin<REAL>::FKfast(const REAL* q, REAL m[][3], REAL p[])
{
    REAL c1 = cos(q[0]), s1 = sin(q[0]), c2 = cos(q[1]), s2 = sin(q[1]);
    REAL c3 = cos(q[2]), s3 = sin(q[2]), c4 = cos(q[3]), s4 = sin(q[3]);
    REAL c5 = cos(q[4]), s5 = sin(q[4]), c6 = cos(q[5]), s6 = sin(q[5]);

    m[0][0] = c1 * c3 * s2* (c6 * s4 - c4 * c5 * s6) + c1 * c2 * s6 * (c4 * c5 * s3  -  c3 * s5 ) - c5 * s1 * s4 * s6 - c1 * c2 * c6 * s3 * s4 - c4 * c6 * s1  - c1 * s2 * s3 * s5 * s6  ;
    m[0][1] = c4 * s1 * s6 - c5 * c6 * s1 * s4 - c1 * c2 * c3 * c6 * s5 + c1 * c2 * s3 * s4 * s6 - c1 * c3 * s2 * s4 * s6 - c1 * c6 * s2 * s3 * s5 + c1 * c2 * c4 * c5 * c6 * s3 - c1 * c3 * c4 * c5 * c6 * s2;
    m[0][2] = c1* c2* c3* c5 - s1 * s4 * s5 + c1 * c5 * s2 * s3 + c1 * c2 * c4 * s3 * s5 - c1 * c3 * c4 * s2 * s5;
    p[0] = L1 * c1 + Lc * cos(alpha) * c1 * c2 + L4 * c1 * c2 * c3 + L5 * c1 * c2 * s3 - L5 * c1 * c3 * s2 + Lc * c1 * sin(alpha) * s2 + L4 * c1 * s2 * s3 - L6 * s1 * s4 * s5 + L6 * c1 * c5 * s2 * s3 + L6 * c1 * c2 * c3 * c5 + L6 * c1 * c2 * c4 * s3 * s5 - L6 * c1 * c3 * c4 * s2 * s5;


    m[1][0] = c1 * c4 * c6 + c1 * c5 * s4 * s6 - c2 * c6 * s1 * s3 * s4 + c3 * c6 * s1 * s2 * s4 - c2 * c3 * s1 * s5 * s6 - s1 * s2 * s3 * s5 * s6 + c2 * c4 * c5 * s1 * s3 * s6 - c3 * c4 * c5 * s1 * s2 * s6;
    m[1][1] = c1 * c5 * c6 * s4 - c1 * c4 * s6 - c2 * c3 * c6 * s1 * s5 + c2 * s1 * s3 * s4 * s6 - c3 * s1 * s2 * s4 * s6 - c6 * s1 * s2 * s3 * s5 + c2 * c4 * c5 * c6 * s1 * s3 - c3 * c4 * c5 * c6 * s1 * s2;
    m[1][2] = c1 * s4 * s5 + c2 * c3 * c5 * s1 + c5 * s1 * s2 * s3 + c2 * c4 * s1 * s3 * s5 - c3 * c4 * s1 * s2 * s5;
    p[1] = L1* s1 + Lc * cos(alpha) * c2 * s1 + L4 * c2 * c3 * s1 + L5 * c2 * s1 * s3 - L5 * c3 * s1 * s2 + L6 * c1 * s4 * s5 + Lc * sin(alpha) * s1 * s2 + L4 * s1 * s2 * s3 + L6 * c2 * c3 * c5 * s1 + L6 * c5 * s1 * s2 * s3 + L6 * c2 * c4 * s1 * s3 * s5 - L6 * c3 * c4 * s1 * s2 * s5;

    m[2][0] = c2 * s3 * s5 * s6 - c6 * s2 * s3 * s4 - c2 * c3 * c6 * s4 - c3 * s2 * s5 * s6 + c2 * c3 * c4 * c5 * s6 + c4 * c5 * s2 * s3 * s6,
        m[2][1] = c2 * c3 * s4 * s6 + c2 * c6 * s3 * s5 - c3 * c6 * s2 * s5 + s2 * s3 * s4 * s6 + c2 * c3 * c4 * c5 * c6 + c4 * c5 * c6 * s2 * s3,
        m[2][2] = c3 * c5 * s2 - c2 * c5 * s3 + c2 * c3 * c4 * s5 + c4 * s2 * s3 * s5,
        p[2] = L5 * c2 * c3 + Lc * cos(alpha) * s2 - Lc * c2 * sin(alpha) - L4 * c2 * s3 + L4 * c3 * s2 + L5 * s2 * s3 - L6 * c2 * c5 * s3 + L6 * c3 * c5 * s2 + L6 * c2 * c3 * c4 * s5 + L6 * c4 * s2 * s3 * s5;
       
}
template <class REAL>
bool _RomKin<REAL>::IKwrist(REAL* q, REAL x, REAL y, REAL z, bool elbow)
{
    q[0] = atan2(y, x);
    x -= L1 * cos(q[0]);
    y -= L1 * sin(q[0]);

    REAL num = x * x + y * y + z * z - Lc * Lc - Ld * Ld; //positive q3 (elbow up)
    const REAL den = 2 * Lc * Ld;
    if (abs(num) > den) return false;//out of range
    q[2] = (elbow ? 1 : -1) * acos(num / den);
    q[1] = atan2(z, sqrt(x * x + y * y)) + (elbow ? 1 : -1) * atan2(Ld * sin(q[2]), Lc + Ld * cos(q[2]));

    q[1] += alpha;
    q[2] += (alpha + beta);
    return true;
}

//from q angles to motor values(also angles but deg and with differente offset)
template<class REAL>
void _RomKin<REAL>::q2m(REAL m[], REAL q[], bool gdl3)
{
    m[0] = 180.0 + rad2deg * q[0];
    m[1] = 180.0 + rad2deg * q[1];
    m[2] = 90.0 + rad2deg * q[2];
    if (gdl3)return;
    m[3] = 180 + rad2deg * q[3] / factor_4;
    m[4] = rad2deg * (q[5] / factor_6 - q[4] ) + 180;
    m[5] = rad2deg * (q[4]  + q[5] / factor_6) + 180;

    m[4]+= 360;
    m[5]+= 360;
    // while(m[4]<0 || m[5]<0){
    //     m[4]+= 360;
    //     m[5]+= 360;
    // }
}
// from m values to q angles
template<class REAL>
void _RomKin<REAL>::m2q(REAL q[], REAL m[], bool gdl3) {

    q[0] = (m[0] - 180) * deg2rad;
    q[1] = (m[1] - 180) * deg2rad;
    q[2] = (m[2] - 90) * deg2rad;
    if (gdl3)return;
    q[3] = (m[3] - 180.0) * deg2rad * factor_4;
    q[4] = (m[5] - m[4]) * deg2rad /2;
    q[5] = ((m[5] + m[4]) / 2 - 180) * deg2rad * factor_6;
}

// from m torque  to q torques
template<class REAL>
void _RomKin<REAL>::mt2qt(REAL qt[], REAL mt[], bool gdl3){
    qt[0] = mt[0]*0.0015 ; //MX-64 6Nm = 4A
    qt[1] = mt[1]*0.002 ; //XH540-270 10Nm= 5A
    qt[2] = mt[2]*0.002 ; //XH540-270 10Nm= 5A
    if (gdl3)return;
    qt[3] = mt[3] * factor_4 * 0.00135; //XM430-240  1.9Nm 1.4A
    qt[4] = (mt[5] - mt[4]) * 0.00135;
    qt[5] = (mt[5] + mt[4])  * factor_6 * 0.00135;
}


//NON ARDUINO METHODS DEFINITIONS//////////////////////////////////////////
#ifndef ARDUINO

/*NOTAS: obtener la matriz de rotacion de una 4d:
Matrix3d rotation = T.block<3,3>(0,0);

obtener el cuaternio:
Eigen::Quaterniond quat(rotation)
*/
template<class REAL>
Vector3d _RomKin<REAL>::FKwristDH(REAL q1, REAL q2, REAL q3)
{
    Matrix4d T = rom_dh_matrix(q1, 0) * rom_dh_matrix(q2, 1) *
        rom_dh_matrix(q3, 2) * rom_dh_matrix(0, 3);
    return T.block<3, 1>(0, 3);
}


template<class REAL>
Matrix4d _RomKin<REAL>::FK(REAL q1, REAL q2, REAL q3, REAL q4, REAL q5, REAL q6)
{
    Matrix4d T = rom_dh_matrix(q1, 0) * rom_dh_matrix(q2, 1) *
        rom_dh_matrix(q3, 2) * rom_dh_matrix(q4, 3) *
        rom_dh_matrix(q5, 4) * rom_dh_matrix(q6, 5);
    return T;
}


template<class REAL>
Vector3d _RomKin<REAL>::FKwrist(REAL q1, REAL q2, REAL q3)
{
    q2 -= alpha;
    q3 -= (alpha + beta); 
    REAL L = L1 + Lc * cos(q2) + Ld * cos(q2 - q3);
    Vector3d pos;
    pos(0) = cos(q1) * L;
    pos(1) = sin(q1) * L;
    pos(2) = Lc * sin(q2) + Ld * sin(q2 - q3);
    return pos;
}

//podria ser interesante incluir las q actuales para resolver ambiguedades de orientacion
//TODO: verificar. mirar el cuaderno
template<class REAL>
bool _RomKin<REAL>::IK(REAL* q, Matrix4d T, bool elbow, bool wrist)
{
    Vector3d pm = T.block<3,1>(0,3)-L6*(T.block<3,1>(0,2));
    if(!IKwrist(q, pm(0), pm(1), pm(2), elbow))return false;
    //this could be easily optimized, but integrity first
    Matrix4d A03 = rom_dh_matrix(q[0], 0) * rom_dh_matrix(q[1], 1) * rom_dh_matrix(q[2], 2);
    Matrix3d R03 = A03.block<3,3>(0,0);
    Matrix3d R06 = T.block<3,3>(0,0);
    Matrix3d R36 = R03.transpose()*R06, &R=R36;
    //si q5!=0 ojo.
    if(abs(R(2,2))<0.99){
        //dando prioridad a q4 (-pi/2, pi/2), por lo que c4>0
        if(R(0,2)>0){//q5 >0
            q[3]=atan2(R(1,2),R(0,2));
            q[4]=acos(-R(2,2));
            q[5]=atan2(R(2,0),R(2,1));
        }else{
            q[3]=atan2(-R(1,2),-R(0,2));
            q[4]=-acos(-R(2,2)); //esto impide abs(q5) > pi/2
            q[5]=atan2(-R(2,0),-R(2,1));
        }
    }else //pata extendida, q4 y q6 acoplados, no se resuelve de momento
    {
        //for removing the unused warning
        if(!wrist)wrist=false;
        return false;
    }

    return true;
}

#endif // !ARDUINO


#ifndef ARDUINO
template struct _RomKin<double>;
#else
template struct _RomKin<float>;
#endif // !ARDUINO
