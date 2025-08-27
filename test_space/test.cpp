#include <iostream>
#include <fstream>
#include <math.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <vector>

void FKfast1(const double* q, double m[][3], double p[]);
void FKfast2(const double* q, double m[][3], double p[]);
bool Compare(const double m1[][3], const double p1[], const double m2[][3], const double p2[]);

bool IKfast(double* q, const double m[][3], const double p[], bool elbow, bool wrist = true);
bool IKwrist(double* q, double x, double y, double z, bool elbow);
bool calculo(double x);
void q2m(double m[], double q[]);
bool limits(double m[], double x);


void printVector(const double *vec, int i) {
    for (int n = 0; n<i; n++) {
        std::cout << vec[n] << " ";
    }
    std::cout << std::endl;
}
void printMatrix(const double m[][3], int i) {
    for (int n = 0; n<i; n++) {
        for (int j = 0; j<3; j++) {
            std::cout << m[n][j] << " ";
        }
        std::cout << std::endl;
    }
}

constexpr double L1 = 0.068;
constexpr double L2 = 0.236;
constexpr double L3 = 0.0149;
constexpr double L4 = 0.28;
constexpr double L5 = 0.022;
constexpr double L6 = 0.087;
double Lc = sqrt(L2 * L2 + L3 * L3);
double Ld = sqrt(L4 * L4 + L5 * L5);
double alpha = atan2(L3,L2);
double beta = atan2(L5,L4);
constexpr double rad2deg = 180.0 / M_PI;
constexpr double step = 0.005; // Incremento de x
constexpr double max_x = 0.7;



int main() {
    std::ofstream file("datos.txt");


    double q[3];
    double y = 0.0, z = 0.0;
    bool elbow = true;
    char cad[100];

    double m[3][3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}, m2[3][3]; // Matriz identidad
    double p[3] = {0.4,0,0.1}, p2[3];


    for(int q1= 70; q1 <= 290; q1+= 20){
        std::cout << "Q1: " << q1 << std::endl;
        for( int q2 = 70; q2 <= 290; q2+= 10){
            for(int q3 = 55; q3 <= 260; q3+= 10){
                for(int q4 = 0; q4 <= 360; q4+= 30){
                    for(int q5 = 0; q5 <= 360; q5+= 20){
                        for(int q6 = 0; q6 <= 360; q6+= 20){
                            double q[6] = {q1 * M_PI / 180.0, q2 * M_PI / 180.0, q3 * M_PI / 180.0, 
                                            q4 * M_PI / 180.0, q5 * M_PI / 180.0, q6 * M_PI / 180.0};
                            FKfast1(q, m, p);
                            FKfast2(q, m2, p2);
                            elbow = Compare(m, p, m2, p2);
                        }
                    }
                }
            }
        }                      
    }
    std::cout << "Finalizado, resultado = " << elbow << std::endl;


    // for(float x = 0.0; x <= max_x; x += step) {
    //     if(IKwrist(q, x, y, z, elbow)){
    //         std::cout << "X:" << x << " | Q1:" << q[0] << " | Q2:" << q[1] << " | Q3:" << q[2] << std::endl;
    //         // std::cout << "X:" << x << " | Q1:" << q[0] * rad2deg << " | Q2:" << q[1] * rad2deg << " | Q3:" << q[2] * rad2deg << std::endl;
    //         file << x << "\t" << q[0] << "\t" << q[1] << "\t" << q[2] << std::endl;
    //     }
    //     else{
           
    //         std::cout << "X:" << x << " | Q1:" << "---" << " | Q2:" << "---" << " | Q3:" << "---" << std::endl;
    //         file << "X:" << x << " | Q1:" << "---" << " | Q2:" << "---" << " | Q3:" << "---" << std::endl;
    //     }
    // }

    // file.close();
    return 0;
}

