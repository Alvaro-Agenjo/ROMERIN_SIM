#include <iostream>
#include <math.h>

#define alpha 0.06305190495
static constexpr double factor_4 = 24.0 / 32.0 ;
static constexpr double factor_6 = 34.0 / 51.0 ;
static constexpr double rad2deg = 180.0 / M_PI;
static constexpr double deg2rad = M_PI / 180.0;

void Calc3x3ROT(float a, float b, float c, double orientacion[][3])
{
    // Convertir grados a radianes
    a /= rad2deg;
    b /= rad2deg;
    c /= rad2deg;

    double cx = cos(a), sx = sin(a);
    double cy = cos(b), sy = sin(b);
    double cz = cos(c), sz = sin(c);

    // Matriz de rotación sobre ejes globales (Rx * Ry * Rz)
    orientacion[0][0] = cy * cz;
    orientacion[0][1] = -cy * sz;
    orientacion[0][2] = sy;

    orientacion[1][0] = sx * sy * cz + cx * sz;
    orientacion[1][1] = -sx * sy * sz + cx * cz;
    orientacion[1][2] = -sx * cy;

    orientacion[2][0] = -cx * sy * cz  + sx * sz;
    orientacion[2][1] = cx * sy * sz + sx * cz;
    orientacion[2][2] = cx * cy;
}

void IK(double *q, double m[3][3]){
    double q1 = q[0], q2 = q[1] - alpha, q3 = q[2] - alpha;
    double s1 = sin(q1), c1 = cos(q1);
    double c2_3 = cos(q2 - q3), s3_2 = sin(q3 - q2);
    
    //Test hecho desde cero
    double A[3][3] = { {c1 * s3_2,  -s1   , -c1 * c2_3}, //A03
                       {s1 * s3_2,   c1   , -s1 * c2_3},
                       {c2_3     ,   0    , s3_2     }};
   
   
    // double A[3][3]={ {c1 * s3_2, -s1, -c1* c2_3}, //A03
    //                   {s1 * s3_2,  c1, -s1* c2_3},
    //                   {c2_3       ,   0, s3_2}};
    double R[3][3];//R = A.trasp * m
    for (int i = 0; i < 3; i++) {
        R[i][0] = A[0][i] * m[0][0] + A[1][i] * m[1][0] + A[2][i] * m[2][0];
        R[i][1] = A[0][i] * m[0][1] + A[1][i] * m[1][1] + A[2][i] * m[2][1];
        R[i][2] = A[0][i] * m[0][2] + A[1][i] * m[1][2] + A[2][i] * m[2][2];
    }

    // sen(q5) != 0, por lo que q5 != 0,180
    if(abs(R[2][2]) < 0.99) {
        q[3] = atan2(R[1][2], R[0][2]);
        q[4] = acos(-R[2][2]);
        q[5] = atan2(R[2][1], -R[2][0]);

        // if (R[0][2] > 0) {//q5 >0
        //     q[3] = atan2(R[1][2], R[0][2]);
        //     q[4] = acos(-R[2][2]);
        //     q[5] = atan2(-R[2][1], R[2][0]);
        // }
        // else {
        //     q[3] = atan2(-R[1][2], -R[0][2]);
        //     q[4] = -acos(-R[2][2]); //esto impide abs(q5) > pi/2
        //     q[5] = atan2(-R[2][0], -R[2][1]);
        // }
    }
    else{
        //for removing the unused warning
        // if(!wrist)wrist=false;
        // return false; //pata extendida, no se resuelve de momento
    }
}

void q2m(double m[], double q[], bool gdl3)
{
    m[0] = 180.0 + rad2deg * q[0];
    m[1] = 180.0 + rad2deg * q[1];
    m[2] = 90.0 + rad2deg * q[2];
    if (gdl3)return;
    m[3] = 180 + rad2deg * q[3] / factor_4;
    m[4] = rad2deg * (q[5] / factor_6 - q[4] ) + 180;
    m[5] = rad2deg * (q[4]  + q[5] / factor_6) + 180;
}
// from m values to q angles
void m2q(double q[], double m[], bool gdl3) {

    q[0] = (m[0] - 180) * deg2rad;
    q[1] = (m[1] - 180) * deg2rad;
    q[2] = (m[2] - 90) * deg2rad;
    if (gdl3)return;
    q[3] = (m[3] - 180.0) * deg2rad * factor_4;
    q[4] = (m[5] - m[4]) * deg2rad /2;
    q[5] = ((m[5] + m[4]) / 2 - 180) * deg2rad * factor_6;
}


void printQs(double *q){
    for(int i=3; i<6; i++){
        std::cout << "q[" << i << "] = " << q[i]*rad2deg << std::endl;
    }
}
void printMatrix(double m[3][3]){
    for(int i=0; i<3; i++){
        for(int j=0; j<3; j++){
            std::cout << std::fixed << m[i][j] << " ";
        }
        std::cout << std::endl;
    }
}
int main(){

    double q[6] = {0, 0, 0, 0, 0, 0};
    double m[3][3];
    double RPY[3] = {0, 0, 0};

    for(int i = 0; i < 360; i += 5) {
    
        RPY[0] = i; RPY[1] = 0; RPY[2] =0; // Yaw

        Calc3x3ROT(RPY[0], RPY[1], RPY[2], m);
        std::cout << "RPY: " << RPY[0] << ", " << RPY[1] << ", " << RPY[2] << std::endl;
        // std::cout << "Matriz de rotacion:" << std::endl;
        // printMatrix(m); 

        IK(q, m);
        std::cout << "Valores de q:" << std::endl;
        printQs(q); 

    
        std::cout << "------------------------" << std::endl;
    }

    for(int i = 0; i < 360; i += 5) {
    
        RPY[0] = 0; RPY[1] = i; RPY[2] =0; // Yaw

        Calc3x3ROT(RPY[0], RPY[1], RPY[2], m);
        std::cout << "RPY: " << RPY[0] << ", " << RPY[1] << ", " << RPY[2] << std::endl;
        // std::cout << "Matriz de rotacion:" << std::endl;
        // printMatrix(m); 

        IK(q, m);
        std::cout << "Valores de q:" << std::endl;
        printQs(q); 

    
        std::cout << "------------------------" << std::endl;
    }

    for(int i = 0; i < 360; i += 60) {
    
        RPY[0] = 0; RPY[1] = 0; RPY[2] = i; // Yaw

        Calc3x3ROT(RPY[0], RPY[1], RPY[2], m);
        std::cout << "RPY: " << RPY[0] << ", " << RPY[1] << ", " << RPY[2] << std::endl;
        // std::cout << "Matriz de rotacion:" << std::endl;
        // printMatrix(m); 

        IK(q, m);
        std::cout << "Valores de q:" << std::endl;
        printQs(q); 

    
        std::cout << "------------------------" << std::endl;
    }
    return 0;
}