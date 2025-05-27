#include <iostream>
#include <math.h>
#include <iomanip>

bool calculo(double x);
void q2m(double m[], double q[]);
bool limits(double m[], double x);

const double Lc = 0.23647621444872635;
const double Ld = 0.28086295590554483;
const double L1 = 0.068;
const double alpha = 0.063473940176806798, beta = 0.078410338738641713;
const double den = 2 * Lc * Ld;
const double rad2deg = 180.0 / M_PI;
int main() {
    // Parámetros del robot según tu imagen
    
    const double step = 0.005; // Incremento de x
    const double max_x = 0.6;
    
    for(float x = 0.0; x <= max_x; x += step) {
        calculo(x);
    }
    //double min_valid = -1, max_valid = -1;
    // for (double x = 0; x <= max_x; x += step) {
    //     double num = std::abs(x * x + y*y+z*z - Lc * Lc - Ld * Ld);
    //     if (num <= den) {
    //         std::cout<< x<<std::endl;
    //         if  (min_valid <0) min_valid =  x;
    //         max_valid = x;
    //     }
    // }

    // std::cout << std::fixed << std::setprecision(6);
    // if (min_valid >= 0) {
    //     std::cout << "Rango válido de x: [" << min_valid << ", " << max_valid << "]" << std::endl;
    // } else {
    //     std::cout << "Ningún valor de x entre 0 y " << max_x << " es válido." << std::endl;
    // }

    return 0;
}
bool calculo(double x){
    double y = 0.0, z = 0.0;
    double q[3];
    
    bool elbow = true; 

    q[0] = atan2(y, x);
    x -= L1 * cos(q[0]);
    y -= L1 * sin(q[0]);

    double num = x * x + y * y + z * z - Lc * Lc - Ld * Ld; //positive q3 (elbow up)
    const double den = 2 * Lc * Ld;
    if (abs(num) > den) return false;//out of range
    q[2] = (elbow ? 1 : -1) * acos(num / den);
    q[1] = atan2(z, sqrt(x * x + y * y)) + (elbow ? 1 : -1) * atan2(Ld * sin(q[2]), Lc + Ld * cos(q[2]));

    q[1] += alpha;
    q[2] += (alpha + beta);

    //std::cout << "\tQ1 = " << q[0] * rad2deg<< "\tQ2 = " << q[1]*rad2deg << "\tQ3 = " << q[2] * rad2deg<< std::endl;
    double m[3];
    q2m(m, q);
    //std::cout << "\tQ1 = " << m[0] << "\tQ2 = " << m[1] << "\tQ3 = " << m[2] << std::endl;
    return limits(m, x + L1 * cos(q[0]));
}

void q2m(double m[], double q[])
{
    m[0] = 180.0 + rad2deg * q[0];
    m[1] = 180.0 + rad2deg * q[1];
    m[2] = 90.0 + rad2deg * q[2];
}

bool limits(double m[], double x) {
    
    int mins [] = {70,70,55,0,0,0};
    int maxs [] = {290,290,260,360,360,360};

    if( m[0] < mins[0] || m[0] > maxs[0] ||
        m[1] < mins[1] || m[1] > maxs[1] ||
        m[2] < mins[2] || m[2] > maxs[2]) {
        return false; // Fuera de límites
    }
    std::cout << "X: = " << x << " |\tQ1 = " << m[0] << "\tQ2 = " << m[1] << "\tQ3 = " << m[2] << std::endl;
    return true; // Dentro de límites
}