void FKfast1(const double* q, double m[][3], double p[])
{

    // double c1 = cos(q[0]), s1 = sin(q[0]), c2 = cos(q[1]), s2 = sin(q[1]);
    // double c3 = cos(q[2]), s3 = sin(q[2]), c4 = cos(q[3]), s4 = sin(q[3]);
    // double c5 = cos(q[4]), s5 = sin(q[4]), c6 = cos(q[5]), s6 = sin(q[5]);
    
    double q2 = q[1] - alpha,    q3 = q[2] - alpha;
    double c1 = cos(q[0]), s1 = sin(q[0]), c2 = cos(q2), s2 = sin(q2);
    double s3_2 = sin(q3-q2), c2_3 = cos(q2 - q3);
    double c4 = cos(q[3]), s4 = sin(q[3]), c5 = cos(q[4]), s5 = sin(q[4]), c6 = cos(q[5]), s6 = sin(q[5]);

    // m[0][0] = c1 * c3 * s2* (c6 * s4 - c4 * c5 * s6) + c1 * c2 * s6 * (c4 * c5 * s3  -  c3 * s5 ) - c5 * s1 * s4 * s6 - c1 * c2 * c6 * s3 * s4 - c4 * c6 * s1  - c1 * s2 * s3 * s5 * s6  ;
    // m[0][1] = c4 * s1 * s6 - c5 * c6 * s1 * s4 - c1 * c2 * c3 * c6 * s5 + c1 * c2 * s3 * s4 * s6 - c1 * c3 * s2 * s4 * s6 - c1 * c6 * s2 * s3 * s5 + c1 * c2 * c4 * c5 * c6 * s3 - c1 * c3 * c4 * c5 * c6 * s2;
    // m[0][2] = c1* c2* c3* c5 - s1 * s4 * s5 + c1 * c5 * s2 * s3 + c1 * c2 * c4 * s3 * s5 - c1 * c3 * c4 * s2 * s5;
    // p[0] = L1 * c1 + Lc * cos(alpha) * c1 * c2 + L4 * c1 * c2 * c3 + L5 * c1 * c2 * s3 - L5 * c1 * c3 * s2 + Lc * c1 * sin(alpha) * s2 + L4 * c1 * s2 * s3 - L6 * s1 * s4 * s5 + L6 * c1 * c5 * s2 * s3 + L6 * c1 * c2 * c3 * c5 + L6 * c1 * c2 * c4 * s3 * s5 - L6 * c1 * c3 * c4 * s2 * s5;


    // m[1][0] = c1 * c4 * c6 + c1 * c5 * s4 * s6 - c2 * c6 * s1 * s3 * s4 + c3 * c6 * s1 * s2 * s4 - c2 * c3 * s1 * s5 * s6 - s1 * s2 * s3 * s5 * s6 + c2 * c4 * c5 * s1 * s3 * s6 - c3 * c4 * c5 * s1 * s2 * s6;
    // m[1][1] = c1 * c5 * c6 * s4 - c1 * c4 * s6 - c2 * c3 * c6 * s1 * s5 + c2 * s1 * s3 * s4 * s6 - c3 * s1 * s2 * s4 * s6 - c6 * s1 * s2 * s3 * s5 + c2 * c4 * c5 * c6 * s1 * s3 - c3 * c4 * c5 * c6 * s1 * s2;
    // m[1][2] = c1 * s4 * s5 + c2 * c3 * c5 * s1 + c5 * s1 * s2 * s3 + c2 * c4 * s1 * s3 * s5 - c3 * c4 * s1 * s2 * s5;
    // p[1] = L1* s1 + Lc * cos(alpha) * c2 * s1 + L4 * c2 * c3 * s1 + L5 * c2 * s1 * s3 - L5 * c3 * s1 * s2 + L6 * c1 * s4 * s5 + Lc * sin(alpha) * s1 * s2 + L4 * s1 * s2 * s3 + L6 * c2 * c3 * c5 * s1 + L6 * c5 * s1 * s2 * s3 + L6 * c2 * c4 * s1 * s3 * s5 - L6 * c3 * c4 * s1 * s2 * s5;

    // m[2][0] = c2 * s3 * s5 * s6 - c6 * s2 * s3 * s4 - c2 * c3 * c6 * s4 - c3 * s2 * s5 * s6 + c2 * c3 * c4 * c5 * s6 + c4 * c5 * s2 * s3 * s6,
    //     m[2][1] = c2 * c3 * s4 * s6 + c2 * c6 * s3 * s5 - c3 * c6 * s2 * s5 + s2 * s3 * s4 * s6 + c2 * c3 * c4 * c5 * c6 + c4 * c5 * c6 * s2 * s3,
    //     m[2][2] = c3 * c5 * s2 - c2 * c5 * s3 + c2 * c3 * c4 * s5 + c4 * s2 * s3 * s5,
    //     p[2] = L5 * c2 * c3 + Lc * cos(alpha) * s2 - Lc * c2 * sin(alpha) - L4 * c2 * s3 + L4 * c3 * s2 + L5 * s2 * s3 - L6 * c2 * c5 * s3 + L6 * c3 * c5 * s2 + L6 * c2 * c3 * c4 * s5 + L6 * c4 * s2 * s3 * s5;
    

    m[0][0] = c1 * s3_2 * (c4 * c5 * s6 - s4 * c6)  - s1 * (s4 * c5 * s6 + c4 * c6)  - c1 * c2_3 * (s5 * s6);
    m[0][1] = c1 * s3_2 * (c4 * c5 * c6 + s4 * s6)  - s1 * (s4 * c5 * c6 - c4 * s6)  - c1 * c2_3 * (s5 * c6);
    m[0][2] = c1 * s3_2 * (c4 * s5)                 - s1 * (s4 * s5)                 - c1 * c2_3 * (-c5);
    p[0]    = c1 * s3_2 * (L6 * c4 * s5)            - s1 * (L6 * s4 * s5)            - c1 * c2_3 * (-L6 * c5 - L4)   + c1 * (L5 * s3_2 + Lc * c2 + L1);
    

    m[1][0] = s1 * s3_2 * (c4 * c5 * s6 - s4 * c6)  + c1 * (s4 * c5 * s6 + c4 * c6)  - s1 * c2_3 * (s5 * s6);     
    m[1][1] = s1 * s3_2 * (c4 * c5 * c6 + s4 * s6)  + c1 * (s4 * c5 * c6 - c4 * s6)  - s1 * c2_3 * (s5 * c6);
    m[1][2] = s1 * s3_2 * (c4 * s5)                 + c1 * (s4 * s5)                 - s1 * c2_3 * (-c5);
    p[1]    = s1 * s3_2 * (L6 * c4 * s5)            + c1 * (L6 * s4 * s5)            - s1 * c2_3 * (-L6 * c5 - L4)   + s1 * (L5 * s3_2 + Lc * c2 + L1);    

    m[2][0] = c2_3 * (c4 * c5 * s6 - s4 * c6)                                        + s3_2 * (s5 * s6);
    m[2][1] = c2_3 * (c4 * c5 * c6 + s4 * s6)                                        + s3_2 * (s5 * c6);
    m[2][2] = c2_3 * (c4 * s5)                                                       + s3_2 * (-c5);
    p[2]    = c2_3 * (L6 * c4 * s5)                                                  + s3_2 * (-L6 * c5 - L4)        + L5 * c2_3 + Lc * s2;
}
void FKfast2(const double* q, double m[][3], double p[])
{

    double c1 = cos(q[0]), s1 = sin(q[0]), c2 = cos(q[1]), s2 = sin(q[1]);
    double c3 = cos(q[2]), s3 = sin(q[2]), c4 = cos(q[3]), s4 = sin(q[3]);
    double c5 = cos(q[4]), s5 = sin(q[4]), c6 = cos(q[5]), s6 = sin(q[5]);
    
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
bool Compare(const double m1[][3], const double p1[], const double m2[][3], const double p2[])
{
    bool ok = true;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (abs(m1[i][j] - m2[i][j]) > 0.00001) {
                std::cout << "Error in matrix at (" << i << "," << j << "): " << m1[i][j] << " != " << m2[i][j] << std::endl;
                ok = false;
            }
        }
        if (abs(p1[i] - p2[i]) > 0.00001) {
            std::cout << "Error in position at index " << i << ": " << p1[i] << " != " << p2[i] << std::endl;
            ok = false;
        }
    }
    return ok;
}

bool IKfast(double* q, const double m[][3], const double p[], bool elbow, bool wrist)
{
    double pm[3] = { p[0] - L6 * m[0][2], p[1] - L6 * m[1][2], p[2] - L6 * m[2][2]};
    
    if (!IKwrist(q, pm[0], pm[1], pm[2], elbow))return false;
    double q1 = q[0], q2 = q[1] - alpha, q3 = q[2] - alpha;
    double s1 = sin(q1), c1 = cos(q1);
    double c2_3 = cos(q2 - q3), s3_2 = sin(q3 - q2);
    double A[3][3]={ {c1 * s3_2, -s1 , -c1 * c2_3}, //A03
                     {s1 * s3_2,  c1 , -s1 * c2_3},
                     {c2_3     ,   0 ,      s3_2}};
    double R[3][3];//R = A.trasp * m
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
            q[5] = atan2(-R[2][1], R[2][0]);
        }
        else {
            q[3] = atan2(-R[1][2], -R[0][2]);
            q[4] = -acos(-R[2][2]); //esto impide abs(q5) > pi/2
            q[5] = atan2(R[2][1], -R[2][0]);
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
bool IKwrist(double* q, double x, double y, double z, bool elbow)
{
    q[0] = atan2(y, x);
    x -= L1 * cos(q[0]);
    y -= L1 * sin(q[0]);

    double num =x * x + y * y + z * z - Lc * Lc - Ld * Ld; 
    const double den = 2 * Lc * Ld;

    if(abs(num) > den) return false; //out of range
    q[2] = (elbow ? 1 : -1) * acos(num / den);
    q[1] = atan2(z, sqrt(x * x + y * y)) + (elbow ? 1 : -1) * atan2(Ld * sin(q[2]), Lc + Ld * cos(q[2]));
    
    q[1] += alpha;
    q[2] += (alpha + beta);
    return true;
}


// bool calculo(double x){
//     double y = 0.0, z = 0.0;
//     double q[3];
    
//     bool elbow = true; 

//     q[0] = atan2(y, x);
//     x -= L1 * cos(q[0]);
//     y -= L1 * sin(q[0]);

//     double num = x * x + y * y + z * z - Lc * Lc - Ld * Ld; //positive q3 (elbow up)
//     const double den = 2 * Lc * Ld;
//     if (abs(num) > den) return false;//out of range
//     q[2] = (elbow ? 1 : -1) * acos(num / den);
//     q[1] = atan2(z, sqrt(x * x + y * y)) + (elbow ? 1 : -1) * atan2(Ld * sin(q[2]), Lc + Ld * cos(q[2]));

//     q[1] += alpha;
//     q[2] += (alpha + beta);

//     //std::cout << "\tQ1 = " << q[0] * rad2deg<< "\tQ2 = " << q[1]*rad2deg << "\tQ3 = " << q[2] * rad2deg<< std::endl;
//     double m[3];
//     q2m(m, q);
//     //std::cout << "\tQ1 = " << m[0] << "\tQ2 = " << m[1] << "\tQ3 = " << m[2] << std::endl;
//     return limits(m, x + L1 * cos(q[0]));
// }

// void q2m(double m[], double q[])
// {
//     m[0] = 180.0 + rad2deg * q[0];
//     m[1] = 180.0 + rad2deg * q[1];
//     m[2] = 90.0 + rad2deg * q[2];
// }

// bool limits(double m[], double x) {
    
//     int mins [] = {70,70,55,0,0,0};
//     int maxs [] = {290,290,260,360,360,360};

//     if( m[0] < mins[0] || m[0] > maxs[0] ||
//         m[1] < mins[1] || m[1] > maxs[1] ||
//         m[2] < mins[2] || m[2] > maxs[2]) {
//         return false; // Fuera de límites
//     }
//     std::cout << "X: = " << x << " |\tQ1 = " << m[0] << "\tQ2 = " << m[1] << "\tQ3 = " << m[2] << std::endl;
//     return true; // Dentro de límites
// }



